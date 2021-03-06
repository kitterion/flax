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

std::string ToString(const sol::object& obj) {
  std::string result;
  internal::Visit(obj, 0, &result);
  return result;
}

void TechnologyTree::Load(const sol::object& obj) {
  for (auto& item : obj.as<sol::table>()) {
    std::string name = item.first.as<std::string>();
    auto tech = std::make_unique<Technology>();
    tech->id = name;

    sol::optional<sol::table> effects = item.second.as<sol::table>()["effects"];
    if (!effects) {
      continue;
    }

    for (const auto& effect : effects.value()) {
      sol::optional<std::string> recipe =
          effect.second.as<sol::table>()["recipe"];
      if (recipe) {
        tech->unlocked_recipes.emplace_back(recipe.value());
      }
    }

    technologies_.emplace(name, std::move(tech));
  }
}

void LuaData::LoadRecipes(const sol::object& obj) {
  for (const auto& recipe : obj.as<sol::table>()) {
    auto new_recipe = std::make_unique<Recipe>();
    std::string recipe_name = recipe.first.as<std::string>();
    new_recipe->id = recipe_name;
    new_recipe->enabled = recipe.second.as<sol::table>().get<bool>("enabled");

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

bool LuaData::LoadFromString(const std::string& lua_code) {
  sol::state lua;
  lua.script(lua_code);

  LoadRecipes(lua["data"]["raw"]["recipe"]);
  technology_tree_.Load(lua["data"]["raw"]["technology"]);
  EnableRecipesUnlockedFromTechnology();

  return true;
}

std::vector<const Recipe*> LuaData::GetEnabledRecipes() const {
  std::vector<const Recipe*> result;
  for (const auto& recipe : recipes_) {
    if (recipe.second->enabled) {
      result.emplace_back(recipe.second.get());
    }
  }

  return result;
}
