#ifndef _CLOUD_H
#define _CLOUD_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <random>
#include <deque>
#include "shader.h"

/*
 * Cloud base class.
 */
class Cloud
{
public:
    Cloud(glm::vec3 position, float size) :
            position(position),
            size(size)
    {}

    inline glm::vec3 getPosition()
    {
        return position;
    }

    inline float getSize()
    {
        return size;
    }

private:
    glm::vec3 position;
    float size;
};

/*
 * Store the cloud data, in particular a grid representing various cloud data.
 */
class CloudData
{
    friend class CloudSimulator;

public:
    CloudData(int length, int width, int height);

    ~CloudData();

    GLfloat ***nextDensity; // Continuous density distribution.
    GLfloat ***prevDensity; // Previous continuous density distribution.

    double nextTime; // Time when nextDensity was computed.
    double prevTime; // Time when prevDensity was computed.

    inline int getGridLength() const
    { return length; }

    inline int getGridWidth() const
    { return width; }

    inline int getGridHeight() const
    { return height; }

private:
    const int length;
    const int width;
    const int height;

    bool ***hum; // Humidity.
    bool ***cld; // Clouds.
    bool ***act; // Phase transition/activation.
    bool ***fAc;

    float ***distSize;      // Private size/distance to nearest cloud ratio.
    float ***workDensity;   // Private continuous density distribution.

    // Creates a dynamic 3d boolean array of size x*y*z.
    template<typename T>
    void make3Darray(T ***type);

    // Frees the 3d boolean array memory
    template<typename T>
    void delete3Darray(T ***type);
};

/*
 * A cellular automata approach to simulating clouds, such as described by Dobashi et al., which extends Nagel's method.
 */
class CloudSimulator
{
public:
    CloudSimulator(int x, int y, int z);

    void Simulate(CloudData &data, float startTime);

private:
    const int x;
    const int y;
    const int z;

    std::deque<Cloud> clouds;

    // Smaller base gives bigger clouds that more closely resemble ellipsoids.
    const int exponentialBase;
    const int minCloudSize;
    const int maxCloudSize;

    // Vapor extinction probability.
    float pHumExt;
    // Phase transition extinction probability.
    float pActExt;
    // Cloud extinction probability.
    float pCldExt;

    std::mt19937 gen;
    int randomResolution;

    // Simulate a cellular automata step.
    void simulateCellular(bool ***hum, bool ***act, bool ***cld, bool ***fAc, float ***distSize);

    // Creates a cloud at random position with random size.
    void createRandomCloud();

    // Recalculates dist/size, should be called when clouds are created.
    void calculateDistSize(float ***distSize);

    // Calculate continuous cloud density distribution for entire grid.
    void calculateDensity(bool ***cld, float ***den);

    // Calculate continuous cloud density distribution for one cell.
    float singleDensity(int i, int j, int k, bool ***cld, int S);

    // Calculate field function.
    float fieldFunction(float a);

    // Override one array with another.
    void copyGrid(float ***src, float ***dest);

    // Computes f_act(i,j,k) function. fAc is size x*y*z.
    void computeFAc(bool ***fAc, bool ***act, int i, int j, int k, int x, int y, int z);

    // Compute the distance of point (x1, y1, z1) from point (x2,y2, z2).
    float distFrom(int x1, int y1, int z1, int x2, int y2, int z2);
};

/*
 * Module responsible for cloud rendering.
 */
class CloudRender
{
public:
    CloudRender(int length, int width, int height);

    void Render(CloudData &data, Shader shader, float startTime);

private:
    float ***interpolatedData;
    GLuint volumeTexture;
    GLuint VAO, VBO, EBO;

    void interpolateCloudData(CloudData &data, float startTime);

    void renderRayCastingClouds(CloudData &data, Shader shader, float startTime);
};

#endif
