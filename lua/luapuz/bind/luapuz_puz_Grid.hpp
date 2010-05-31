// ---------------------------------------------------------------------------
// luapuz_puz_Grid.hpp was generated by puzbind.lua
//
// Any changes made to this file will be lost when the file is regenerated.
// ---------------------------------------------------------------------------

#ifndef luapuz_puz_Grid_hpp
#define luapuz_puz_Grid_hpp

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "../luapuz_functions.hpp"
#include "../luapuz_tracking.hpp"

// ---------------------------------------------------------------------------
// class Grid
// ---------------------------------------------------------------------------

#include "puz/Grid.hpp"

LUAPUZ_API extern const char * Grid_meta;

// Grid userdata
// Userdata member should_gc is used to suppress garbage collection of the
// actual data.
// Calling luapuz_newGrid() with default parameters enables
// garbage collection.
// Calling luapuz_pushGrid/Ref() with default parameters
// suppresses garbage collection.
struct LUAPUZ_API Grid_ud
{
    puz::Grid * grid;
    bool should_gc;
};

// Get the userdata
inline Grid_ud * luapuz_checkGrid_ud(lua_State * L, int index)
{
    return (Grid_ud *)luaL_checkudata(L, index, Grid_meta);
}

// Get the actual data
inline puz::Grid * luapuz_checkGrid(lua_State * L, int index)
{
    Grid_ud * ud = luapuz_checkGrid_ud(L, index);
    if (! ud->grid)
        luaL_typerror(L, index, Grid_meta);
    return ud->grid;
}


// Check if this is the correct data type
inline bool luapuz_isGrid(lua_State *L, int index, const char *tname)
{
    return luapuz_isudata(L, index, tname);
}

// Create a new userdata with actual data and push it on the stack.
// The userdata will be tracked in the tracked objects table.
LUAPUZ_API void luapuz_newGrid(lua_State * L, puz::Grid * grid, bool should_gc = true);

// Push the actual data.
// If we have already tracked this userdata, push that userdata.
inline void luapuz_pushGrid(lua_State * L, puz::Grid * grid, bool should_gc = false)
{
    if (! grid)
        lua_pushnil(L);
    else if (! luapuz_push_tracked_object(L, grid))
        luapuz_newGrid(L, grid, should_gc);
}



// enum GridState
//------------

LUAPUZ_API extern const char * GridState_meta;

inline puz::Grid::GridState luapuz_checkGridState(lua_State * L, int index)
{
    return static_cast<puz::Grid::GridState>(luapuz_checkEnum(L, index, GridState_meta));
}

inline void luapuz_pushGridState(lua_State * L, puz::Grid::GridState gridstate)
{
    lua_pushnumber(L, gridstate);
}

inline bool luapuz_isGridState(lua_State * L, int index)
{
    return luapuz_isEnum(L, index, GridState_meta);
}

void luapuz_openGridlib (lua_State *L);
#endif // luapuz_puz_Grid_hpp
