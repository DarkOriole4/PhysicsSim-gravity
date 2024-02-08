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
    //std::cout << dtime << std::endl;

    float expected_diff = 1.0f / target_fps;
    if (dtime < expected_diff) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((expected_diff - dtime) * 1000)));
    }

    //------ I'll come up with a better solution later ---------
    // auto real_end = std::chrono::high_resolution_clock::now();
    // float real_dtime = (float) std::chrono::duration_cast<std::chrono::milliseconds>(real_end - start).count() / 1000.0f;
    // std::cout << real_dtime << std::endl;

    start = std::chrono::high_resolution_clock::now();
}


struct matter {
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    float mass = 0;
    float velocity = 0;
};

glm::vec3 center_of_mass(const matter* bodies, const int& skip_index, const int& particle_count) {
    float x;
    float y;
    float total_mass;

    for (int i = 0; i < particle_count; i++) {
        if (i != skip_index) {
            x += (bodies[i].position.x * bodies[i].mass);
            y += (bodies[i].position.y * bodies[i].mass);
            total_mass += bodies[i].mass;
        }
    }
    return glm::vec3(x / total_mass, y / total_mass, total_mass);
}

float distance (const glm::vec2& pos1, glm::vec2& pos2) {
    return sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));
}


void simulate (glm::vec2* translations, int particle_count, const int& fps) {
    matter bodies[particle_count];
    const float G = 9.81;
    float r;
    float force;
    float total_mass;
    glm::vec2 direction;
    glm::vec2 center;
    glm::vec3 center_info;

    for (int i = 0; i < particle_count; i++) {
            center_info = center_of_mass(bodies, i, particle_count);
            center = glm::vec2(center.x, center.y);
            total_mass = center_info.z;

            r = distance(bodies[i].position, center);
            if (r != 0) {
                direction = glm::vec2(center- bodies[i].position) / r;

                force = G * ((bodies[i].mass * total_mass) / pow(r, 2));
                bodies[i].velocity += force / bodies[i].mass / fps;
                bodies[i].position += bodies[i].velocity * direction;
            } else {
                cout << "attempted division by zero" << endl;
            }
        }

    //export the translations to opengl
    for (int j = 0; j < particle_count ; j++) {
        translations[j] = bodies[j].position;
    }
}

/* to do:
    - create some start situations
    - figure out why the screen is black
    - add basic ui
    - mouse as black hole?
*/