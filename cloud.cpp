#include "cloud.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "IncompatibleTypes"

CloudData::CloudData(int length, int width, int height) :
        length(length),
        width(width),
        height(height)
{
    // Allocation of grids.
    hum = new bool **[length];
    cld = new bool **[length];
    act = new bool **[length];
    fAc = new bool **[length];
    distSize = new float **[length];
    workDensity = new float **[length];
    prevDensity = new float **[length];
    nextDensity = new float **[length];

    make3Darray(hum);
    make3Darray(cld);
    make3Darray(act);
    make3Darray(fAc);
    make3Darray(distSize);
    make3Darray(workDensity);
    make3Darray(prevDensity);
    make3Darray(nextDensity);

    prevTime = 0; // TODO: set to glfw time
    nextTime = 0;
}

CloudData::~CloudData()
{
    // Deallocate the array memory and delete the pointers.
    delete3Darray(hum);
    delete3Darray(cld);
    delete3Darray(act);
    delete3Darray(fAc);
    delete3Darray(distSize);
    delete3Darray(workDensity);
    delete3Darray(prevDensity);
    delete3Darray(nextDensity);
    delete hum;
    delete cld;
    delete act;
    delete fAc;
    delete distSize;
    delete workDensity;
    delete prevDensity;
    delete nextDensity;
}

template<typename T>
void CloudData::make3Darray(T ***arr)
{
    // Allocation of a 3d array.
    for (int i = 0; i != length; i++)
    {
        arr[i] = new T *[width];
        for (int j = 0; j != width; j++)
            arr[i][j] = new T[height];
    }

    // Initialization of a 3d array.
    for (int i = 0; i != length; i++)
        for (int j = 0; j != width; j++)
            for (int k = 0; k != height; k++)
                arr[i][j][k] = 0;
}

template<typename T>
void CloudData::delete3Darray(T ***arr)
{
    // Deallocation of a 3d array.
    for (int i = 0; i != length; i++)
    {
        for (int j = 0; j != width; j++)
            delete[] arr[i][j];
        delete[] arr[i];
    }
}

CloudSimulator::CloudSimulator(int x, int y, int z) :
        x(x),
        y(y),
        z(z),
        pHumExt(0.1f),
        pActExt(0.05f),
        pCldExt(0.1f),
        randomResolution(10000),
        exponentialBase(22),
        minCloudSize(11),
        maxCloudSize(20)
{
    std::random_device rd;
    gen = std::mt19937(rd());
}

void CloudSimulator::Simulate(CloudData &data, float startTime)
{
    simulateCellular(data.hum, data.act, data.cld, data.fAc, data.distSize);
    calculateDensity(data.cld, data.workDensity);
    // Copy nextDensity to prevDensity and workDensity to nextDensity.
    copyGrid(data.prevDensity, data.nextDensity);
    copyGrid(data.nextDensity, data.workDensity);
    // Update times.
    data.prevTime = data.nextTime;
    data.nextTime = startTime;
}

void CloudSimulator::simulateCellular(bool ***hum, bool ***act, bool ***cld, bool ***fAc, float ***distSize)
{
    std::cout << "Simulation step" << std::endl;

    // Generate new clouds.
    while (clouds.size() < 1 || gen() % 10 == 0)
    {
        createRandomCloud();
    }

    // Move the clouds.
    for (Cloud &c : clouds)
    {
        //c.move( 0.02f, 1.5f );
    }

    // Recalculate.
    calculateDistSize(distSize);

    // Perform cellular automaton step.
    for (int i = 0; i != x; ++i)
        for (int j = 0; j != y; ++j)
            for (int k = 0; k != z; ++k)
                computeFAc(fAc, act, i, j, k, x, y, z);

    for (int i = 0; i != x; i++)
    {
        for (int j = 0; j != y; j++)
        {
            for (int k = 0; k != z; k++)
            {
                // Compute clouds first, since they don't influence act/hum.
                cld[i][j][k] = cld[i][j][k] || act[i][j][k];
                bool newAct = !act[i][j][k] && hum[i][j][k] && fAc[i][j][k];
                hum[i][j][k] = hum[i][j][k] && !act[i][j][k];
                act[i][j][k] = newAct;

                // Scale probabilities with the distance from nearest ellipsoid.
                float minDistSize = distSize[i][j][k];

                // Extinction probability increases with distance, other two decrease.
                float scaledPCldExt = pCldExt * (1 - minDistSize);
                if (scaledPCldExt > 1) scaledPCldExt = 1;
                float scaledPHumExt = pHumExt * minDistSize;
                if (scaledPHumExt < 0) scaledPHumExt = 0;
                float scaledPActExt = pActExt * minDistSize;
                if (scaledPActExt < 0) scaledPActExt = 0;

                // Cloud extinction.
                if (cld[i][j][k])
                {
                    if ((float) (gen() % randomResolution) / randomResolution < scaledPCldExt)
                        cld[i][j][k] = false;
                }
                if ((float) (gen() % randomResolution) / randomResolution < scaledPHumExt)
                    hum[i][j][k] = true;
                if ((float) (gen() % randomResolution) / randomResolution < scaledPActExt)
                    act[i][j][k] = true;
            }
        }
    }
}

