#include "Aircraft.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "resource_manager.h"

extern GLuint width, height;
extern int WIDTH, HEIGHT;
extern bool keys[1024];
extern Camera *currentcamera;

float Aircraft::getLength(const glm::vec3 &v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z + v.z);
}

void WorldUpRotate(glm::vec3 &v, double val)
{
    double cosv = cos(val), sinv = sin(val);
    float vx = v.x, vz = v.z;
    v.x = vx * cosv + vz * sinv;
    v.z = -vx * sinv + vz * cosv;
}

glm::vec3 Aircraft::getAcceleration()
{
    return glm::vec3(0);
}

Aircraft::Aircraft() : Model(), Camera(), inAir(1), target_thrust(20), thrust(20), controlx(0), controly(0)
{
    Position = glm::vec3(0.0f);
    WorldUp = glm::vec3(0, 1, 0);
    Front = glm::vec3(1, 0, 0);
    Up = glm::vec3(0, 1, 0);
    Right = glm::cross(Front, Up);
    airspeed = glm::vec3(0, 0, 0);
}


Aircraft::~Aircraft()
{

}

void Aircraft::loadModel(string path)
{
    Model::loadModel(path);
    Offset = glm::vec3(-2.0f, 0.3f, 0);
}

void Aircraft::Update(float dt)
{
    glm::vec3 acc(-airspeed * 0.4f * glm::clamp(abs(glm::dot(Up, WorldUp)), 0.3f, 0.75f) +
                  thrust * Front * 0.025f * glm::clamp(1.0f - Position.y / 300.0f, 0.1f, 1.0f));
    acc += Up * ias * glm::clamp(ias, 0.0f, 4.0f) * glm::clamp(glm::dot(Up, WorldUp), 0.1f, 1.0f) * 0.05f;
    auto yaw = glm::dot(acc, Right) / (ias > 0.5 ? ias : 0.5) / 5 * dt;
    WorldUpRotate(Up, yaw);
    WorldUpRotate(Front, yaw);
    WorldUpRotate(Right, yaw);
    acc.y -= 1.0f;
    if (!inAir)
    {
        if (acc.y < 0)acc.y = 0;
    }
    float controlval = 0.005f * (4.0f + ias * 1.1f);
    Front = glm::normalize(Front + controlval * controly * Up);
    Up += controlval * controlx * Right;
    Up = glm::normalize(Up - glm::dot(Up, Front) * Front);
    Right = glm::cross(Front, Up);
    Position += airspeed * dt;
    airspeed += acc * dt;
    thrust += ((target_thrust > 20 ? target_thrust : 20) - thrust) * dt * 0.45;
    ias = glm::dot(airspeed, Front);
}

const glm::vec3 &&Aircraft::getAirspeed()
{
    return std::move(airspeed);
}

void Aircraft::Draw(Shader &shader)
{
    glm::mat4 vp = glm::perspective(currentcamera->Zoom, (float) width / (float) height,
                                    currentcamera->NearClippingPlaneDistance,
                                    currentcamera->FarClippingPlaneDistance)
                   * currentcamera->GetViewMatrix();
//	if (currentcamera == this)DrawHUD();
    glm::mat4 model = {
            Front.x, Front.y, Front.z, 0,
            Up.x, Up.y, Up.z, 0,
            Right.x, Right.y, Right.z, 0,
            Position.x, Position.y, Position.z, 1
    };
    shader.Use();
    shader.SetMatrix4("Model", model);
    shader.SetMatrix4("VP", vp);
    Model::Draw(shader);
}

void Aircraft::setAirspeed(const glm::vec3 &v)
{
    airspeed = v;
}

void Aircraft::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{}

void Aircraft::KeyBoardControl(bool *keys, GLfloat deltaTime)
{
    if (keys[GLFW_KEY_R])Position = glm::vec3(-10.0f, 0, 0);
    if (keys[GLFW_KEY_F1])target_thrust -= 1.0f;
    if (keys[GLFW_KEY_F4])target_thrust += 1.0f;
    if (target_thrust > 100)target_thrust = 100;
    if (target_thrust < 0)target_thrust = 0;
}

