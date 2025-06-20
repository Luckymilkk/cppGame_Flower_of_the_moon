#include "gtest/gtest.h"
#include "../domain/SpellManager.h"

TEST(SpellManagerTest, AddAndGetSpell) {
    domain::SpellManager manager;
    domain::Spell spell;
    spell.id = "s1";
    spell.name = "Огненный шар";
    manager.addSpell(spell);
    const domain::Spell* found = manager.getSpell("s1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Огненный шар");
    EXPECT_EQ(manager.getSpell("notfound"), nullptr);
}

TEST(SpellManagerTest, GetAllSpells) {
    domain::SpellManager manager;
    domain::Spell s1; s1.id = "a"; s1.name = "A";
    domain::Spell s2; s2.id = "b"; s2.name = "B";
    manager.addSpell(s1);
    manager.addSpell(s2);
    auto all = manager.getAllSpells();
    EXPECT_EQ(all.size(), 2);
} 