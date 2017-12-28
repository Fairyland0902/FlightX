#include "aircraft.h"

float Aircraft::getLength(const glm::vec3& v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z + v.z);
}

glm::vec3 Aircraft::getAcceleration() {
	glm::vec3 acc(-airspeed*0.2f+ thrust*Front);
	acc.y -= 0.5;
	acc += Up * ias*ias*0.3f;
	if(!inAir) {
		if (acc.y < 0)acc.y = 0;
	}
	return acc;
}

Aircraft::Aircraft(string modelfile) :Model(modelfile),Camera(),inAir(false)
{
	Position = glm::vec3(0.0f);
	WorldUp = glm::vec3(0, 1, 0);
	Front = glm::vec3(1, 0, 0);
	Up = glm::vec3(0, 1, 0);
	Right = glm::cross(Front, Up);
	airspeed = glm::vec3(0,0,0);
}


Aircraft::~Aircraft()
{

}
void WorldUpRotate(glm::vec3& v,double val) {
	double cosv = cos(val), sinv=sin(val);
	float vx = v.x, vz = v.z;
	v.x = vx * cosv - vz * sinv;
	v.z = vx * sinv + vz * cosv;
}
void Aircraft::Update(float dt) {
	glm::vec3  a = getAcceleration();
	auto yaw=glm::dot(a, Right)/(ias+0.05);
	WorldUpRotate(Up, yaw);
	WorldUpRotate(Front, yaw);
	WorldUpRotate(Right, yaw);
	Position += airspeed * dt;
	airspeed += a * dt;
	thrust += ((target_thrust>20?target_thrust:20) - thrust) *dt *0.25 ;
	ias = getLength(airspeed);
}

const glm::vec3&& Aircraft::getAirspeed() {
	return std::move(airspeed);
}

void Aircraft::setAirspeed(const glm::vec3& v) {
	airspeed = v;
}

