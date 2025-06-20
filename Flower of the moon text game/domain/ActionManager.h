#pragma once
#include "Action.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class ActionManager {
public:
    void addAction(const Action& action);
    const Action* getAction(const std::string& id) const;
    std::vector<Action> getAllActions() const;
private:
    std::unordered_map<std::string, Action> actions;
};

} // namespace domain 