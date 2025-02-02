// Per vertex
#type Vertex
#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
    mat4 view;
} ubo;

struct QuadVertexData {
    vec3 position;
    vec3 scale;
};

layout(std140, set = 0, binding = 1) readonly buffer QuadData {
    QuadVertexData[] quadVertexData;
} quadData;

layout(location = 0) in vec4 inVertexPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in int inTextureIndex;
layout(location = 4) in int inQuadIndex;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out int v_TextureIndex;

mat4 Translate(vec3 delta)
{
    mat4 m = mat4(1.0);
    m[0][0] = 1;
    m[1][1] = 1;
    m[2][2] = 1;
    m[3].xyzw = vec4(delta, 1.0);
    return m;
}

mat4 Scale(vec3 scale) {
    mat4 m = mat4(1.0);
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
    return m;
}

void main() {
    mat4 transform = Translate(quadData.quadVertexData[inQuadIndex].position) * Scale(quadData.quadVertexData[inQuadIndex].scale);

    gl_Position = ubo.proj * ubo.view * (transform * inVertexPosition);
    fragColor = inColor;
    v_TexCoord = inTexCoord;
    v_TextureIndex = inTextureIndex;
}


// Per Pixel
#type Pixel
#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in flat int v_TextureIndex;

layout (binding = 2) uniform sampler2D u_Textures[32];

layout(location = 0) out vec4 outColor;

void main() {
    // outColor = vec4(v_TexCoord, 0.0, 1.0);
    outColor = texture(u_Textures[v_TextureIndex], v_TexCoord) * fragColor;
}

