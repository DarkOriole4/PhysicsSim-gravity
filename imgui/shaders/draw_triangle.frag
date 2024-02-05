#version 330 core
    in vec3 vertexColor;
    in vec3 vertexPosition;
    out vec4 fragColor;

    //uniform float aRadius;
    //uniform mat4 matrix;

    void main()
    {
        float aRadius = 0.5f;
        float dist = distance(vertexPosition.xy, vec2(0.0f, 0.0f));
        if (dist > aRadius)
            discard;

        fragColor = vec4(vertexColor, 1.0f);
    }