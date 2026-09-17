/*
 * Raylib + Lua Calculator
 * Customizable via scripts/app.lua (or fallback defaults).
 */

#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EXPR 256

// Theme and Config structures
typedef struct {
    Color background;
    Color surface;
    Color button;
    Color button_hover;
    Color button_down;
    Color number_fg;
    Color operator_fg;
    Color danger_fg;
    Color accent;
    Color display_fg;
    Color dim_fg;
    int radius;
} Theme;

typedef struct {
    char title[64];
    int width;
    int height;
    Theme theme;
    int font_size;
} Config;

// Helper: parse hex color "#RRGGBB" to Raylib Color
Color HexToColor(const char* hex, Color defaultColor) {
    if (!hex || hex[0] != '#' || strlen(hex) < 7) return defaultColor;
    unsigned int r, g, b;
    if (sscanf(hex + 1, "%02x%02x%02x", &r, &g, &b) == 3) {
        return (Color){ (unsigned char)r, (unsigned char)g, (unsigned char)b, 255 };
    }
    return defaultColor;
}

// Default config
void LoadDefaultConfig(Config* cfg) {
    strcpy(cfg->title, "Calculator (Raylib + Lua)");
    cfg->width = 400;
    cfg->height = 600;
    cfg->font_size = 28;

    cfg->theme.background   = (Color){ 17,  17,  20,  255 };
    cfg->theme.surface      = (Color){ 27,  29,  35,  255 };
    cfg->theme.button       = (Color){ 38,  40,  51,  255 };
    cfg->theme.button_hover = (Color){ 51,  53,  74,  255 };
    cfg->theme.button_down  = (Color){ 61,  63,  86,  255 };
    cfg->theme.number_fg    = (Color){ 230, 232, 239, 255 };
    cfg->theme.operator_fg  = (Color){ 122, 162, 247, 255 };
    cfg->theme.danger_fg    = (Color){ 247, 118, 142, 255 };
    cfg->theme.accent       = (Color){ 122, 162, 247, 255 };
    cfg->theme.display_fg   = (Color){ 230, 232, 239, 255 };
    cfg->theme.dim_fg       = (Color){ 139, 144, 166, 255 };
    cfg->theme.radius       = 12;
}

// Load config from Lua script
void LoadLuaConfig(lua_State* L, Config* cfg) {
    LoadDefaultConfig(cfg);

    const char* paths[] = {
        "cbp/examples/scripts/app.lua",
        "examples/scripts/app.lua",
        "scripts/app.lua",
        "app.lua"
    };

    int loaded = 0;
    for (int i = 0; i < 4; i++) {
        if (luaL_dofile(L, paths[i]) == LUA_OK) {
            loaded = 1;
            break;
        } else {
            lua_pop(L, 1); // pop error
        }
    }

    if (!loaded) {
        printf("Warning: Could not load app.lua. Using defaults.\n");
        return;
    }

    // Expecting table on top of stack
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    // window = { title = "...", width = ..., height = ... }
    lua_getfield(L, -1, "window");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "title");
        if (lua_isstring(L, -1)) {
            strncpy(cfg->title, lua_tostring(L, -1), sizeof(cfg->title) - 1);
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "width");
        if (lua_isnumber(L, -1)) cfg->width = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "height");
        if (lua_isnumber(L, -1)) cfg->height = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    // theme = { background = "...", ... }
    lua_getfield(L, -1, "theme");
    if (lua_istable(L, -1)) {
        #define PARSE_COLOR(field, var) \
            lua_getfield(L, -1, field); \
            if (lua_isstring(L, -1)) cfg->theme.var = HexToColor(lua_tostring(L, -1), cfg->theme.var); \
            lua_pop(L, 1);

        PARSE_COLOR("background", background);
        PARSE_COLOR("surface", surface);
        PARSE_COLOR("button", button);
        PARSE_COLOR("button_hover", button_hover);
        PARSE_COLOR("button_down", button_down);
        PARSE_COLOR("number_fg", number_fg);
        PARSE_COLOR("operator_fg", operator_fg);
        PARSE_COLOR("danger_fg", danger_fg);
        PARSE_COLOR("accent", accent);
        PARSE_COLOR("display_fg", display_fg);
        PARSE_COLOR("dim_fg", dim_fg);

        lua_getfield(L, -1, "radius");
        if (lua_isnumber(L, -1)) cfg->theme.radius = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    // font = { size = ... }
    lua_getfield(L, -1, "font");
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "size");
        if (lua_isnumber(L, -1)) cfg->font_size = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_pop(L, 1); // pop root config table
}

