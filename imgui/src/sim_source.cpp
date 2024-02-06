#include <sim_headers.h>

#include <glm/glm.hpp>
#include <random>

void simulate(glm::vec2* translations, int particle_count) {
    int resolution = 1024;
    float size = 2.0;

    std::random_device rd {"/dev/urandom"};
    static std::uniform_int_distribution<int> d(0, resolution);
    
    for (int i = 0; i < particle_count; i++) {
        float x = ((float) d(rd) / (float) resolution - 0.5) * size;
        float y = ((float) d(rd) / (float) resolution - 0.5) * size;
        translations[i] = glm::vec2(x, y);
    }
}