#include "aircraft.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
extern int WIDTH, HEIGHT;
extern bool keys[1024];
extern Camera* currentcamera;
float Aircraft::getLength(const glm::vec3& v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z + v.z);
}

glm::vec3 Aircraft::getAcceleration() {
	glm::vec3 acc(-airspeed*0.3f+ thrust*Front*0.015f);
	acc.y -= 0.5;
	acc += Up * ias*ias*0.05f;
	if(!inAir) {
		if (acc.y < 0)acc.y = 0;
	}
	return acc;
}

Aircraft::Aircraft() :Model(),Camera(),inAir(1),target_thrust(20),thrust(20),controlx(0),controly(0)
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

void Aircraft::loadModel(string path) {
	Model::loadModel(path);
	Offset = glm::vec3(-2.0f, 0.3f, 0);
}

void WorldUpRotate(glm::vec3& v,double val) {
	double cosv = cos(val), sinv=sin(val);
	float vx = v.x, vz = v.z;
	v.x = vx * cosv - vz * sinv;
	v.z = vx * sinv + vz * cosv;
}
void Aircraft::Update(float dt) {
	glm::vec3  a = getAcceleration();
	auto yaw=glm::dot(a, Right)/(ias+0.05)*dt;
	WorldUpRotate(Up, yaw);
	WorldUpRotate(Front, yaw);
	WorldUpRotate(Right, yaw);
	Position += airspeed * dt;
	airspeed += a * dt;
	thrust += ((target_thrust>20?target_thrust:20) - thrust) *dt *0.45 ;
	ias = glm::dot(airspeed, Front);
}

const glm::vec3&& Aircraft::getAirspeed() {
	return std::move(airspeed);
}

void Aircraft::Draw(Shader& shader) {
	glm::mat4 vp = glm::perspective(0.8f, float(WIDTH) / HEIGHT, .001f, 100.0f) 
	*currentcamera->GetViewMatrix();
	glm::mat4 model = {
		Front.x,Front.y,Front.z,0,
		Up.x,Up.y,Up.z,0,
		Right.x,Right.y,Right.z,0,
		Position.x,Position.y,Position.z,1
	};
	shader.Use();
	shader.SetMatrix4("Model",model);
	shader.SetMatrix4("VP", vp);
	Model::Draw(shader);
}

void Aircraft::setAirspeed(const glm::vec3& v) {
	airspeed = v;
}
void Aircraft::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
	
}

void Aircraft::KeyBoardControl(bool* keys, GLfloat deltaTime) {
	if (keys[GLFW_KEY_R])Position = glm::vec3(-10.0f, 0, 0);
	if (keys[GLFW_KEY_F1])target_thrust -= 1.0f;
	if (keys[GLFW_KEY_F4])target_thrust+=1.0f;
	if (target_thrust > 100)target_thrust = 100;
	if (target_thrust <0)target_thrust = 0;
	std::cout << ias <<" "<<thrust << std::endl;
}
void Aircraft::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch ) {}
void Aircraft::ProcessMouseScroll(GLfloat yoffset) {
	airspeed *= 1 + yoffset/3;
}