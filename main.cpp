#include <iostream>
// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
<<<<<<< HEAD
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Game
#include "game.h"
=======
>>>>>>> 11a224ee057affa6d268103a139d7a75e1c44961

// Function prototypes.
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void do_movement();

// Window size.
int WIDTH, HEIGHT;

// Keyboard.
bool keys[1024];

// Mouse.
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Game.
Game game;

// Start our application and run our game loop.
int main()
{
    // Init GLFW.
    glfwInit();
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
#endif
    // Create a "Windowed full screen" window in the primary monitor.
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "FlightX", monitor, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions.
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions.
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers.
    glewInit();

    // Define the viewport dimensions.
    glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
    std::cout << "window width = " << WIDTH << " pixels" << std::endl;
    std::cout << "window height = " << HEIGHT << " pixels" << std::endl;
    glViewport(0, 0, WIDTH, HEIGHT);

    // Setup some OpenGL options.
    glEnable(GL_DEPTH_TEST);

    // Input Options.
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Do some initialization for our game (include loading shaders, models, etc.(
    game.Init();

    // Game loop.
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame.
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions.
        glfwPollEvents();
        do_movement();

        // Clear the color buffer.
        glClearColor(67 / 256.0f, 128 / 256.0f, 183 / 256.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render.
        game.Render(WIDTH, HEIGHT, currentFrame);

        // Swap the screen buffers.
        glfwSwapBuffers(window);
    }
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW.
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    game.camera.ProcessMouseMovement(xoffset, yoffset);
}

void do_movement()
{
    // Camera control.
    if (keys[GLFW_KEY_W])
        game.camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        game.camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        game.camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        game.camera.ProcessKeyboard(RIGHT, deltaTime);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    game.camera.ProcessMouseScroll(yoffset);
}
