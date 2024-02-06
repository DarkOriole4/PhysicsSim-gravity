#include <sim_headers.h>

#include <glm/glm.hpp>

void simulate(glm::vec2* translations, int particle_count) {
    int index = 0;
        for (int i = 0; i < sqrt(particle_count); i++) {
            for (int j = 0; j < sqrt(particle_count); j++) {
                translations[index] = glm::vec2(((float)i / (float)sqrt(particle_count) - 0.5f), (float)j / (float)sqrt(particle_count) - 0.5f);;
                index++;
            }
        }
}