// Evaluate expression via Lua
void EvaluateExpression(lua_State* L, const char* expr, char* output, size_t max_out) {
    if (strlen(expr) == 0) {
        strcpy(output, "0");
        return;
    }

    char code[MAX_EXPR + 32];
    snprintf(code, sizeof(code), "return (%s)", expr);

    if (luaL_dostring(L, code) != LUA_OK) {
        // Try calling global evaluate if available
        lua_getglobal(L, "evaluate");
        if (lua_isfunction(L, -1)) {
            lua_pushstring(L, expr);
            if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
                if (lua_isnumber(L, -1)) {
                    double val = lua_tonumber(L, -1);
                    snprintf(output, max_out, "%.10g", val);
                    lua_pop(L, 1);
                    return;
                }
            }
            lua_pop(L, 1);
        } else {
            lua_pop(L, 1); // pop non-function
        }

        snprintf(output, max_out, "Error");
        return;
    }

    if (lua_isnumber(L, -1)) {
        double val = lua_tonumber(L, -1);
        snprintf(output, max_out, "%.10g", val);
    } else {
        snprintf(output, max_out, "Error");
    }
    lua_pop(L, 1);
}

int main(void) {
    // Initialize Lua state
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    Config cfg;
    LoadLuaConfig(L, &cfg);

    InitWindow(cfg.width, cfg.height, cfg.title);
    SetTargetFPS(60);

    char expr[MAX_EXPR] = "";
    char resultStr[MAX_EXPR] = "0";
    bool evaluated = false;

    // font
    Font font = LoadFontEx("assets/fonts/mono.ttf", 64, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    // Layout buttons
    const char* buttons[5][4] = {
        { "C", "(", ")", "/" },
        { "7", "8", "9", "*" },
        { "4", "5", "6", "-" },
        { "1", "2", "3", "+" },
        { "0", ".", "⌫", "=" }
    };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // UI Layout dimensions
        float margin = 16.0f;
        float displayHeight = 100.0f;
        float topOffset = margin + displayHeight + margin;
        float availWidth = cfg.width - (margin * 2);
        float availHeight = cfg.height - topOffset - margin;

        float btnWidth = (availWidth - (3 * 10.0f)) / 4.0f;
        float btnHeight = (availHeight - (4 * 10.0f)) / 5.0f;

        // Handle button clicks & interactions
        int clickedRow = -1, clickedCol = -1;
        if (mousePressed) {
            for (int r = 0; r < 5; r++) {
                for (int c = 0; c < 4; c++) {
                    float bx = margin + c * (btnWidth + 10.0f);
                    float by = topOffset + r * (btnHeight + 10.0f);
                    Rectangle rect = { bx, by, btnWidth, btnHeight };
                    if (CheckCollisionPointRec(mousePos, rect)) {
                        clickedRow = r;
                        clickedCol = c;
                        break;
                    }
                }
                if (clickedRow != -1) break;
            }
        }

        if (clickedRow != -1 && clickedCol != -1) {
            const char* btn = buttons[clickedRow][clickedCol];
            if (strcmp(btn, "C") == 0) {
                expr[0] = '\0';
                strcpy(resultStr, "0");
                evaluated = false;
            } else if (strcmp(btn, "⌫") == 0) {
                if (evaluated) {
                    expr[0] = '\0';
                    strcpy(resultStr, "0");
                    evaluated = false;
                } else {
                    int len = strlen(expr);
                    if (len > 0) expr[len - 1] = '\0';
                }
            } else if (strcmp(btn, "=") == 0) {
                if (strlen(expr) > 0) {
                    EvaluateExpression(L, expr, resultStr, sizeof(resultStr));
                    evaluated = true;
                }
            } else {
                if (evaluated) {
                    // If starting new input after equals, keep result as start or clear
                    if (strcmp(btn, "+") == 0 || strcmp(btn, "-") == 0 || strcmp(btn, "*") == 0 || strcmp(btn, "/") == 0) {
                        strcpy(expr, resultStr);
                    } else {
                        expr[0] = '\0';
                    }
                    evaluated = false;
                }
                if (strlen(expr) + strlen(btn) < MAX_EXPR - 1) {
                    strcat(expr, btn);
                }
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(cfg.theme.background);

        // Display screen
        Rectangle displayRect = { margin, margin, availWidth, displayHeight };
        DrawRectangleRounded(displayRect, cfg.theme.radius / displayHeight, 4, cfg.theme.surface);

        // Draw expression / result text
        const char* displayText = strlen(expr) > 0 ? expr : "0";
        int fontSize = cfg.font_size;
        Vector2 textSize = MeasureTextEx(font, displayText, (float)fontSize, 1.0f);
        while (textSize.x > availWidth - 32 && fontSize > 16) {
            fontSize -= 2;
            textSize = MeasureTextEx(font, displayText, (float)fontSize, 1.0f);
        }
        DrawTextEx(font, displayText, (Vector2){displayRect.x + displayRect.width - textSize.x - 16, displayRect.y + 20}, (float)fontSize, 1.0f, cfg.theme.display_fg);

        if (evaluated) {
            Vector2 resSize = MeasureTextEx(font, resultStr, 20.0f, 1.0f);
            DrawTextEx(font, resultStr, (Vector2){displayRect.x + displayRect.width - resSize.x - 16, displayRect.y + 65}, 20.0f, 1.0f, cfg.theme.dim_fg);
        }

        // Draw buttons
        for (int r = 0; r < 5; r++) {
            for (int c = 0; c < 4; c++) {
                float bx = margin + c * (btnWidth + 10.0f);
                float by = topOffset + r * (btnHeight + 10.0f);
                Rectangle rect = { bx, by, btnWidth, btnHeight };

                bool hovered = CheckCollisionPointRec(mousePos, rect);
                bool down = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

                Color btnColor = cfg.theme.button;
                if (down) btnColor = cfg.theme.button_down;
                else if (hovered) btnColor = cfg.theme.button_hover;

                DrawRectangleRounded(rect, cfg.theme.radius / btnHeight, 4, btnColor);

                const char* btnText = buttons[r][c];
                Color fgColor = cfg.theme.number_fg;
                if (strcmp(btnText, "/") == 0 || strcmp(btnText, "*") == 0 ||
                    strcmp(btnText, "-") == 0 || strcmp(btnText, "+") == 0 ||
                    strcmp(btnText, "=") == 0) {
                    fgColor = cfg.theme.operator_fg;
                } else if (strcmp(btnText, "C") == 0 || strcmp(btnText, "⌫") == 0) {
                    fgColor = cfg.theme.danger_fg;
                }

                int bFontSize = 26;
                Vector2 bTextSize = MeasureTextEx(font, btnText, (float)bFontSize, 1.0f);
                DrawTextEx(font, btnText, (Vector2){rect.x + (rect.width - bTextSize.x) / 2, rect.y + (rect.height - bTextSize.y) / 2}, (float)bFontSize, 1.0f, fgColor);
            }
        }

        EndDrawing();
    }

    lua_close(L);
    CloseWindow();
    return 0;
}
