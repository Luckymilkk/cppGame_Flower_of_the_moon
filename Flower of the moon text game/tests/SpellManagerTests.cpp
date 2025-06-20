#include "gtest/gtest.h"
#include "../domain/SpellManager.h"

TEST(SpellManagerTest, AddAndGetSpell) {
    domain::SpellManager manager;
    domain::Spell spell;
    spell.id = "s1";
    spell.name = "Огненный шар";
    manager.AddSpell(spell);
    const domain::Spell* found = manager.GetSpell("s1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Огненный шар");
    EXPECT_EQ(manager.GetSpell("notfound"), nullptr);
}

TEST(SpellManagerTest, GetAllSpells) {
    domain::SpellManager manager;
    domain::Spell s1; s1.id = "a"; s1.name = "A";
    domain::Spell s2; s2.id = "b"; s2.name = "B";
    manager.AddSpell(s1);
    manager.AddSpell(s2);
    auto all = manager.GetAllSpells();
    EXPECT_EQ(all.size(), 2);
} 