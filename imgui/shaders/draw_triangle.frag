#version 330 core
    in vec3 vertexPosition;
    out vec4 fragColor;

    void main()
    {
        float aRadius = 0.01f;
        float dist = distance(vertexPosition.xy, vec2(0.0f, 0.0f));
        if (dist > aRadius)
            discard;

        fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f); // blue
    }