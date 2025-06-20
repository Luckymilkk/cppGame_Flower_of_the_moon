#include "ActionManager.h"

namespace domain {

void ActionManager::AddAction(const Action& action) {
    actions_[action.id] = action;
}

const Action* ActionManager::GetAction(const std::string& id) const {
    auto it = actions_.find(id);
    if (it != actions_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Action> ActionManager::GetAllActions() const {
    std::vector<Action> result;
    for (const auto& pair : actions_) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain 