#ifndef _WAVE_H
#define _WAVE_H

#include <random>
#include <complex>
#include <glm/glm.hpp>

/*
 * Ocean surface synthesized by FFT:
 * "Simulating Ocean Water", J. Tessendorf,
 * SIGGRAPH course notes, 2005.
 */

// Get k vector from mesh grid (n,m)
#define K_VEC(n, m) glm::vec2(2 * M_PI * (n - N / 2) / L_x, 2 * M_PI * (m  - M / 2) / L_z)

class Wave
{
public:
    // N, M:      Resolution
    // L_x, L_z:  Actual lengths of the grid (as in meters)
    // omega_hat: Direction of wind
    // V:         Speed of wind
    Wave(int N, int M, float L_x, float L_z, glm::vec2 omega, float V, float A, float lambda);

    ~Wave();

    void buildField(float time);

    glm::vec3 *heightField;
    glm::vec3 *normalField;

private:
    std::complex<float> *value_h_twiddle_0 = NULL;
    std::complex<float> *value_h_twiddle_0_conj = NULL;
    std::complex<float> *value_h_twiddle = NULL;

    std::default_random_engine generator;
    std::normal_distribution<float> normal_dist;

    const float pi = float(M_PI);
    const float g = 9.81f; // Gravitational constant
    const float l = 0.1;
    float A;
    float V;
    int N, M;
    int kNum;
    glm::vec2 omega_hat;
    float lambda;
    float L_x, L_z;

    inline float omega(float k) const
    {
        return sqrt(g * k);
    }

    inline float phillips_spectrum(glm::vec2 vec_k) const
    {
        if (vec_k == glm::vec2(0.0f, 0.0f))
            return 0.0f;

        // Largest possible waves arising from a continuous wind of speed V
        float L = V * V / g;

        float k = glm::length(vec_k);
        glm::vec2 k_hat = normalize(vec_k);

        float dot_k_hat_omega_hat = glm::dot(k_hat, omega_hat);
        float result = A * exp(-1 / (k * L * k * L)) / pow(k, 4) * pow(dot_k_hat_omega_hat, 2);

        result *= exp(-k * k * l * l);  // Eq24

        return result;
    }

    // Eq25
    inline std::complex<float> h_twiddle_0(glm::vec2 vec_k)
    {
        float xi_r = normal_dist(generator);
        float xi_i = normal_dist(generator);
        return sqrt(0.5f) * std::complex<float>(xi_r, xi_i) * sqrt(phillips_spectrum(vec_k));
    }

    // Eq26
    inline std::complex<float> h_twiddle(int kn, int km, float t) const
    {
        int index = km * N + kn;
        float k = glm::length(K_VEC(kn, km));
        std::complex<float> term1 = value_h_twiddle_0[index] * exp(std::complex<float>(0.0f, omega(k) * t));
        std::complex<float> term2 = value_h_twiddle_0_conj[index] * exp(std::complex<float>(0.0f, -omega(k) * t));
        return term1 + term2;
    }
};

#endif
