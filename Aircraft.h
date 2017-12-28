#pragma once
#include "model.h"
#include "camera.h"
#include <glm/detail/type_mat.hpp>

class Aircraft :
	public Model,
	public Camera
{
	bool inAir;
	float thrust;
	float target_thrust;
	glm::vec3 airspeed;
	float ias;
	float getLength(const glm::vec3&);
	glm::vec3 getAcceleration();
public:
	Aircraft(string modelname);
	~Aircraft();
	void Update(float deltaTime);
	const glm::vec3&& getAirspeed();
	void setAirspeed(const glm::vec3&);
};

