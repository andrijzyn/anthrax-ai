layout (location = 0) in vec4 inpos;
layout (location = 1) in vec3 innormal;
layout (location = 2) in vec3 incolor;
layout (location = 3) in vec2 incoord;

layout (location = 0) out vec4 outfragcolor;

layout(set = 0, binding = 0) uniform  CameraBuffer {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 viewproj;
	vec4 viewpos;
    vec4 pos;
    vec4 viewport;

    vec4 lightcolor;
	vec4 lightpos;
	float ambient;
	float diffuse;
	float specular;
} cameradata;

layout(set = 1, binding = 0) uniform sampler2D texturesampler;