#version 330 core
    layout (location = 0) in vec3 inLocalSpace;       
    out vec3 vertexColor;
    out vec3 vertexPosition;

    uniform mat4 matrix;
    uniform vec2 offsets[1000];

    void main()                                         
    {                                         
        vec2 offset = offsets[gl_InstanceID];
        gl_Position = matrix * (vec4(inLocalSpace, 1.0) + vec4(offset, 0.0f, 0.0f));   
        vertexPosition = inLocalSpace;
    }