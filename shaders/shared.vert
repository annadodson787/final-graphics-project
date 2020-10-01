#version 330 core

// Camera properties
layout (std140) uniform camera
{
	// View -> Screen transformation matrix
	mat4 projection;
	// World -> View coordinate transformation
	mat4 view;
	// Premultiplied projection * view (for efficiency)
	mat4 pvm;
	// Orthographic projection (not used in this assignment)
	mat4 ortho;
	// Camera world space position
	vec4 position;
};

// Model -> World transformation matrix
uniform mat4 model=mat4(1.0f);

// Available input
// Object space position
layout (location=0) in vec4 pos;
// Object space normal
layout (location=2) in vec4 normal;
// Vertex texture coordinate
layout (location=3) in vec4 uv;
// Vertex texture coordinate
layout (location=4) in vec4 tangent;

// Values to pass to the fragment shader (interpolated)
// World space vertex position
out vec3 vtx_position;
// World space vertex normal
out vec3 vtx_normal;
// Vertex uv coordinate
out vec2 vtx_uv;
out vec2 vtx_normalized_uv;
out vec2 vtx_caustic_uv;

// World space tangent vector and bitangent sign
out vec4 vtx_tangent;
// Visibility
out float visibility;
// Background Color;
out vec4 background_color;

// Fog values
const float density = .01;
const float gradient = 1.5;

void main()
{
	vec4 positionToCamera = view * model * vec4(pos.xyz,1.f);
	gl_Position=pvm*model*vec4(pos.xyz,1.f);
	vtx_position=vec3(model*vec4(pos.xyz,1.f));
	vtx_normal=vec3(transpose(inverse(model))*vec4(normal.xyz,0.f)).xyz;
	vtx_uv = uv.xy;
	vtx_normalized_uv = vec2((pos.x)/500.f, (pos.z)/500.f);

	float sampler_x = pos.x - int((pos.x)/100)* 100 + normal.x;
	float sampler_y = pos.z - int((pos.z)/100)* 100 + normal.z;
	vtx_caustic_uv = vec2(sampler_x /10.f, sampler_y/20.f);

	float distance = length(positionToCamera.xyz);
  visibility = exp(-pow(distance*density, gradient));
  visibility = clamp(visibility, 0.0, 1.0);

  vtx_tangent = tangent;

  background_color = vec4(0.f,.15f,.30f, 0.f);
}
