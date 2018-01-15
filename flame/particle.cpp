#include "particle.h"
#include <random>
#include <time.h>

// Contructor giving a random starting position (within range).
Particle::Particle()
{
    m_lifetime = 1.0f;
    m_velocity = glm::vec3(0, 0, 0);
    m_position = glm::vec3((((float) rand() / (float) RAND_MAX) - 0.5) / 100,
                           -0.1f,
                           (((float) rand() / (float) RAND_MAX) - 0.5) / 100);
}

/*
 * Update loop goes through all of the particles within a small range of itself and either accelerates towards or away from them depending on density
 * though not quite a full fluid dynamics equation it gives a good substitute at a low cost to the cpu.
 */
void Particle::Update(std::vector<Particle *> &_particlesInRange, glm::vec3 direction, float _deltaTime)
{
    glm::vec3 acceleration = glm::normalize(direction);
    // Upwards acceleration due to convection added.
//    acceleration.y += 0.6f;
    // Lifetime of the particle decreased.
    m_lifetime -= _deltaTime;
    // For each particle in range move towards or away from it depending on density.
    for (unsigned int i = 0; i < _particlesInRange.size(); i++)
    {
        if (_particlesInRange[i] != this)
        {
            if (_particlesInRange.size() > 20)
            {
                acceleration += (m_position - _particlesInRange[i]->m_position) * 0.02f;
            } else
            {
                acceleration += (m_position - _particlesInRange[i]->m_position) * -2.0f;
            }
        }
    }
    // Accelerate the particle.
    m_velocity += acceleration * _deltaTime / 10.0f;
    // Move the particle.
    m_position += m_velocity * _deltaTime;
}

