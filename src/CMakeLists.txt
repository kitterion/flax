set(SRC_HXX
  lua_data.h
  macros.h
)

set(SRC_CXX
  lua_data.cpp
)

set(SRC_TESTS
  lua_data_test.cpp
)

add_executable(flax main.cpp ${SRC_CXX} ${SRC_HXX})
target_link_libraries(flax lua sol)

add_executable(flax_tests ${SRC_TESTS} ${SRC_HXX} ${SRC_CXX})
target_link_libraries(flax_tests gmock_main gtest lua sol)
