#ifndef DOMAIN_ACTION_MANAGER_H_
#define DOMAIN_ACTION_MANAGER_H_

#include "Action.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class ActionManager {
public:
    void AddAction(const Action& action);
    const Action* GetAction(const std::string& id) const;
    std::vector<Action> GetAllActions() const;
private:
    std::unordered_map<std::string, Action> actions_;
};

} // namespace domain 

#endif  // DOMAIN_ACTION_MANAGER_H_ 