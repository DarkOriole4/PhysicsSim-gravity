#version 330 core
    layout (location = 0) in vec3 inLocalSpace;       
    out float velocity;
    out vec3 vertexPosition;
    out float radius;

    uniform mat4 matrix;
    uniform vec2 offsets[1000];
    uniform float velocities[1000];
    uniform float particle_size;
    vec2 offset;

    void main()                                         
    {                                         
        offset = offsets[gl_InstanceID];
        gl_Position = matrix * (vec4(inLocalSpace, 1.0) + vec4(offset, 0.0f, 0.0f));   
        vertexPosition = inLocalSpace;
        velocity = velocities[gl_InstanceID];
        radius = particle_size;
    }