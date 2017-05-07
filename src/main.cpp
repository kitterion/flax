#include <cstdio>

#include <chrono>
#include <string>

#include "src/lua_data.h"

using namespace std::string_literals;

void PrintUsage(const char* program_name) {
  printf("Usage: %s --factorio-path <path to Factorio folder>", program_name);
}

int main(int argc, char* argv[]) {
  if (argc != 3 || "--factorio-path"s != argv[1]) {
    PrintUsage(argv[0]);
    return 0;
  }

  auto start = std::chrono::steady_clock::now();

  LuaData lua;
  lua.Load(argv[2]);

  auto finish = std::chrono::steady_clock::now();
  auto elapsed_milliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
          .count();
  std::cout << elapsed_milliseconds << " ms" << std::endl;
}
