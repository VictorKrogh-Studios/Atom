// Per vertex
#type Vertex
#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_TexCoord;

void main() {
    gl_Position = position;
    v_TexCoord = texCoord;
    fragColor = color;
}


// Per Pixel
#type Pixel
#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragTexCoord, 0.0, 1.0);
}

