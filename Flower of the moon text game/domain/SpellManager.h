#pragma once
#include "Spell.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class SpellManager {
public:
    void addSpell(const Spell& spell);
    const Spell* getSpell(const std::string& id) const;
    std::vector<Spell> getAllSpells() const;
private:
    std::unordered_map<std::string, Spell> spells;
};

} // namespace domain 