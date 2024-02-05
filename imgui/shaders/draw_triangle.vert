#version 330 core
    layout (location = 0) in vec3 inLocalSpace;
    layout (location = 1) in vec3 inColor;        
    out vec3 vertexColor;
    out vec3 vertexPosition;

    uniform mat4 matrix;

    void main()                                         
    {                                         
        gl_Position = matrix * vec4(inLocalSpace, 1.0);  
        vertexColor = inColor;   
        vertexPosition = inLocalSpace;
    }