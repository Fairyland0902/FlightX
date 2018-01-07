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
	float controlx, controly;
	GLuint HUDVAO;
	float getLength(const glm::vec3&);
	glm::vec3 getAcceleration();
public:
	Aircraft();
	~Aircraft();
	void loadModel(string path) override;
	void Update(float deltaTime);
	const glm::vec3&& getAirspeed();
	void Draw(Shader& shader) override;
	void setAirspeed(const glm::vec3&);
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)override;
	void KeyBoardControl(bool *keys, GLfloat deltaTime);
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLfloat xpos, GLfloat ypos, GLboolean constrainPitch = true)override;
	void ProcessMouseScroll(GLfloat yoffset)override;
	void DrawHUD();
};

