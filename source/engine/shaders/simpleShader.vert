#version 450
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vUv;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outCoord;

layout(set = 0, binding = 0) uniform  CameraBuffer{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec4 color;
} cameraData;

//push constants block
layout( push_constant ) uniform constants
{
 vec4 data;
 mat4 render_matrix;
} PushConstants;

void main()
{
    //mat4 transformMatrix = (cameraData.viewproj * PushConstants.render_matrix);
    gl_Position = vec4(vPosition, 1.0f);
    outColor = vColor;
    outCoord = vUv;
}
