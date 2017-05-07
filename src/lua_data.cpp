#include "src/lua_data.h"

#include <sys/stat.h>

#include <fstream>
#include <iostream>

namespace internal {
void AddPackagePath(const std::string& path, sol::state_view* lua) {
  std::string package_path = (*lua)["package"]["path"];
  (*lua)["package"]["path"] = std::string(package_path + ";" + path + "/?.lua");
}

void SetUpLuaState(const std::string& factorio_path, sol::state_view* lua) {
  lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::package,
                      sol::lib::string, sol::lib::table);
  AddPackagePath(factorio_path + "/data/core/lualib", lua);
  std::string data_loader_file =
      factorio_path + "/data/core/lualib/dataloader.lua";
  lua->script_file(data_loader_file);

  lua->script(
      "defines = {\n"
      "  difficulty_settings = {\n"
      "    recipe_difficulty = {\n"
      "      normal = 0,\n"
      "      expensive = 1,\n"
      "    },\n"
      "    technology_difficulty = {\n"
      "      normal = 0,\n"
      "      expensive = 1,\n"
      "    }\n"
      "  },\n"
      "  direction = {\n"
      "    north = 0,\n"
      "    northeast = 1,\n"
      "    east = 2,\n"
      "    southeast = 3,\n"
      "    south = 4,\n"
      "    southwest = 5,\n"
      "    west = 6,\n"
      "    northwest = 7,\n"
      "  },\n"
      "}\n");
}

bool FileExists(const std::string& filename) {
  struct stat buffer;
  return stat(filename.c_str(), &buffer) == 0;
}

void LoadMods(const std::string factorio_path, sol::state* lua) {
  std::string base_package_path = (*lua)["package"]["path"];

  auto mods = {factorio_path + "/data/base"};

  for (auto filename :
       {"data.lua", "data-updates.lua", "data-final-fixes.lua"}) {
    for (const auto& mod_path : mods) {
      (*lua)["package"]["path"] = base_package_path;

      AddPackagePath(mod_path, lua);

      lua->script(
          "for k, v in pairs(package.loaded) do\n"
          "  package.loaded[k] = false\n"
          "end\n");

      const std::string filepath = mod_path + '/' + filename;
      if (FileExists(filepath)) {
        lua->script_file(filepath);
      }
    }
  }
}

void Visit(const sol::object& o, int level, std::string* output) {
  if (o.is<bool>()) {
    if (o.as<bool>()) {
      output->append("true");
    } else {
      output->append("false");
    }
    return;
  }

  if (o.is<int>()) {
    output->append(std::to_string(o.as<int>()));
    return;
  }

  if (o.is<double>()) {
    output->append(std::to_string(o.as<double>()));
    return;
  }

  if (o.is<std::string>()) {
    output->append(1, '"');
    output->append(o.as<std::string>());
    output->append(1, '"');
  }

  if (o.is<sol::table>()) {
    output->append("{\n");
    for (const auto& item : o.as<sol::table>()) {
      output->append((level + 1) * 2, ' ');
      output->append(1, '"');
      output->append(item.first.as<std::string>());
      output->append(1, '"');
      output->append(": ");
      Visit(item.second, level + 1, output);
      output->append(",\n");
    }

    output->append(level * 2, ' ');
    output->append(1, '}');
  }
}
}  // namespace internal

std::string ToString(const sol::object& obj){
  std::string result;
  internal::Visit(obj, 0, &result);
  return result;
}

void LuaData::LoadRecipes(const sol::object& obj) {
  for (const auto& recipe : obj.as<sol::table>()) {
    auto new_recipe = std::make_unique<Recipe>();
    std::string recipe_name = recipe.first.as<std::string>();
    new_recipe->id = recipe_name;

    recipes_.emplace(recipe_name, std::move(new_recipe));
  }
}

void LuaData::EnableRecipesUnlockedFromTechnology() {
  for (const auto& tech : technology_tree_.technologies()) {
    for (const auto& recipe_name : tech.second->unlocked_recipes) {
      auto it = recipes_.find(recipe_name);
      if (it != recipes_.end()) {
        it->second->enabled = true;
      }
    }
  }
}

bool LuaData::Load(const std::string& factorio_path) {
  sol::state lua;
  internal::SetUpLuaState(factorio_path, &lua);
  internal::LoadMods(factorio_path, &lua);

  LoadRecipes(lua["data"]["raw"]["recipe"]);
  technology_tree_.Load(lua["data"]["raw"]["technology"]);
  EnableRecipesUnlockedFromTechnology();

  return true;
}
