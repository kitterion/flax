#ifndef SRC_LUA_DATA_H_
#define SRC_LUA_DATA_H_

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "sol.hpp"

std::string ToString(const sol::object& obj);

struct Recipe {
  struct Slot {
    std::string item;
    int amount;
  };
  std::vector<Slot> inputs;
  std::vector<Slot> outputs;
  std::string id;
  bool enabled;
};

struct Technology {
  std::string id;
  std::vector<std::string> unlocked_recipes;
};

class TechnologyTree {
 public:
  void Load(const sol::object& obj) {
    for (auto& item : obj.as<sol::table>()) {
      std::string name = item.first.as<std::string>();
      auto tech = std::make_unique<Technology>();
      tech->id = name;

      sol::optional<sol::table> effects =
          item.second.as<sol::table>()["effects"];
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

  const auto& technologies() const { return technologies_; }

 private:
  std::unordered_map<std::string, std::unique_ptr<Technology>> technologies_;
};

class LuaData {
 public:
  bool Load(const std::string& factorio_path);

 private:
  void EnableRecipesUnlockedFromTechnology();

  void LoadRecipes(const sol::object& obj);
  std::unordered_map<std::string, std::unique_ptr<Recipe>> recipes_;
  TechnologyTree technology_tree_;
};

#endif  // SRC_LUA_DATA_H_