void CloudSimulator::createRandomCloud()
{
    std::cout << "Create a random cloud" << std::endl;

    if (clouds.size() > 1)
        clouds.pop_front();

    int distFromBorder = 25;
//    glm::vec3 position = glm::vec3(gen() % (x - 2 * distFromBorder) + distFromBorder,
//                                   gen() % (y - 2 * distFromBorder) + distFromBorder,
//                                   gen() % (z - 2 * distFromBorder) + distFromBorder);
    glm::vec3 position = glm::vec3(0.0f);
    int size = gen() % (maxCloudSize - minCloudSize) + minCloudSize;
    std::cout << "Position: " << glm::to_string(position) << " size: " << size << std::endl;
    Cloud cloud = Cloud(position, size);
    clouds.push_back(cloud);
}

void CloudSimulator::calculateDistSize(float ***distSize)
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            for (int k = 0; k < z; k++)
            {
                // Find the closest/largest cloud.
                float minDistSize = FLT_MAX;
                for (std::deque<Cloud>::iterator it = clouds.begin(); it != clouds.end(); it++)
                {
                    glm::vec3 pos = it->getPosition();
                    glm::vec3 voxel = glm::vec3(i, j, k);
//                    float dist = distFrom(i, j, k, pos.x, pos.y, pos.z);
                    float dist = glm::distance(pos, voxel);
                    if (dist / it->getSize() < minDistSize)
                        // We actually need just the size/dist ratio.
                        minDistSize = dist / it->getSize();
                }
                // Calculate exponent so we have rapid descent of cloud density.
                minDistSize = pow(exponentialBase, -minDistSize);
                if (minDistSize < 0.01) minDistSize = 0;
                distSize[i][j][k] = minDistSize;
            }
        }
    }
}

void CloudSimulator::calculateDensity(bool ***cld, float ***den)
{
    int S = 6; // Blur matrix is size SxSxS.
    for (int i = 0; i != x; ++i)
        for (int j = 0; j != y; ++j)
            for (int k = 0; k != z; ++k)
                // Do a box blur
                den[i][j][k] = singleDensity(i, j, k, cld, S);
}

float CloudSimulator::singleDensity(int i, int j, int k, bool ***cld, int S)
{
    // Go through kernel.
    int halfS = (S - 1) / 2;
    float sum = 0;
    for (int kI = i - halfS; kI <= i + halfS; ++kI)
        for (int kJ = j - halfS; kJ <= j + halfS; ++kJ)
            for (int kK = k - halfS; kK <= k + halfS; ++kK)
            {
                // Check if kernel isn't in the grid.
                if (kI < 0 || kJ < 0 || kK < 0 ||
                    kI >= x || kJ >= y || kK >= z)
                    continue; // Skip if it is
                if (cld[kI][kJ][kK]) sum += 1; // w_i=1; box filter
            }
    return sum / (S * S * S);
}

float CloudSimulator::fieldFunction(float a)
{
    return -4.0f / 9.0f * pow(a, 6) + 17.0f / 9 * pow(a, 4) - 22.0f / 9 * pow(a, 2) + 1;
}

void CloudSimulator::copyGrid(float ***src, float ***dest)
{
    for (int i = 0; i != x; ++i)
        for (int j = 0; j != y; ++j)
            for (int k = 0; k != z; ++k)
                dest[i][j][k] = src[i][j][k];
}

