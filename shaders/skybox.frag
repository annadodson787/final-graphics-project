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

// Light properties
struct light
{
	ivec4 att;
	vec4 pos;
	vec4 dir;
	vec4 amb;
	vec4 dif;
	vec4 spec;
	vec4 atten;
	vec4 r;
};

layout (std140) uniform lights
{
	vec4 amb;	// ambient color
	ivec4 lt_att;	// lt_att[0]: number of lights
	light lt[2];	// Values for each light
};

uniform sampler2D tex;
uniform samplerCube skybox;
in vec3 TexCoords;

// Input attributes, written in the vertex shader and interpolated
in vec3 vtx_normal;
in vec3 vtx_position;
in vec2 vtx_uv;
in vec4 vtx_tangent;

in float visibility;
in vec4 background_color;

// Output attributes, final color of the fragment
out vec4 frag_color;

vec3 lamb(int i, vec4 baseColor, vec3 n, vec3 v) {
	vec3 lightDir =  normalize(-lt[i].dir.xyz);
	vec3 lightCol = lt[i].dif.rgb;
	vec3 ambient_color = amb.rgb;
	float diff = max(dot(n, lightDir), 0.0);
	vec3 ambient = ambient_color;
	vec3 lamb_lighting = ambient + 0.5 * diff * lightCol* baseColor.rgb;
	return lamb_lighting;
}

vec3 lighting(vec4 baseColor, vec3 n, vec3 v) {
	vec3 color = vec3(0);
	for(int i=0;i<lt_att[0];i++) {
		color+=lamb(i,baseColor,n,v);
	}
	return color;
}

void main()
{
	frag_color = texture(skybox, TexCoords);

	// Implement fog as mix of background water color and visibility
	frag_color = mix(background_color, frag_color, visibility);
	frag_color.w = 1;
}
