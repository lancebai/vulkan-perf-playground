#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 lightPos;
    vec4 viewPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragLightDir;
layout(location = 3) out vec3 fragViewDir;
layout(location = 4) out vec3 fragPos;

void main() {
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    fragColor = inColor;
    fragNormal = mat3(transpose(inverse(ubo.model))) * inNormal;
    fragLightDir = ubo.lightPos.xyz - worldPos.xyz;
    fragViewDir = ubo.viewPos.xyz - worldPos.xyz;
    fragPos = worldPos.xyz;
}
