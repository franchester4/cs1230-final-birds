#include "flock.h"

#include <iostream>

Flock::Flock()
{

}

void Flock::createBoids(int numBoids) {

    std::cout << "creating boids" << std::endl;

    for (int i = 0; i < numBoids; i++) {
        glm::vec3 pos = glm::vec3(rand() % 100 / 100.0f, rand() % 100 / 100.0f, 0.0f);
        glm::vec3 vel = glm::vec3(rand() % 100 / 100.0f - 0.5f, rand() % 100 / 100.0f - 0.5f, 0.0f);

        boids.push_back(Boid(pos, vel));
    }
}

void Flock::update(float deltaTime) {
    for (Boid& boid : boids) {
        boid.update(deltaTime);
    }
}
