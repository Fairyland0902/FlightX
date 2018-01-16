#include "BoundingBox.h"
#include "resource_manager.h"
#include "camera.h"
extern Camera *currentcamera;
BoundingBox::BoundingBox(const glm::mat4* objM, const glm::vec3& pos, const glm::vec3& scale):objectModel(objM)
{
	model={
		scale.x,0,0,0,
		0,scale.y,0,0,
		0,0,scale.z,0,
		pos.x,pos.y,pos.z,1
	};
	invModel = glm::inverse(*objectModel*model);
}


BoundingBox::~BoundingBox()
{
}
void BoundingBox::Draw(){
	static GLuint VAO = util::genVAO();
	static GLuint vert_buf = util::genBuf();
	glBindVertexArray(VAO);
	static std::vector<glm::vec3> vpos = {
		{-1,-1,-1},{-1,-1,1},
		{1,-1,-1},{1,-1,1},
		{-1,1,-1},{-1,1,1},
		{1,1,-1},{1,1,1},
		{ -1,-1,-1 },{ -1,1,-1 },
		{ 1,-1,-1 },{ 1,1,-1 },
		{ -1,-1,1 },{ -1,1,1 },
		{ 1,-1,1 },{ 1,1,1 },
		{ -1,-1,-1 },{ 1,-1,-1 },
		{ -1,1,-1 },{ 1,1,-1 },
		{ -1,-1,1 },{ 1,-1,1 },
		{ -1,1,1 },{ 1,1,1 },
	};
	ResourceManager::GetShader("boundingbox").SetMatrix4("MVP", currentcamera->getVPMatrix()**objectModel*model, true);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vpos.size(), &vpos[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_LINES, 0, vpos.size());
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

int BoundingBox::inBox(const glm::vec3& wpos) {
	auto posg=invModel*glm::vec4(wpos, 1);
	if (posg.x > 1 || posg.x < -1)return 0;
	if (posg.y > 1 || posg.y < -1)return 0;
	if (posg.z > 1 || posg.z < -1)return 0;
	return 1;
}

void BoundingBox::Update() {
	invModel = glm::inverse(*objectModel*model);
}
