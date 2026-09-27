#ifdef __cplusplus
    extern "C" {
#endif

/* -------------------------------------------------------------------------------------------------------------------- */

#include <gtk-3.0/gtk/gtk.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

#include <ctype.h>
#include <math.h>
#include <string.h>

#include <stden.h>
#include <cmath.h>

#define RESET "\033[0m"

#define COLOR_INFO "#3877a8"
#define COLOR_ITEM "#525252"
#define COLOR_LOAD "#234f4d"
#define COLOR_WARN "#ffe1ff"

/* upper bound on an expression we are willing to prepare; the prep buffers are
 * stack allocated so this also bounds our stack use */
#define CALC_EXPR_MAX 1024

static GtkApplication *app;
static GtkWidget *window;
static GtkWidget *grid;
static GtkWidget *display_entry;
static lua_State * L;
static str_t arena;

/* the arena holds short-lived colour escapes and log strings only. Anything that has to
 * outlive the statement which wrote it goes on the stack, because the arena is rewound by
 * str_resetArena and two callers both believing they own arena.buffer[0] is how you end up
 * printing an escape sequence where a number belongs.
 */

/* -------------------------------------------------------------------------------------------------------------------- */
/* expression preparation                                                             */
/* -------------------------------------------------------------------------------------------------------------------- */

/* Names Lua resolves to functions. When one of these sits directly in front of a `(` the
 * parenthesis is a call, not an implicit multiplication, so no `*` may be inserted there.
 * Keep in step with the globals installed by calc__register_lua. */
static const char *const CALC_FUNCS[] = {
    "sin", "cos", "tan", "asin", "acos", "atan",
    "sinh", "cosh", "tanh",
    "sqrt", "cbrt", "fact", "abs", "min", "max",
    "log", "ln", "exp", "floor", "ceil", "round",
    "gcd", "lcm", "sgn", "mod", "hypot", "deg", "rad",
    NULL
};

static bool calc_is_word(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static bool calc_is_digit(char c) {
    return isdigit((unsigned char)c);
}

static bool calc_is_name(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

/* a character that can end an operand: a literal, a name, or a closed bracket */
static bool calc_is_value_end(char c) {
    return calc_is_word(c) || c == ')';
}

/* a character that can begin an operand */
static bool calc_is_value_start(char c) {
    return calc_is_word(c) || c == '(';
}

static bool calc_ident_is_func(const char *s, size_t n) {
    for (int i = 0; CALC_FUNCS[i] != NULL; i++)
        if (strlen(CALC_FUNCS[i]) == n && strncmp(CALC_FUNCS[i], s, n) == 0)
            return true;
    return false;
}

/* Insert the `*` that Lua's grammar demands but people leave out, so
 *     2(1 * 2)  ->  2*(1*2)
 *     (2)(3)    ->  (2)*(3)
 *     2(3)4     ->  2*(3)*4
 *     2sin(x)   ->  2*sin(x)
 * while leaving sqrt(2), 10%3 and 2*3 alone. Returns the length written, -1 on overflow. */
static int calc_expand_mul(char *out, size_t outsz, const char *in) {
    size_t o = 0;
    char prev = '\0';

    for (size_t i = 0; in[i] != '\0'; i++) {
        char c = in[i];

        if (isspace((unsigned char)c)) {
            /* drop the run of spaces so `2 (3)` becomes `2*(3)` */
            while (isspace((unsigned char)in[i + 1]))
                i++;
            continue;
        }

        bool insert = false;

        if (prev != '\0') {
            if (calc_is_digit(prev) && calc_is_digit(c)) {
                /* one literal: 10 is ten, not 1*0 */
            } else if (calc_is_name(prev) && calc_is_name(c)) {
                /* one identifier: sqrt must not become s*q*r*t */
            } else if (prev == '%' || c == '%') {
                /* % already binds, and the pad's % is a postfix percent, so never pad it */
            } else if (calc_is_value_end(prev) && calc_is_value_start(c)) {
                if (c == '(' && calc_is_word(prev)) {
                    /* the identifier in front of the paren decides: a name is a call, a
                     * number is a factor, e.g. 2( multiplies but sin( calls. Walk back over
                     * name characters only, so `2sin(` yields "sin" and not "2sin". */
                    size_t s = i;
                    while (s > 0 && calc_is_name(in[s - 1]))
                        s--;
                    insert = !calc_ident_is_func(in + s, i - s);
                } else {
                    insert = true;
                }
            }
        }

        if (insert) {
            if (o + 2 >= outsz) return -1;
            out[o++] = '*';
        }

        if (o + 1 >= outsz) return -1;
        out[o++] = c;
        prev = c;
    }

    out[o] = '\0';
    return (int)o;
}

/* Close whatever the user left open, so a button which emits a trailing `(` (sqrt, fact,
 * x^...) does not turn `=` into a syntax error. Returns the length written, -1 on overflow. */
static int calc_balance_parens(char *out, size_t outsz, const char *in) {
    size_t o = 0;
    int depth = 0;

    for (size_t i = 0; in[i] != '\0'; i++) {
        if (o + 1 >= outsz) return -1;
        out[o++] = in[i];
        if (in[i] == '(')
            depth++;
        else if (in[i] == ')' && depth > 0)
            depth--;
    }

    while (depth-- > 0) {
        if (o + 1 >= outsz) return -1;
        out[o++] = ')';
    }

    out[o] = '\0';
    return (int)o;
}

/* -------------------------------------------------------------------------------------------------------------------- */
/* button model                                                                         */
/* -------------------------------------------------------------------------------------------------------------------- */

/* trig cycles through these */
static const char *const CALC_TRIG_CYCLE[] = { "sin", "cos", "tan", NULL };

static GtkWidget *trig_button = NULL;

typedef enum {
    CALC_EMIT,      /* append token to the entry */
    CALC_CLEAR,     /* empty the entry */
    CALC_EQUALS,    /* evaluate */
    CALC_BACKSPACE, /* drop the last character */
    CALC_NEGATE,    /* toggle the sign of the trailing number */
    CALC_TRIG,      /* insert the next function in the trig cycle */
    CALC_PLACEHOLDER
} calc_kind;

typedef struct {
    const char *label; /* what the button shows */
    const char *token; /* for CALC_EMIT, what actually goes in the entry */
    calc_kind kind;
} calc_btn;

/* A button shows a glyph; it does not necessarily insert that glyph. `%` has to become a
 * postfix /100 so that 200+50% reads 200.5, and x! has to become fact( so Lua gets a call
 * rather than the two characters the user pressed. Matching on label text is what let `trig`
 * type the literal word "trig" into the entry and hand it to the evaluator as garbage.
 * The two modulus keys are deliberately distinct: `%` is postfix percent, `|x|` is the
 * infix remainder, so 10 |x| 3 is 1 and 10 % 3 is 0.1. */
static const calc_btn CALC_PAD[6][5] = {
    { {"C",      NULL,      CALC_CLEAR},
      {"ⁿ√x",    "^(1/",    CALC_EMIT},
      {"xⁿ",     "^(",      CALC_EMIT},
      {"%",      "/100",    CALC_EMIT},
      {"trig",   NULL,      CALC_TRIG} },

    { {"⌫",      NULL,      CALC_BACKSPACE},
      {"(",      "(",       CALC_EMIT},
      {")",      ")",       CALC_EMIT},
      {"√x",     "sqrt(",   CALC_EMIT},
      {"+",      "+",       CALC_EMIT} },

    { {"7",      "7",       CALC_EMIT},
      {"8",      "8",       CALC_EMIT},
      {"9",      "9",       CALC_EMIT},
      {"x!",     "fact(",   CALC_EMIT},
      {"-",      "-",       CALC_EMIT} },

    { {"4",      "4",       CALC_EMIT},
      {"5",      "5",       CALC_EMIT},
      {"6",      "6",       CALC_EMIT},
      {"advanced", NULL,    CALC_PLACEHOLDER},
      {"*",      "*",       CALC_EMIT} },

    { {"1",      "1",       CALC_EMIT},
      {"2",      "2",       CALC_EMIT},
      {"3",      "3",       CALC_EMIT},
      {"00",     "00",      CALC_EMIT},
      {"÷",      "/",       CALC_EMIT} },

    { {"±",      NULL,      CALC_NEGATE},
      {"0",      "0",       CALC_EMIT},
      {".",      ".",       CALC_EMIT},
      {"|x|",    "%",       CALC_EMIT},
      {"=",      NULL,      CALC_EQUALS} }
};

/* -------------------------------------------------------------------------------------------------------------------- */
/* callbacks                                                                            */
/* -------------------------------------------------------------------------------------------------------------------- */

static void calc__log(const char *level, const char *color, const char *prefix, const char *detail) {
    str_resetArena(&arena);
    printf("%s[%s] :%s %s%s%s%s\n",
        hex_to_ansi(color, &arena, false).pointer, level, RESET,
        hex_to_ansi(COLOR_ITEM, &arena, false).pointer, prefix, detail, RESET);
}

static void calc__insert(const char *token) {
    if (!display_entry || token == NULL)
        return;
    const gchar *current = gtk_entry_get_text(GTK_ENTRY(display_entry));
    gchar *next = g_strconcat(current, token, NULL);
    gtk_entry_set_text(GTK_ENTRY(display_entry), next);
    g_free(next);
}

static void calc__cbk_EMIT(GtkWidget *button, gpointer user_data) {
    calc__insert((const char *)user_data);
}

static void calc__cbk_CLEAR(GtkWidget *button, gpointer user_data) {
    if (display_entry)
        gtk_entry_set_text(GTK_ENTRY(display_entry), "");
}

static void calc__cbk_BACKSPACE(GtkWidget *button, gpointer user_data) {
    if (!display_entry)
        return;
    const gchar *current = gtk_entry_get_text(GTK_ENTRY(display_entry));
    gsize len = strlen(current);
    if (len == 0) /* the original strlen(current) - 1 underflowed here and built a 2^64 string */
        return;
    gchar *next = g_strndup(current, len - 1);
    gtk_entry_set_text(GTK_ENTRY(display_entry), next);
    g_free(next);
}

/* Toggle the sign of the number the cursor sits in. Lua reads a run of operator characters
 * as separate operators, so 2*-3 and 2--3 are both legal, which means we never have to
 * parenthesise: the only choice is drop an existing sign, flip a binary one, or add one. */
static void calc__cbk_NEGATE(GtkWidget *button, gpointer user_data) {
    if (!display_entry)
        return;

    gchar *t = g_strdup(gtk_entry_get_text(GTK_ENTRY(display_entry)));
    if (t == NULL)
        return;

    gsize len = strlen(t);
    gsize n = len;
    while (n > 0 && (g_ascii_isdigit(t[n - 1]) || t[n - 1] == '.'))
        n--;

    if (n == len) { /* no trailing number to work on */
        g_free(t);
        return;
    }

    if (t[n] == '-') {
        memmove(t + n, t + n + 1, len - n);
    } else if (n > 0 && (t[n - 1] == '+' || t[n - 1] == '-')) {
        /* the sign is binary if the character before it can end an operand, else it is a
         * unary sign sitting on the number and should simply come off */
        bool binary = (n == 1) || calc_is_value_end(t[n - 2]);
        if (binary)
            t[n - 1] = (t[n - 1] == '+') ? '-' : '+';
        else
            memmove(t + n - 1, t + n, len - n + 1);
    } else {
        gchar *next = g_malloc0(len + 2);
        memcpy(next, t, n);
        next[n] = '-';
        memcpy(next + n + 1, t + n, len - n + 1);
        g_free(t);
        t = next;
    }

    gtk_entry_set_text(GTK_ENTRY(display_entry), t);
    g_free(t);
}

static void calc__cbk_TRIG(GtkWidget *button, gpointer user_data) {
    const char *name = CALC_TRIG_CYCLE[0];
    char token[8];

    snprintf(token, sizeof token, "%s(", name);
    calc__insert(token);

    /* rotate the cycle and show what the next press will insert */
    size_t n = sizeof CALC_TRIG_CYCLE / sizeof CALC_TRIG_CYCLE[0] - 1;
    for (size_t i = 0; CALC_TRIG_CYCLE[i] != NULL; i++) {
        if (CALC_TRIG_CYCLE[i] == name) {
            name = CALC_TRIG_CYCLE[(i + 1) % n];
            break;
        }
    }

    if (trig_button) {
        char label[16];
        snprintf(label, sizeof label, " %s ", name);
        gtk_button_set_label(GTK_BUTTON(trig_button), label);
    }
}

static void calc__cbk_EQUALS(GtkWidget *button, gpointer user_data) {
    if (!display_entry)
        return;

    const char *raw = gtk_entry_get_text(GTK_ENTRY(display_entry));

    char expanded[CALC_EXPR_MAX];
    if (calc_expand_mul(expanded, sizeof expanded, raw) < 0) {
        calc__log("ERROR", COLOR_WARN, "expression too long to prepare: ", "");
        return;
    }

    char balanced[CALC_EXPR_MAX];
    int n = calc_balance_parens(balanced, sizeof balanced, expanded);
    if (n < 0) {
        calc__log("ERROR", COLOR_WARN, "expression too long to prepare: ", "");
        return;
    }
    if (n == 0) {
        calc__log("INFO", COLOR_INFO, "nothing to evaluate: ", "");
        return;
    }

    char code[CALC_EXPR_MAX + 16];
    snprintf(code, sizeof code, "return (%s)", balanced);

    calc__log("DEBUG", COLOR_INFO, "prepared: ", code);

    if (luaL_dostring(L, code) != LUA_OK) {
        /* the original only handled LUA_OK, so a bad expression failed in silence */
        const char *err = lua_tostring(L, -1);
        calc__log("ERROR", COLOR_WARN, "lua: ", err ? err : "unknown error");
        gtk_entry_set_text(GTK_ENTRY(display_entry), err ? err : "error");
        lua_pop(L, 1);
        return;
    }

    if (!lua_isnumber(L, -1)) {
        calc__log("ERROR", COLOR_WARN, "did not evaluate to a number: ", "");
        lua_pop(L, 1);
        return;
    }

    double result = (double)lua_tonumber(L, -1);
    lua_pop(L, 1);

    char display[64];
    if (isnan(result))
        snprintf(display, sizeof display, "nan");
    else if (isinf(result))
        snprintf(display, sizeof display, result > 0 ? "inf" : "-inf");
    else
        snprintf(display, sizeof display, "%.14g", result);

    calc__log("DEBUG", COLOR_INFO, "result: ", display);
    gtk_entry_set_text(GTK_ENTRY(display_entry), display);
}

static void calc__cbk_PLACEHOLDER(GtkWidget *button, gpointer user_data) {
    calc__log("INFO", COLOR_INFO, "advanced is not wired up yet: ", "");
}

static void calc__wire(GtkWidget *btn, const calc_btn *spec) {
    switch (spec->kind) {
    case CALC_CLEAR:     g_signal_connect(btn, "clicked", G_CALLBACK(calc__cbk_CLEAR), NULL); break;
    case CALC_EQUALS:    g_signal_connect(btn, "clicked", G_CALLBACK(calc__cbk_EQUALS), NULL); break;
    case CALC_BACKSPACE: g_signal_connect(btn, "clicked", G_CALLBACK(calc__cbk_BACKSPACE), NULL); break;
    case CALC_NEGATE:    g_signal_connect(btn, "clicked", G_CALLBACK(calc__cbk_NEGATE), NULL); break;
    case CALC_TRIG:      g_signal_connect(btn, "clicked", G_CALLBACK(calc__cbk_TRIG), NULL); break;
    case CALC_PLACEHOLDER:g_signal_connect(btn, "clicked", G_CALLBACK(calc__cbk_PLACEHOLDER), NULL); break;
    case CALC_EMIT:
    default:             g_signal_connect(btn, "clicked", G_CALLBACK(calc__cbk_EMIT), (gpointer)spec->token); break;
    }
}

/* -------------------------------------------------------------------------------------------------------------------- */
/* lua setup                                                                            */
/* -------------------------------------------------------------------------------------------------------------------- */

/* the tokens the pad emits call these as globals, so bind them once up front */
static const char *CALC_MATH =
    "sqrt = math.sqrt\n"
    "sin = math.sin\n"
    "cos = math.cos\n"
    "tan = math.tan\n"
    "function fact(n)\n"
    "  if n < 0 or n ~= math.floor(n) then error('fact: expected a non-negative integer') end\n"
    "  local r = 1\n"
    "  for i = 2, n do r = r * i end\n"
    "  return r\n"
    "end\n";

static void calc__register_lua(void) {
    if (luaL_dostring(L, CALC_MATH) != LUA_OK) {
        calc__log("WARN", COLOR_WARN, "failed to bind math globals: ", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

/* -------------------------------------------------------------------------------------------------------------------- */

void activate(GtkApplication *app, gpointer user_data) {
    /* Initialize window */
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 440, 540);
    gtk_container_set_border_width(GTK_CONTAINER(window), 12);

    /* Main vertical box to hold display entry and grid */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Display / Result Entry */
    display_entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(display_entry), TRUE);
    gtk_entry_set_alignment(GTK_ENTRY(display_entry), 1.0f);
    gtk_widget_set_size_request(display_entry, -1, 50);
    gtk_box_pack_start(GTK_BOX(vbox), display_entry, FALSE, FALSE, 0);

    /* Initialize grid for buttons */
    grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 5; col++) {
            const calc_btn *spec = &CALC_PAD[row][col];

            GtkWidget *btn = gtk_button_new_with_label(spec->label);
            calc__wire(btn, spec);

            if (spec->kind == CALC_TRIG)
                trig_button = btn;

            gtk_grid_attach(GTK_GRID(grid), btn, col, row, 1, 1);
        }
    }

    /* Show all widgets */
    gtk_widget_show_all(window);

    if (trig_button)
        gtk_button_set_label(GTK_BUTTON(trig_button), " cos ");
}

int main() {
    str_initArena(&arena, NULL);

    L = luaL_newstate();
    luaL_openlibs(L);
    calc__register_lua();

    if (luaL_dofile(L, "scripts/config.lua") != LUA_OK) {
        lua_pop(L , 1);
        printf("%s[WARN] :%s Failed to load config.lua\n", hex_to_ansi(COLOR_WARN, &arena, false).pointer, RESET);
        printf("%s[WARN] :%s No config loaded, using defaults\n", hex_to_ansi(COLOR_WARN, &arena, false).pointer, RESET);
    }


    printf("%s[INFO] :%s Starting calculator...\n", hex_to_ansi(COLOR_INFO, &arena, false).pointer, RESET);
    printf("%s[INFO] :%s Specs: \n", hex_to_ansi(COLOR_INFO, &arena, false).pointer, RESET);
    printf("%s  - GTK+ 3.0 %s................ %s[loaded]\n" RESET, hex_to_ansi(COLOR_ITEM, &arena, false).pointer, RESET, hex_to_ansi(COLOR_LOAD, &arena, false).pointer);
    printf("%s  - Lua JIT %s................. %s[loaded]\n" RESET, hex_to_ansi(COLOR_ITEM, &arena, false).pointer, RESET, hex_to_ansi(COLOR_LOAD, &arena, false).pointer);
    printf("%s  - CEN (vendor, stdlib) %s.... %s[loaded]\n" RESET, hex_to_ansi(COLOR_ITEM, &arena, false).pointer, RESET, hex_to_ansi(COLOR_LOAD, &arena, false).pointer);
    printf("-------------------------starting log-----------------------\n");
    printf("%s[INFO] :%s Starting GTK+ 3.0. Initializing window and activation.\n" RESET, hex_to_ansi(COLOR_INFO, &arena, false).pointer, RESET);

    app = gtk_application_new("yy.calc.gtk.example.cbp.yutils", G_APPLICATION_DEFAULT_FLAGS);

    printf("%s[INFO] :%s Started %sGTK+ 3.0%s. Connecting signal to activate...\n", hex_to_ansi(COLOR_INFO, &arena, false).pointer,
        hex_to_ansi(COLOR_LOAD, &arena, false).pointer,
        hex_to_ansi(COLOR_ITEM, &arena, false).pointer, RESET);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    printf("%s[INFO] :%s Application initialized. %sRunning...\n" RESET,  hex_to_ansi(COLOR_INFO, &arena, false).pointer,
        hex_to_ansi(COLOR_LOAD, &arena, false).pointer, RESET);

    int status = g_application_run(G_APPLICATION(app), 0, NULL);

    printf("%s[INFO] :%s Application exited.%s Cleaning up...\n" RESET,   hex_to_ansi(COLOR_INFO, &arena, false).pointer,
        hex_to_ansi(COLOR_LOAD, &arena, false).pointer, RESET);

    if (L) lua_close(L);

    printf("%s[INFO] :%s Finished cleaning up lua instance(s)%s. Now cleaning up GTK+ 3.0...\n", hex_to_ansi(COLOR_INFO, &arena, false).pointer, hex_to_ansi(COLOR_LOAD, &arena, false).pointer, RESET);

    g_object_unref(app);

    return status;
}

/* -------------------------------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
    }
#endif
