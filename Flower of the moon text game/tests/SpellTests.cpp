#include "gtest/gtest.h"
#include "../domain/Spell.h"

TEST(SpellTest, FieldsAndCast) {
    domain::Spell spell;
    spell.id = "spell1";
    spell.name = "Огненный шар";
    spell.description = "Наносит урон огнём";
    spell.mana_cost = 3;
    spell.effects["damage"] = 15;
    EXPECT_EQ(spell.id, "spell1");
    EXPECT_EQ(spell.name, "Огненный шар");
    EXPECT_EQ(spell.description, "Наносит урон огнём");
    EXPECT_EQ(spell.mana_cost, 3);
    EXPECT_EQ(spell.effects["damage"], 15);
    // Cast() пока не реализует логику, просто вызывается
    spell.Cast();
} 