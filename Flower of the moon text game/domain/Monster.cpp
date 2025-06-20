#include "Monster.h"

namespace domain {

void Monster::takeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

bool Monster::isAlive() const {
    return health > 0;
}

} // namespace domain 