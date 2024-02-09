#version 330 core
    in vec3 vertexPosition;
    out vec4 fragColor;

    const float aRadius = 0.007f;
    const vec3 baseColor = vec3(0.0f, 1.0f, 1.0f); // blue

    void main()
    {
        float dist = distance(vertexPosition.xy, vec2(0.0f, 0.0f));
        if (dist > aRadius)
            discard;

        fragColor = vec4(baseColor, 1.0f);
    }