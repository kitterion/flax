#ifndef SRC_LUA_DATA_H_
#define SRC_LUA_DATA_H_

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "sol.hpp"

std::string ToString(const sol::object& obj);

struct Ingredient {
  std::string item;
  int amount;
};

struct Recipe {
  std::vector<Ingredient> inputs;
  std::vector<Ingredient> outputs;
  std::string id;
  bool enabled;
};

struct Technology {
  std::string id;
  std::vector<std::string> unlocked_recipes;
};

class TechnologyTree {
 public:
  void Load(const sol::object& obj);
  const auto& technologies() const { return technologies_; }

 private:
  std::unordered_map<std::string, std::unique_ptr<Technology>> technologies_;
};

class LuaData {
 public:
  bool Load(const std::string& factorio_path);

  // Useful for tests.
  bool LoadFromString(const std::string& lua_code);

  std::vector<const Recipe*> GetEnabledRecipes() const;
  std::vector<std::string> GetEnabledRecipeNames() const;

 private:
  void EnableRecipesUnlockedFromTechnology();

  void LoadRecipes(const sol::object& obj);
  std::unordered_map<std::string, std::unique_ptr<Recipe>> recipes_;
  TechnologyTree technology_tree_;
};

#endif  // SRC_LUA_DATA_H_
