#pragma once

#include "model.h"
#include "camera.h"
#include "flame/flame.h"
#include <glm/detail/type_mat.hpp>
#include <vector>
#include <string>
class BoundingBox;
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
	glm::mat4 modelMatrix;
    float getLength(const glm::vec3 &);
	void _updateModel();
    glm::vec3 getAcceleration();

    float _getHDG() const;

    GLuint glossyEnvmap;

    GLuint loadCubemap(std::vector<std::string> faces);

    std::vector<Flame *> flames;
	std::vector<BoundingBox*> bb;
public:
    Aircraft();

    ~Aircraft();

    void Init();

    void loadModel(string path) override;

    void Update(float deltaTime);

    const glm::vec3 &&getAirspeed();

    void Draw(Shader &shader, GLuint shadowMap, glm::mat4 lightSpaceMatrix);

    void DrawDepth(Shader &shader);

    void setAirspeed(const glm::vec3 &);

    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) override;

    void KeyBoardControl(bool *keys, GLfloat deltaTime) override;

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLfloat xpos, GLfloat ypos,
                              GLboolean constrainPitch = true) override;

    void ProcessMouseScroll(GLfloat yoffset) override;

    void DrawHUD(int drawId);
	void DrawBoundingBox();
	int detechCrash(const glm::vec3& wpos);
    Camera *AroundCam;
};

class AroundCamera : public Camera
{
    glm::vec3 *pos, *front;
    float distance;
public:
    AroundCamera(glm::vec3 *pos, glm::vec3 *front);
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) override;
    void KeyBoardControl(bool *keys, GLfloat deltaTime) override;
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLfloat xpos, GLfloat ypos,
                              GLboolean constrainPitch = true) override;
    void ProcessMouseScroll(GLfloat yoffset) override;
    virtual glm::mat4 GetViewMatrix() const;
    virtual glm::vec3 GetViewPosition();
};