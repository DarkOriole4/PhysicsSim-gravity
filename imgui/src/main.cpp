// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// @ts-ignore
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>
#include <cmath>
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE
#if defined(IMGUI_IMPL_OPENGL_ES2)
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <imgui_spectrum.h>
#include <glad.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sim_headers.h>
#include <chrono>

using namespace std;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

char* loadShader(const char* path) {
    fstream file(path, ios::in); // open file
    
    if (!file.is_open()) {
        exit(1);
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg(); // figure out buffer size
    file.seekg(0, std::ios::beg);
    
    char* buffer = new char[size + 1]; // allocate a buffer for the file
    if (!file.read(buffer, size)) {
        cerr << "Error reading file: " << path << endl;
        delete[] buffer;
        return nullptr;
    }
    
    file.close();
    buffer[size] = '\0';
    return buffer;
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Physics Sim - gravity", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::Spectrum::StyleColorsSpectrum();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    gladLoadGL();
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    io.Fonts->AddFontFromFileTTF("../misc/fonts/Roboto-Medium.ttf", 16.0f);

    // OPENGL SHADER SETUP
    
    // vertex shader source code
    char* vsSrc = loadShader("../shaders/draw_triangle.vert");

    // fragment shader source code
    char* fsSrc = loadShader("../shaders/draw_triangle.frag");

    // create program object
    unsigned int pId = glCreateProgram();

    // define a triangle
    float vertices[] = {
    -0.866f, -0.5f, 0.0f,
    0.866f, -0.5f, 0.0f,
    0.0f, 1.0f, 0.0f};


    // create shaders
    unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    if (vsSrc != nullptr && fsSrc != nullptr) {
        glShaderSource(vertexShaderId, 1, &vsSrc, NULL);
        delete []vsSrc;

        glShaderSource(fragmentShaderId, 1, &fsSrc, NULL);
        delete []fsSrc;
    }

    glCompileShader(vertexShaderId);
    glCompileShader(fragmentShaderId);

    GLint success;
    GLchar infoLog[512];

    // Check vertex shader compilation
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Check fragment shader compilation
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // attachment of shaders to program object
    glAttachShader(pId, vertexShaderId);
    glAttachShader(pId, fragmentShaderId);

    glLinkProgram(pId);

    // buffer
    unsigned int vertexArray;
    unsigned int vertexBuffer;
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    // predefinitions
    int particle_count = 100;
    int target_fps = 144;
    float sim_speed = 1.0f;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    float dtime;
    
    glm::vec2 translations[particle_count];
    matter bodies[particle_count];
    create_start_condition(bodies, particle_count);


    while (!glfwWindowShouldClose(window))
#endif
    {
        // SIMULATION STUFF
        framerate_control(start, end, dtime, target_fps); //update the dtime variable and keep the fps steady
        simulate(translations, bodies, particle_count, target_fps, sim_speed); //calculate and update the particles positions

        // -----------------------------  

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // DISPLAY SIMULATION INFO
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoBackground;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;
        //window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

        // etc.
        bool value = true;
        bool* open_ptr = &value;
        ImGui::Begin("Metadata Window", open_ptr, window_flags);           
        
        ImGui::SetWindowFontScale(1.3f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("particle count : %d", particle_count);
        ImGui::Text("frametime : %.3f ms", 1000.0f / io.Framerate);
        ImGui::Text("%.1f fps", io.Framerate);
        ImGui::PopStyleColor();

        ImGui::End();
        
        // INTERACTIVE SIMULATION SETTINGS

        window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoBackground;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;
        //window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

        // etc.
        ImGui::Begin("Settings Window", open_ptr, window_flags);           
        
        ImGui::SetWindowFontScale(1.3f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("sim_speed");
        ImGui::PopStyleColor();

        ImGui::SetWindowFontScale(1.0f);
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::SetNextItemWidth(200);
        
        ImGui::SliderFloat(".", &sim_speed, 0.0f, 2.0f);
        
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        

        ImGui::End();


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        glClearColor(0.05f, 0.05f, 0.06f, 0); // background color
        glClear(GL_COLOR_BUFFER_BIT);

    // SET UNIFORMS ---------------------------
        glUseProgram(pId);
        int matrixLocation = glGetUniformLocation(pId, "matrix");
        int offsetsLocation = glGetUniformLocation(pId, "offsets");

        if (matrixLocation == -1) {
            cout << "failed to locate uniform [matrixLocation]" << endl;
        }

        if (offsetsLocation == -1) {
            cout << "failed to locate uniform [offsetsLocation]" << endl;
        }

        float sceneScale = 0.003;
        glm::mat4 projection = glm::ortho(-(display_w / 2) * sceneScale, display_w / 2 * sceneScale, -(display_h / 2) * 0.75f * sceneScale, display_h / 2 * 0.75f *sceneScale, 0.01f, 100.0f); //ortho projection
        // define ortho projection matrix, also sets up the scene coordinates and scale. The center of the screen is now (0, 0) [ 0.75 is a correction for the aspect ratio ]

        glm::mat4 view = glm::lookAt(
        glm::vec3(0,1,1), // Camera is at (4,3,3), in World Space
        glm::vec3(0,0,0), // and looks at the origin
        glm::vec3(0,0,-1)  // Head is up (set to 0,-1,0 to look upside-down)
        );

        glm::mat4 mvp = projection * view;
        

        //send over matrix info
        glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &mvp[0][0]);

        //send over particle info
        glUniform2fv(offsetsLocation, particle_count, glm::value_ptr(translations[0]));
        
        // -------------------------------
        
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, particle_count);  
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
