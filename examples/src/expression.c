#ifdef __cplusplus
    extern "C" {
#endif


#include <expression.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "cbool.h"

static lua_State *L = NULL;

static int calc_initLua(void) {
    L = luaL_newstate();
    if (!L) return false;

    luaL_openlibs(L);
    return true;
}

static void calc_destroyLua(void) {
    if (L)
        lua_close(L);
    L = NULL;
}


/* make sure that the expression is valid lua code */
calc_result calc_eval_vL(const char *expr) {
    calc_result result = {
        .value = 0.0F,
        .error = 0
    };

    if (!L && !calc_initLua()) {
        result.error = 1; /* code 1: ERROR */
        return result;
    }

    if (luaL_loadstring(L, expr) != LUA_OK) {
        lua_pop(L, 1); /* de-init it from the lua stack */
        result.error = 1; /* code 1: ERROR */
        return result;
    }

    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        lua_pop(L, 1); /* de-init it from the lua stack */
        result.error = 1; /* code 1: ERROR */
        return result;
    }

    if (!lua_isnumber(L, -1)) {
        lua_pop(L, 1); /* de-init it from the lua stack */
        result.error = 1; /* code 1: ERROR */
        return result;
    }

    result.value = (calc_result_t)lua_tonumber(L, -1);
    lua_pop(L, 1); /* de-init it from the lua stack */

    return result;
}

/* this guy helps with that */
calc_result calc_eval(const char *expr) {

    return (calc_result){ 0, 0 };
}

#ifdef  __cplusplus
    }
#endif
