set(SRC_HXX
  lua_data.h
  macros.h
)

set(SRC_CXX
  lua_data.cpp
  main.cpp
)

set(SRC_TESTS
)

add_executable(flax ${SRC_CXX} ${SRC_HXX})
target_link_libraries(flax lua sol)
