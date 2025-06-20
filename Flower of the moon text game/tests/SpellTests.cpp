#include "gtest/gtest.h"
#include "../domain/Spell.h"

TEST(SpellTest, FieldsAndCast) {
    domain::Spell spell;
    spell.id = "spell1";
    spell.name = "Огненный шар";
    spell.description = "Наносит урон огнем";
    spell.manaCost = 3;
    spell.effects["damage"] = 15;
    EXPECT_EQ(spell.id, "spell1");
    EXPECT_EQ(spell.name, "Огненный шар");
    EXPECT_EQ(spell.description, "Наносит урон огнем");
    EXPECT_EQ(spell.manaCost, 3);
    EXPECT_EQ(spell.effects["damage"], 15);
    // cast() пока не реализует логику, просто вызывается
    spell.cast();
} 