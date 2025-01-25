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
layout(location = 2) in int inQuadIndex;

layout(location = 0) out vec4 fragColor;

mat4 Translate(vec3 delta)
{
    // return mat4(
    //     vec4(1.0, 0.0, 0.0, 0.0),
    //     vec4(0.0, 1.0, 0.0, 0.0),
    //     vec4(0.0, 0.0, 1.0, 0.0),
    //     vec4(delta.xyz,     1.0)
    // );

    // return mat4(
    //     vec4(1.0, 0.0, 0.0, delta.x),
    //     vec4(0.0, 1.0, 0.0, delta.y),
    //     vec4(1.0, 0.0, 1.0, delta.z),
    //     vec4(0.0, 0.0, 0.0, 1.0)
    // );

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

mat4 TranslateAndScale(vec3 position, vec3 scale)
{
    mat4 m = mat4(1.0);
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
    m[3] = vec4(position, 1.0);
    return m;
}

void main() {
    mat4 transform = Translate(quadData.quadVertexData[inQuadIndex].position) * Scale(quadData.quadVertexData[inQuadIndex].scale);
    //mat4 transform = TranslateAndScale(quadData.quadVertexData[inQuadIndex].position, quadData.quadVertexData[inQuadIndex].scale);

    gl_Position = ubo.proj * ubo.view * (transform * inVertexPosition);
    fragColor = inColor;
}


// Per Pixel
#type Pixel
#version 450

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}

