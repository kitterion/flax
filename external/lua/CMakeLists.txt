cmake_minimum_required(VERSION 3.1)
project(lua C)

set(SRC_H
  src/lapi.h
  src/lauxlib.h
  src/lcode.h
  src/lctype.h
  src/ldebug.h
  src/ldo.h
  src/lfunc.h
  src/lgc.h
  src/llex.h
  src/llimits.h
  src/lmem.h
  src/lobject.h
  src/lopcodes.h
  src/lparser.h
  src/lprefix.h
  src/lstate.h
  src/lstring.h
  src/ltable.h
  src/ltm.h
  src/luaconf.h
  src/lua.h
  src/lua.hpp
  src/lualib.h
  src/lundump.h
  src/lvm.h
  src/lzio.h
)

set(SRC_C
  src/lapi.c
  src/lauxlib.c
  src/lbaselib.c
  src/lbitlib.c
  src/lcode.c
  src/lcorolib.c
  src/lctype.c
  src/ldblib.c
  src/ldebug.c
  src/ldo.c
  src/ldump.c
  src/lfunc.c
  src/lgc.c
  src/linit.c
  src/liolib.c
  src/llex.c
  src/lmathlib.c
  src/lmem.c
  src/loadlib.c
  src/lobject.c
  src/lopcodes.c
  src/loslib.c
  src/lparser.c
  src/lstate.c
  src/lstring.c
  src/lstrlib.c
  src/ltable.c
  src/ltablib.c
  src/ltm.c
  src/lundump.c
  src/lutf8lib.c
  src/lvm.c
  src/lzio.c
)

set(C_STANDARD 99)
set(C_STANDARD_REQUIRED ON)

add_library(lua ${SRC_C} ${SRC_H})
target_compile_definitions(lua PUBLIC LUA_COMPAT_5_1 LUA_USE_LINUX)
target_include_directories(lua PUBLIC src)
target_link_libraries(lua PRIVATE dl)
