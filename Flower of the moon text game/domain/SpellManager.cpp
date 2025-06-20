 #include "SpellManager.h"

namespace domain {

void SpellManager::addSpell(const Spell& spell) {
    spells[spell.id] = spell;
}

const Spell* SpellManager::getSpell(const std::string& id) const {
    auto it = spells.find(id);
    if (it != spells.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Spell> SpellManager::getAllSpells() const {
    std::vector<Spell> result;
    for (const auto& pair : spells) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain
