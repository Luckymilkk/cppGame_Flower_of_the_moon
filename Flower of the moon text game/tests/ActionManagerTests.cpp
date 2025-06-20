#include "gtest/gtest.h"
#include "../domain/ActionManager.h"

TEST(ActionManagerTest, AddAndGetAction) {
    domain::ActionManager manager;
    domain::Action action;
    action.id = "a1";
    action.name = "Тренировка";
    manager.addAction(action);
    const domain::Action* found = manager.getAction("a1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Тренировка");
    EXPECT_EQ(manager.getAction("notfound"), nullptr);
}

TEST(ActionManagerTest, GetAllActions) {
    domain::ActionManager manager;
    domain::Action a1; a1.id = "a"; a1.name = "A";
    domain::Action a2; a2.id = "b"; a2.name = "B";
    manager.addAction(a1);
    manager.addAction(a2);
    auto all = manager.getAllActions();
    EXPECT_EQ(all.size(), 2);
} 