#include "SpellManager.h"

namespace domain {

void SpellManager::AddSpell(const Spell& spell) {
    spells_[spell.id] = spell;
}

const Spell* SpellManager::GetSpell(const std::string& id) const {
    auto it = spells_.find(id);
    if (it != spells_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Spell> SpellManager::GetAllSpells() const {
    std::vector<Spell> result;
    for (const auto& pair : spells_) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain
