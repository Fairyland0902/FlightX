#ifndef _PARTICLE_H
#define _PARTICLE_H

#include <glm/glm.hpp>
#include <vector>

/*
 * Particle class containing neccessary per-particle information and functions for calculating movement and distance of one particle to another
 * as well as some simple get functions.
 */
class Particle
{
private:
    glm::vec3 m_position;
    float m_lifetime;
    glm::vec3 m_velocity;
public:
    Particle(glm::vec3 position, glm::vec3 velocity);

    void Update(std::vector<Particle *> &_particlesInRange, glm::vec3 direction, float _deltaTime);

    glm::vec3 GetPos()
    { return m_position; }

    float GetLifetime()
    { return m_lifetime; }

    float Magnitude(glm::vec3 _vector)
    { return (_vector.x * _vector.x) + (_vector.y * _vector.y) + (_vector.z * _vector.z); }
};


#endif
