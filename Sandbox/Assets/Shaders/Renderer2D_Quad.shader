#type Vertex
#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
    mat4 view;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * vec4(inPosition, 1.0);
    fragColor = inColor;
}


#type Pixel
#version 450

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}