void CloudSimulator::computeFAc(bool ***fAc, bool ***act, int i, int j, int k, int x, int y, int z)
{
    if ((i + 1 < x && act[i + 1][j][k]) ||
        (j + 1 < y && act[i][j + 1][k]) ||
        (k + 1 < z && act[i][j][k + 1]) ||
        (i - 1 >= 0 && act[i - 1][j][k]) ||
        (j - 1 >= 0 && act[i][j - 1][k]) ||
        (k - 1 >= 0 && act[i][j][k - 1]) ||

        (i + 2 < x && act[i + 2][j][k]) ||
        (j + 2 < y && act[i][j + 2][k]) ||
        (i - 2 >= 0 && act[i - 2][j][k]) ||
        (j - 2 >= 0 && act[i][j - 2][k]) ||
        (k - 2 >= 0 && act[i][j][k - 2]))
        fAc[i][j][k] = true;
    else
        fAc[i][j][k] = false;
}

float CloudSimulator::distFrom(int x1, int y1, int z1, int x2, int y2, int z2)
{
    return static_cast<float>(sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2)));
}

CloudRender::CloudRender(int length, int width, int height)
{
    // Generate the 3D textures.
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &volumeTexture);
    glBindTexture(GL_TEXTURE_3D, volumeTexture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_3D);

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

//    GLfloat vertices[] = {
//            0.5f, 0.5f, 0.0f,  // Top Right
//            0.5f, -0.5f, 0.0f,  // Bottom Right
//            -0.5f, -0.5f, 0.0f,  // Bottom Left
//            -0.5f, 0.5f, 0.0f   // Top Left
//    };
//    GLuint indices[] = {  // Note that we start from 0!
//            0, 1, 3,  // First Triangle
//            1, 2, 3   // Second Triangle
//    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GLfloat vertices[] = {
            0.0, 0.0, 0.0,
            0.0, 0.0, 1.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 1.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, 1.0,
            1.0, 1.0, 0.0,
            1.0, 1.0, 1.0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    GLuint indices[] = {
            0, 1, 2,
            2, 1, 3,
            4, 6, 5,
            6, 7, 5,
            1, 5, 3,
            3, 5, 7,
            0, 2, 4,
            2, 6, 4,
            6, 2, 3,
            6, 3, 7,
            1, 0, 4,
            5, 1, 4
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    // Unbind VAO.
    glBindVertexArray(0);

    // Initialize interpolated data.
    interpolatedData = new float **[length];
    for (int i = 0; i != length; i++)
    {
        interpolatedData[i] = new float *[width];
        for (int j = 0; j != width; j++)
            interpolatedData[i][j] = new float[height];
    }
}

void CloudRender::Render(CloudData &data, Shader shader, float startTime)
{
    interpolateCloudData(data, startTime);
    renderRayCastingClouds(data, shader, startTime);
}

void CloudRender::interpolateCloudData(CloudData &data, float startTime)
{
    int x = data.getGridLength();
    int y = data.getGridWidth();
    int z = data.getGridHeight();

    // Calculate relative difference for linear interpolation.
    float relDiff = (startTime - data.nextTime) / (data.nextTime - data.prevTime);
    if (relDiff > 1.0f) relDiff = 1.0f;

    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            for (int k = 0; k < z; ++k)
            {
                if (data.prevDensity[i][j][k] > 0.0f)
                {
                    // Linearly interpolate the density.
                    interpolatedData[i][j][k] = data.prevDensity[i][j][k] +
                                                relDiff * (data.nextDensity[i][j][k] - data.prevDensity[i][j][k]);
                } else
                {
                    interpolatedData[i][j][k] = 0.0f;
                }
            }
        }
    }
}

void CloudRender::renderRayCastingClouds(CloudData &data, Shader shader, float startTime)
{
    glBindVertexArray(VAO);
    glDisable(GL_CULL_FACE);

    int x = data.getGridLength();
    int y = data.getGridWidth();
    int z = data.getGridHeight();

    // Convert float*** to float* (stream of data).
    auto texData = new float[x * y * z];
    int pos = 0;
    for (int i = 0; i != x; i++)
    {
        for (int j = 0; j != y; j++)
        {
            for (int k = 0; k != z; k++)
            {
                texData[pos] = interpolatedData[i][j][k];
                pos++;
            }
        }
    }

    // Fill the data into 3D texture. A texture cell includes only one component (GL_RED = density, float).
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, x, y, z, 0, GL_RED, GL_FLOAT, texData);
    delete[] texData;

    // Bind Textures using texture units.
    glUniform1i(glGetUniformLocation(shader.ID, "density"), 0);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Unbind VAO.
    glBindVertexArray(0);
}

#pragma clang diagnostic pop