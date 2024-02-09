#version 330 core
    in vec3 vertexPosition;
    in float velocity;
    in float radius;
    out vec4 fragColor;

    float aRadius = radius / 1000;
    const vec3 slowColor = vec3(0.0f, 1.0f, 1.0f);
    const vec3 fastColor = vec3(1.0f, 1.0f, 0.0f); 
    vec3 lerpColor;

    void main()
    {
        float dist = distance(vertexPosition.xy, vec2(0.0f, 0.0f));
        if (dist > aRadius)
            discard;

        lerpColor = mix(slowColor, fastColor, velocity * 2);
        fragColor = vec4(lerpColor, 1.0f);
    }