#include "ActionManager.h"

namespace domain {

void ActionManager::addAction(const Action& action) {
    actions[action.id] = action;
}

const Action* ActionManager::getAction(const std::string& id) const {
    auto it = actions.find(id);
    if (it != actions.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Action> ActionManager::getAllActions() const {
    std::vector<Action> result;
    for (const auto& pair : actions) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain 