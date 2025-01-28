// Per vertex
#type Vertex
#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 v_TexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 0.0, 1.0);
    v_TexCoord = texCoord;
    fragColor = color;
}


// Per Pixel
#type Pixel
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texSampler;

void main() {
    // outColor = vec4(v_TexCoord, 0.0, 1.0);
    outColor = texture(texSampler, v_TexCoord);
}