void
Aircraft::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLfloat xpos, GLfloat ypos, GLboolean constrainPitch)
{
    controlx = xpos / width - 0.5;
    controly = ypos / height - 0.5;
}

void Aircraft::ProcessMouseScroll(GLfloat yoffset)
{
    //airspeed *= 1 + yoffset/3;
}

inline void _AIRCRAFT_UTIL_push_line(float xStart, float yStart, float xEnd, float yEnd, std::vector<glm::vec2> &pos)
{
    pos.emplace_back(xStart, yStart);
    pos.emplace_back(xEnd, yEnd);
}

inline void
_AIRCRAFT_UTIL_push_line_display(float xStart, float yStart, float xEnd, float yEnd, float xLeft, float xRight,
                                 float yUp, float yDown, std::vector<glm::vec2> &pos)
{
    pos.emplace_back(xStart * xRight + xLeft * (1 - xStart), yStart * yUp + yDown * (1 - yStart));
    pos.emplace_back(xEnd * xRight + xLeft * (1 - xEnd), yEnd * yUp + yDown * (1 - yEnd));
}

void _AIRCRAFT_UTIL_push_digit_display(int digit, float xLeft, float xRight, float yUp, float yDown,
                                       std::vector<glm::vec2> &pos)
{
#define AIRCRAFT_UTIL_DIG(x, y, z, w) _AIRCRAFT_UTIL_push_line_display(x,y,z,w,xLeft,xRight,yUp,yDown,pos)
    switch (digit)
    {
        case 0:
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.9, 0.1);
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.9, 0.1);
            AIRCRAFT_UTIL_DIG(0.1, 0.6, 0.9, 0.4);
            break;
        case 1:
            AIRCRAFT_UTIL_DIG(0.8, 0.9, 0.8, 0.1);
            break;
        case 2:
            AIRCRAFT_UTIL_DIG(0.1, 0.7, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.9, 0.3, 0.9, 0.1);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.9, 0.7);
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.9, 0.1);
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.9, 0.7);
            break;
        case 3:
            AIRCRAFT_UTIL_DIG(0.1, 0.8, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.9, 0.6);
            AIRCRAFT_UTIL_DIG(0.6, 0.5, 0.9, 0.6);
            AIRCRAFT_UTIL_DIG(0.6, 0.5, 0.9, 0.4);
            AIRCRAFT_UTIL_DIG(0.9, 0.1, 0.9, 0.4);
            AIRCRAFT_UTIL_DIG(0.9, 0.1, 0.1, 0.1);
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.1, 0.2);
            break;
        case 4:
            AIRCRAFT_UTIL_DIG(0.7, 0.7, 0.7, 0.1);
            AIRCRAFT_UTIL_DIG(0.1, 0.4, 0.9, 0.4);
            AIRCRAFT_UTIL_DIG(0.1, 0.4, 0.6, 0.9);
            break;
        case 5:
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.1, 0.6, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.1, 0.6, 0.9, 0.6);
            AIRCRAFT_UTIL_DIG(0.9, 0.1, 0.9, 0.6);
            AIRCRAFT_UTIL_DIG(0.9, 0.1, 0.1, 0.1);
            break;
        case 6:
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.1, 0.7);
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.9, 0.1);
            AIRCRAFT_UTIL_DIG(0.9, 0.5, 0.9, 0.1);
            AIRCRAFT_UTIL_DIG(0.9, 0.5, 0.1, 0.5);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.1, 0.7);
            break;
        case 7:
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.5, 0.4);
            AIRCRAFT_UTIL_DIG(0.5, 0.1, 0.5, 0.4);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.1, 0.7, 0.1, 0.9);
            break;
        case 8:
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.1, 0.4);
            AIRCRAFT_UTIL_DIG(0.1, 0.6, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.9, 0.4, 0.9, 0.1);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.9, 0.6);
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.9, 0.1);
            AIRCRAFT_UTIL_DIG(0.1, 0.6, 0.9, 0.4);
            AIRCRAFT_UTIL_DIG(0.1, 0.4, 0.9, 0.6);
            break;
        case 9:
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.9, 0.3);
            AIRCRAFT_UTIL_DIG(0.9, 0.9, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.1, 0.5, 0.1, 0.9);
            AIRCRAFT_UTIL_DIG(0.1, 0.5, 0.9, 0.5);
            AIRCRAFT_UTIL_DIG(0.1, 0.1, 0.9, 0.3);
            break;
    }
}

