#include "Aircraft.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "resource_manager.h"
#include "stb_image.h"

#ifdef __WIN32__
#include <corecrt_math_defines.h>
#endif

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
    AroundCam = new AroundCamera(&Position, &Front);
}

Aircraft::~Aircraft()
{
    delete AroundCam;
}

void Aircraft::Init()
{
    // Generate glossy environment map.
    std::vector<std::string> faces
            {
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayRight_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayLeft_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayUp_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayDown_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayBack_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayFront_blur.png"
            };
    glossyEnvmap = loadCubemap(faces);

    // Initialize particle generator.
    flame = new ParticleGenerator(ResourceManager::GetShader("particle"),
                                  ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/flame.png", GL_TRUE, "flame"), 500);
}

void Aircraft::loadModel(string path)
{
    Model::loadModel(path);
    Offset = glm::vec3(-0.4f, 0.06f, 0);
}

void Aircraft::Update(float dt)
{
    glm::vec3 acc(Up *ias
    *glm::clamp(ias, 0.0f, 6.0f) * 0.06f);
    double yaw = -glm::dot(acc, glm::normalize(glm::vec3(Right.x, 0, Right.z))) / (ias > 0.5 ? ias : 0.5) * 3.0f * dt *
                 (glm::dot(Up, WorldUp) > 0 ? 1.0f : -0.8f);
    acc *= glm::clamp(glm::dot(Up, WorldUp), 0.2f, 1.0f);
    acc += -airspeed * 0.4f * glm::clamp(abs(glm::dot(Up, WorldUp)), 0.3f, 0.75f) +
           thrust * Front * 0.02f * glm::clamp(1.0f - Position.y / 200.0f, 0.1f, 1.0f);
    WorldUpRotate(Up, yaw);
    WorldUpRotate(Front, yaw);
    WorldUpRotate(Right, yaw);
    acc.y -= 0.8f;
    if (!inAir)
    {
        if (acc.y < 0)acc.y = 0;
    }
    float controlval = 0.002f * (4.0f + glm::clamp(ias, 0.0f, 3.0f) * 1.1f);
    glm::vec3 frontOrig = Front;
    Front = glm::normalize(Front + controlval * controly * Up);
    Up += controlval * controlx * Right;
    Up = glm::normalize(Up - glm::dot(Up, Front) * Front);
    Right = glm::cross(Front, Up);
    Position += airspeed * dt;
    airspeed += (Front - frontOrig) * ias * 0.8f;
    airspeed += acc * dt;
    thrust += ((target_thrust > 20 ? target_thrust : 20) - thrust) * dt * 0.45;
    ias = glm::dot(airspeed, Front);

    // Update tail flame.
    flame->Update(dt, Position, airspeed, 2, -Front);
}

const glm::vec3 &&Aircraft::getAirspeed()
{
    return std::move(airspeed);
}

void Aircraft::Draw(Shader &shader, GLuint shadowMap, glm::mat4 lightSpaceMatrix)
{
    glm::mat4 vp = currentcamera->getVPMatrix();
//	if (currentcamera == this)DrawHUD();
    glm::mat4 model = {
            Front.x, Front.y, Front.z, 0,
            Up.x, Up.y, Up.z, 0,
            Right.x, Right.y, Right.z, 0,
            Position.x, Position.y, Position.z, 1
    };
    shader.Use();
    shader.SetMatrix4("Model", glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)));
    shader.SetMatrix4("VP", vp);

    // Material setting.
    shader.SetVector3f("material.ambient", glm::vec3(0.0f));
    shader.SetVector3f("material.diffuse", glm::vec3(0.0f));
    shader.SetVector3f("material.specular", glm::vec3(0.0f));

    // Light setting.
    shader.SetVector3f("dirLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader.SetVector3f("dirLight.ambient", glm::vec3(0.6f, 0.6f, 0.6f));
    shader.SetVector3f("dirLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
    shader.SetVector3f("dirLight.specular", glm::vec3(0.4f, 0.4f, 0.4f));

    // View position setting.
    shader.SetVector3f("viewPos", currentcamera->GetViewPosition());

    // Environment mapping setting.
    shader.SetInteger("skyBox", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glossyEnvmap);

    // Shadow setting.
    shader.SetInteger("shadowMap", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

    Model::Draw(shader);

    // Reset texture info.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Render tail flame.
    glDepthMask(GL_FALSE);
    // The model matrix is set according to the aircraft's position.
    Shader particle = ResourceManager::GetShader("particle");
    particle.Use();
    flame->Draw();
    glDepthMask(GL_TRUE);
}

void Aircraft::DrawDepth(Shader &shader)
{
    glm::mat4 model = {
            Front.x, Front.y, Front.z, 0,
            Up.x, Up.y, Up.z, 0,
            Right.x, Right.y, Right.z, 0,
            Position.x, Position.y, Position.z, 1
    };
    shader.Use();
    shader.SetMatrix4("model", glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)));

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
#undef AIRCRAFT_UTIL_DIG
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

void _AIRCRAFT_HUD_show_THR(float thr, float tgtthr, std::vector<glm::vec2> &pos)
{
    static std::vector<glm::vec2> rollpos;
#define AIRCRAFT_HUD_THR(x, y, z, w) _AIRCRAFT_UTIL_push_line_display(x,y,z,w,-0.7,-0.5,-0.55,-0.95,pos)
    if (rollpos.empty())
    {
        AIRCRAFT_HUD_THR(0.5, 0.5, 0.98, 0.5);
        for (int i = 0; i < 99; i++)
        {
            AIRCRAFT_HUD_THR(0.45 * cos(0.044 * i) + 0.5, 0.5 - 0.45 * sin(0.044 * i),
                             0.45 * cos(0.044 * i + 0.044) + 0.5, 0.5 - 0.45 * sin(0.044 * i + 0.044));
        }
    }
    AIRCRAFT_HUD_THR(0.45 * cos(0.044 * thr) + 0.5, 0.5 - 0.45 * sin(0.044 * thr), 0.5, 0.5);
    for (const glm::vec2 &p : rollpos)pos.push_back(p);
    int i;
    for (i = 0; i < tgtthr - 1; i++)
    {
        AIRCRAFT_HUD_THR(0.48 * cos(0.044 * i) + 0.5, 0.5 - 0.48 * sin(0.044 * i), 0.48 * cos(0.044 * i + 0.044) + 0.5,
                         0.5 - 0.48 * sin(0.044 * i + 0.044));
    }
    AIRCRAFT_HUD_THR(0.48 * cos(0.044 * i) + 0.5, 0.5 - 0.48 * sin(0.044 * i), 0.48 * cos(0.044 * tgtthr) + 0.5,
                     0.5 - 0.48 * sin(0.044 * tgtthr));
    AIRCRAFT_HUD_THR(0.45 * cos(0.044 * tgtthr) + 0.5, 0.5 - 0.45 * sin(0.044 * tgtthr),
                     0.48 * cos(0.044 * tgtthr) + 0.5, 0.5 - 0.48 * sin(0.044 * tgtthr));
    _AIRCRAFT_UTIL_show_number(thr, -0.51, -0.65, 0.03, 0.12, pos);
}

float Aircraft::_getHDG() const
{
    glm::vec2 dir = glm::normalize(glm::vec2(Front.x, Front.z));
    if (dir.y < 0)
    {
        return (asin(dir.x) * 180 / 3.14159265358979323846 + 270);
    } else
        return (90 - asin(dir.x) * 180 / 3.14159265358979323846);
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
    _AIRCRAFT_UTIL_push_line(-0.05, -0.05, 0, 0, vpos);
    _AIRCRAFT_UTIL_push_line(0.05, -0.05, 0, 0, vpos);
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
    glm::mat4 vpMatrix = getVPMatrix();
    glm::vec3 frontv = Front;
    frontv.y = 0;
    glm::vec4 Infpos = glm::vec4(GetViewPosition() + glm::normalize(frontv), 1);
    Infpos.y -= Front.y;
    glm::vec4 rv(frontv.z, 0, -frontv.x, 0);
    glm::vec4 posx;
    posx = vpMatrix * (Infpos + rv);
    vpos.emplace_back(posx.x, posx.y);
    posx = vpMatrix * (Infpos - rv);
    vpos.emplace_back(posx.x, posx.y);

    //Compass
    float hdg = _getHDG();
    for (int i = 0; i < 359; i += 5)
    {
        float len = (i % 10) ? 0.05 : (i % 30 ? 0.08 : (i % 90 ? 0.12 : 0.2));
        float ytop = -2 + 1.2 * cos(glm::radians(i - hdg));
        if (ytop < -1)continue;
        _AIRCRAFT_UTIL_push_line(0.6 * (1.2 - len) * sin(glm::radians(i - hdg)),
                                 -2 + (1.2 - len) * cos(glm::radians(i - hdg)), 0.72f * sin(glm::radians(i - hdg)),
                                 ytop, vpos);

    }
    _AIRCRAFT_UTIL_show_number(hdg, 0.045, -0.74, 0.03, 0.12, vpos);
    //Thrust
    _AIRCRAFT_HUD_show_THR(thrust, target_thrust, vpos);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vpos.size(), &vpos[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDrawArrays(GL_LINES, 0, vpos.size());
    glDisableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
}

GLuint Aircraft::loadCubemap(std::vector<std::string> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

AroundCamera::AroundCamera(glm::vec3 *pos, glm::vec3 *front) : pos(pos), front(front), distance(2.0)
{}

void AroundCamera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{}

void AroundCamera::KeyBoardControl(bool *keys, GLfloat deltaTime)
{
    if (keys[GLFW_KEY_A])Yaw += 40.0f * deltaTime;
    if (keys[GLFW_KEY_D])Yaw -= 40.0f * deltaTime;
    if (keys[GLFW_KEY_W])Pitch += 40.0f * deltaTime;
    if (keys[GLFW_KEY_S])Pitch -= 40.0f * deltaTime;
    if (Pitch < -89)Pitch = -89;
    if (Pitch > 89)Pitch = +89;
    updateCameraVectors();
}

void AroundCamera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLfloat xpos, GLfloat ypos,
                                        GLboolean constrainPitch)
{}

void AroundCamera::ProcessMouseScroll(GLfloat yoffset)
{
    distance -= yoffset;
    if (distance < 0.4)distance = 0.4;
}

glm::mat4 AroundCamera::GetViewMatrix() const
{
    return glm::lookAt(*pos + Front * distance, *pos, Up);
}

glm::vec3 AroundCamera::GetViewPosition()
{
    return *pos + Front * distance * 0.8f;
}