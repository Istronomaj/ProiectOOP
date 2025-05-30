#include "zombie.h"
#include "plant.h"
#include "grid.h"

Zombie::Zombie(const int x, const int y, const int health, const int speed, std::string name, const int ticks, const int damage)
    : Entity(x,y, health,std::move(name),ticks,damage), speed(speed) {}

void Zombie::moveForward() {
    if (canMove.get()) {
        x.modify(-speed);
        hitbox.setPosition(static_cast<float>(x.get()),static_cast<float>(y.get()));
    }
}

void Zombie::action(const std::unique_ptr<Entity>& plant) {
    ticks.modify(-1);
    if (ticks.get() == 0){
        plant->takeDamage(damage.get());
        ticks = maxTicks;
    }
}

void Zombie::update(Grid &grid) {
    if (health.get() <= 0) {
        this->setDeletionMark(true);
        return;
    }

    moveForward();

    if (const auto& coords = grid.getCoords(); coords.contains("zombieHit")) {
        for (const auto& coord : coords.at("zombieHit")) {
            if (sf::FloatRect hitBox = this->getHitbox().getGlobalBounds(); hitBox.contains(static_cast<sf::Vector2f>(coord.first))) {
                grid.takeDamage(this->getDamage());
                this->setDeletionMark(true);
            }
            else setCanMove(true);
        }
    }

    for (auto& entity : grid.getEntities()) {
        if (dynamic_cast<Plant*>(entity.get())) {
            sf::FloatRect Hitbox = this->getHitbox().getGlobalBounds();
            if (Hitbox.contains(entity->getHitbox().getPosition())) {
                this->setCanMove(false);
                action(entity);
            }
        }
    }
}
