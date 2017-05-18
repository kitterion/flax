#include "src/lua_data.h"

#include "gmock/gmock.h"

#include "src/lua_test_string.h"

using testing::IsEmpty;
using testing::UnorderedElementsAreArray;

std::vector<std::string> ExtractNames(
    const std::vector<const Recipe*> recipes) {
  std::vector<std::string> result;
  result.reserve(recipes.size());
  for (auto recipe : recipes) {
    result.emplace_back(recipe->id);
  }
  return result;
}

TEST(LuaData, NoEnabledRecipes) {
  LuaTestString s;
  s.Technology("a").UnlockRecipe("non-existent");
  s.Recipe("b").Disable();

  LuaData lua;
  lua.LoadFromString(s);

  EXPECT_THAT(ExtractNames(lua.GetEnabledRecipes()), IsEmpty());
}

TEST(LuaData, EnabledByDefault) {
  LuaTestString s;
  s.Recipe("b");

  LuaData lua;
  lua.LoadFromString(s);

  EXPECT_THAT(ExtractNames(lua.GetEnabledRecipes()),
              UnorderedElementsAreArray({"b"}));
}

TEST(LuaData, EnabledByTechnology) {
  LuaTestString s;
  s.Technology("a").UnlockRecipe("b");
  s.Recipe("b").Disable();

  LuaData lua;
  lua.LoadFromString(s);

  EXPECT_THAT(ExtractNames(lua.GetEnabledRecipes()),
              UnorderedElementsAreArray({"b"}));
}
