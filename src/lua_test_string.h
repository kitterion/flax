#ifndef SRC_LUA_TEST_STRING_H_
#define SRC_LUA_TEST_STRING_H_

#include <string>
#include <vector>

class LuaTestString {
 public:
  class TechnologyProxy {
   public:
    TechnologyProxy(const std::string& name) : name_(name) {}
    TechnologyProxy& UnlockRecipe(const std::string& recipe_name) {
      unlocked_recipes_.emplace_back(recipe_name);
      return *this;
    }
    const std::string& name() const { return name_; }
    const std::vector<std::string> unlocked_recipes() const {
      return unlocked_recipes_;
    }

   private:
    std::string name_;
    std::vector<std::string> unlocked_recipes_;
  };
  class RecipeProxy {
   public:
    RecipeProxy(const std::string name) : name_(name), enabled_(true) {}
    RecipeProxy& Disable() {
      enabled_ = false;
      return *this;
    }
    RecipeProxy& Enable() {
      enabled_ = true;
      return *this;
    }

    const std::string& name() const { return name_; }
    bool enabled() const { return enabled_; }

   private:
    std::string name_;
    bool enabled_;
  };

  TechnologyProxy& Technology(const std::string& name) {
    technologies_.emplace_back(name);
    return technologies_.back();
  }
  RecipeProxy& Recipe(const std::string& name) {
    recipes_.emplace_back(name);
    return recipes_.back();
  }

  operator std::string() {
    std::string result;
    result.append("data = {raw = {");
    result.append("technology = {");
    for (const auto& tech : technologies_) {
      result.append(tech.name() + " = {");
      result.append("name = \"");
      result.append(tech.name());
      result.append("\",");

      if (!tech.unlocked_recipes().empty()) {
        result.append("effects = {");
        for (size_t i = 0; i < tech.unlocked_recipes().size(); ++i) {
          result.append("[" + std::to_string(i + 1) + "] = {");
          result.append("type = \"unlock-recipe\",");
          result.append("recipe = \"" + tech.unlocked_recipes()[i] + "\",");
          result.append("},");
        }
        result.append("},");
      }
      result.append("},");
    }
    result.append("}, recipe = {");
    for (const auto& recipe : recipes_) {
      result.append(recipe.name() + " = {");
      result.append("name = \"" + recipe.name() + "\",");

      result.append("enabled = ");
      if (recipe.enabled()) {
        result.append("true");
      } else {
        result.append("false");
      }
      result.append(",");

      result.append("},");
    }
    result.append("},");
    result.append("}}");

    return result;
  }

 private:
  std::vector<TechnologyProxy> technologies_;
  std::vector<RecipeProxy> recipes_;
};

#endif  // SRC_LUA_TEST_STRING_H_