void _AIRCRAFT_UTIL_show_number(int value, float xRight, float yCenter, float width, float height,
                                std::vector<glm::vec2> &pos, int displayoption = 0)
{
    if (value >= 0)displayoption &= 0xfffe;
    else if (displayoption & 1) value = -value;
    else value = 0;
    float xptr = xRight;
    while (value > 0 || xptr == xRight)
    {
        _AIRCRAFT_UTIL_push_digit_display(value % 10, xptr - width, xptr, yCenter + height / 2, yCenter - height / 2,
                                          pos);
        xptr -= width;
        value /= 10;
    }
    if (displayoption & 1)
    {
        pos.emplace_back(xptr - 0.9 * width, yCenter);
        pos.emplace_back(xptr - 0.1 * width, yCenter);
    }
}

void Aircraft::DrawHUD()
{
    static GLuint VAO = util::genVAO();
    static GLuint vert_buf = util::genBuf();
    glBindVertexArray(VAO);
    glDisable(GL_DEPTH_TEST);
    ResourceManager::GetShader("hudline").Use();
    std::vector<glm::vec2> vpos;
    // Two major lines
    _AIRCRAFT_UTIL_push_line(-0.5, -0.5, -0.5, 0.5, vpos);
    _AIRCRAFT_UTIL_push_line(0.5, -0.5, 0.5, 0.5, vpos);
    _AIRCRAFT_UTIL_push_line(-0.5, 0, -0.6, 0, vpos);
    _AIRCRAFT_UTIL_push_line(0.5, 0, 0.6, 0, vpos);
    // Airspeed and its block
    _AIRCRAFT_UTIL_show_number(ias * 60, -0.61f, 0, 0.05, 0.2, vpos, 0);
    _AIRCRAFT_UTIL_push_line(-0.85, 0.11, -0.6, 0.11, vpos);
    _AIRCRAFT_UTIL_push_line(-0.6, -0.11, -0.6, 0.11, vpos);
    _AIRCRAFT_UTIL_push_line(-0.6, -0.11, -0.85, -0.11, vpos);
    float iasd = ias > 0 ? ias * 3 : 0;
    for (int i = iasd - 2; i <= iasd + 3; ++i)
    {
        if (i < 0)continue;
        float position = (i - iasd) / 6;
        _AIRCRAFT_UTIL_push_line(-0.54, position, -0.5, position, vpos);
        _AIRCRAFT_UTIL_show_number(i * 20, -0.54, position, 0.015, 0.06, vpos);
    }
    //Alt and its block
    _AIRCRAFT_UTIL_show_number(Position.y * 100 + 20000, 0.86f, 0, 0.05, 0.2, vpos, 1);
    _AIRCRAFT_UTIL_push_line(0.85, 0.11, 0.6, 0.11, vpos);
    _AIRCRAFT_UTIL_push_line(0.6, -0.11, 0.6, 0.11, vpos);
    _AIRCRAFT_UTIL_push_line(0.6, -0.11, 0.85, -0.11, vpos);
    for (int i = Position.y - 6; i <= Position.y + 5; ++i)
    {
        float position = (i - Position.y) / 11;
        if (position < -0.5)continue;
        _AIRCRAFT_UTIL_push_line(0.52, position, 0.5, position, vpos);
        _AIRCRAFT_UTIL_show_number(i * 100 + 20000, 0.60, position, 0.015, 0.06, vpos, 1);
    }
    _AIRCRAFT_UTIL_show_number(airspeed.y * 6000, 0.48f, -0.54, 0.02, 0.08, vpos, 1);


    int tgtthr = target_thrust, thr = thrust;
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vpos.size(), &vpos[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_LINES, 0, vpos.size());
    glDisableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}
