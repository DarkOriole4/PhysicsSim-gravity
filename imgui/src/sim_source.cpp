#include <sim_headers.h>

#include <glm/glm.hpp>
#include <random>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;


void framerate_control (std::chrono::high_resolution_clock::time_point& start, std::chrono::high_resolution_clock::time_point& end, float& dtime, const int& target_fps) {
    end = std::chrono::high_resolution_clock::now();
    dtime = (float) std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0f;

    float expected_diff = 1.0f / target_fps;
    if (dtime < expected_diff) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((expected_diff - dtime) * 1000)));
    }

    start = std::chrono::high_resolution_clock::now();
}


glm::vec3 center_of_mass(const matter* bodies, const int& skip_index, const int& particle_count) {
    float x = 0;
    float y = 0;
    int total_mass = 0;

    for (int i = 0; i < particle_count; i++) {
        if (i != skip_index) {
            x += (bodies[i].position.x * (float) bodies[i].mass);
            y += (bodies[i].position.y * (float) bodies[i].mass);
            total_mass += bodies[i].mass;
        }
    }

    if (total_mass != 0) {
        return glm::vec3(x / static_cast<float>(total_mass), y / static_cast<float>(total_mass), total_mass);
    } else {
        // Return an arbitrary value indicating no mass if total_mass is zero
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

float distance (const glm::vec2& pos1, glm::vec2& pos2) {
    return sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));
}

void create_start_condition(matter* bodies, const int& particle_count) {
    //random start positions
    int resolution = 128;
    float size = 20.0; // size of the region in which the positions will be placed

    std::random_device rd {"/dev/urandom"};
    static std::uniform_int_distribution<int> d1(0, resolution); // for the positions
    static std::uniform_int_distribution<int> d2(0, 6.28f); // for the angles (0 to 2*PI)
    
    for (int i = 0; i < particle_count; i++) {
        // set random position
        float x1 = ((float) d1(rd) / (float) resolution - 0.5) * size;
        float y1 = ((float) d1(rd) / (float) resolution - 0.5) * size;
        bodies[i].position = glm::vec2(x1, y1);

        // set random angle
        float speed_multiplier = 0.01;
        float phi = (float) d2(rd);
        float x2 = cos(phi) * speed_multiplier;
        float y2 = sin(phi) * speed_multiplier;
        bodies[i].velocity = glm::vec2(x2, y2);
    }
}


void simulate (glm::vec2* translations, float* velocities, matter* bodies, int particle_count, const int& fps, const float& sim_speed) {
    const float G = 6.674;
    const float min_radius = 0.5;

    float sim_G = G / particle_count;
    float r;
    float force;
    matter* m1;
    matter* m2;
    glm::vec2 direction;

    float time_step = sim_speed / fps;

    for (int i = 0; i < particle_count; i++) {
        for (int j = 0; j < particle_count; j++) {
            if (i > j) {
                m1 = &bodies[i];
                m2 = &bodies[j];

                r = distance(m1->position, m2->position);
                if (r > min_radius) {
                    direction = glm::vec2(m2->position - m1->position) / r; // normalized vector that points from m1 to m2
                    force = sim_G * ((m1->mass * m2->mass) / pow(r, 2));
                    
                    // apply physics step to m1
                    m1->velocity += (force / m1->mass * time_step) * direction;
                    m1->position += m1->velocity * glm::vec2(time_step);

                    // apply physics step to m2
                    m2->velocity += (force / m2->mass * time_step) * -direction;
                    m2->position += m2->velocity * glm::vec2(time_step); //opposite direction
                }
            }  
        }
    }

    //export the translations to opengl
    for (int n = 0; n < particle_count ; n++) {
        translations[n] = bodies[n].position / glm::vec2(10.0);

        velocities[n] = glm::length(bodies[n].velocity);
    }
}

/* to do:
    - create some start situations
    - figure out why the screen is black
    - add basic ui
    - mouse as black hole?
*/