#include "gtest/gtest.h"
#include "../domain/Action.h"

TEST(ActionTest, FieldsAndExecute) {
    domain::Action action;
    action.id = "act1";
    action.name = "Тренировка";
    action.description = "Увеличивает ловкость";
    action.effects["agility"] = 1;
    EXPECT_EQ(action.id, "act1");
    EXPECT_EQ(action.name, "Тренировка");
    EXPECT_EQ(action.description, "Увеличивает ловкость");
    EXPECT_EQ(action.effects["agility"], 1);
    // Execute() пока не реализует логику, просто вызывается
    action.Execute();
} 