#pragma once
#include <glm/glm.hpp>

class BoundingBox
{
	const glm::mat4* objectModel;
	glm::mat4 model;
	glm::mat4 invModel;
public:
	BoundingBox(const glm::mat4* objM,const glm::vec3& pos,const glm::vec3& scale);
	~BoundingBox();
	void Update();
	void Draw();
	int inBox(const glm::vec3&);
};

