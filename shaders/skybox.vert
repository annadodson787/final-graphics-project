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

out vec3 TexCoords;
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
  TexCoords = pos.xyz;
  gl_Position = pvm * model * vec4(pos.xyz, 1.f);

	float distance = length(positionToCamera.xyz);
	visibility = exp(-pow(distance*density, gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	background_color = vec4(0.f,.15f,.30f, 0.f);
}
