#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragLightDir;
layout(location = 3) in vec3 fragViewDir;
layout(location = 4) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConsts {
    int colorMode; // 0: Vertex Color, 1: Normal Visualizer, 2: Rainbow Gradient, 3: Depth/Mist
    float shininess;
    float time;
    float ambientIntensity;
    int gpuLoadIterations;
} push;

void main() {
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(fragLightDir);
    vec3 V = normalize(fragViewDir);
    vec3 R = reflect(-L, N);

    // Color modes
    vec3 baseColor;
    if (push.colorMode == 0) {
        baseColor = fragColor;
    } else if (push.colorMode == 1) {
        baseColor = N * 0.5 + 0.5;
    } else if (push.colorMode == 2) {
        // Rainbow gradient based on position and time
        baseColor = vec3(
            sin(fragPos.x * 2.0 + push.time) * 0.5 + 0.5,
            cos(fragPos.y * 2.0 + push.time) * 0.5 + 0.5,
            sin(fragPos.z * 2.0 + push.time * 0.5) * 0.5 + 0.5
        );
    } else {
        // Depth/Mist visualize
        float dist = length(fragPos) * 0.15;
        baseColor = vec3(dist, dist * 0.8, dist * 0.5);
    }

    // Phong lighting
    float ambient = push.ambientIntensity;
    
    float diffuse = max(dot(N, L), 0.0);
    
    float specular = 0.0;
    if (diffuse > 0.0) {
        specular = pow(max(dot(R, V), 0.0), push.shininess);
    }

    // Dummy GPU workload loop
    float dummy = 0.0;
    for (int i = 0; i < push.gpuLoadIterations; i++) {
        dummy += sin(float(i) * 0.01 + push.time) * cos(float(i) * 0.02 + fragPos.x);
        dummy = sqrt(abs(dummy)) + sin(dummy);
    }

    vec3 lightColor = vec3(1.0, 1.0, 0.95);
    vec3 result = (ambient + diffuse) * baseColor + specular * lightColor * 0.8 + vec3(dummy * 0.000001);
    
    outColor = vec4(result, 1.0);
}
