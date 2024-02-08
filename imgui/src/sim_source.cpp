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
    int resolution = 1024;
    float size = 2.0;

    std::random_device rd {"/dev/urandom"};
    static std::uniform_int_distribution<int> d(0, resolution);
    
    for (int i = 0; i < particle_count; i++) {
        float x = ((float) d(rd) / (float) resolution - 0.5) * size;
        float y = ((float) d(rd) / (float) resolution - 0.5) * size;
        bodies[i].position = glm::vec2(x, y);
    }
}


void simulate (glm::vec2* translations, matter* bodies, int particle_count, const int& fps) {
    const float G = 6.674;
    float r;
    float force;
    int total_mass;
    glm::vec2 direction;
    glm::vec2 center;
    glm::vec3 center_info;

    for (int i = 0; i < particle_count; i++) {
            center_info = center_of_mass(bodies, i, particle_count);
            center = glm::vec2(center_info.x, center_info.y);
            total_mass = center_info.z;

            r = distance(bodies[i].position, center);
            if (r != 0) {
                direction = glm::vec2(center - bodies[i].position) / r;

                force = G * ((bodies[i].mass * total_mass) / pow(r, 2));
                bodies[i].velocity += force / bodies[i].mass / fps;
                bodies[i].position += bodies[i].velocity / fps * direction ;
                std::cout << force << std::endl;
                std::cout << bodies[i].velocity << std::endl;
                std::cout << "(" << bodies[i].position.x << ", " << bodies[i].position.y << ")" << std::endl;
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