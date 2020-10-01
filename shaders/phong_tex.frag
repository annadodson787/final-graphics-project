////////////////////////////////////////////////
// You don't need to change this part
////////////////////////////////////////////////

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
	vec4 amb;		// ambient color
	ivec4 lt_att;	// lt_att[0]: number of lights
	light lt[2];	// Values for each light
};


// Texture samplers (PART 3/4)
uniform sampler2D albedo;
uniform sampler2D emissive;
uniform sampler2D metallic;
uniform sampler2D roughness;
uniform sampler2D normal;
uniform sampler2D alpha;

uniform sampler2D caustic;
uniform sampler2D caustic2;
uniform sampler2D caustic3;
uniform sampler2D caustic4;
uniform sampler2D caustic5;

// Input attributes, written in the vertex shader and interpolated
// Part 2
in vec3 vtx_normal;
in vec3 vtx_position;

// Part 3
in vec2 vtx_uv;

in vec2 vtx_normalized_uv;
in vec2 vtx_caustic_uv;

// Part 4
in vec4 vtx_tangent;

in vec4 background_color;
in float visibility;

// Final color of the fragment
out vec4 frag_color;


// Forward declare the phong lighting function
vec3 phong(int i, vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength, vec4 caustic);



// Compute lighting for each light
vec3 lighting(vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength, vec4 caustic) {
	vec3 color = vec3(0);

	// Iterate through all lights and compute lighting
	for(int i=0;i<lt_att[0];i++) {
		color+=phong(i,baseColor,n,v,shininess,specularStrength, caustic);
	}

	return color;
}



// Compute the ambient, diffusive, and specular lighting using Phong Lighting
vec3 phong(int i, vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength, vec4 caustic) {

	vec3 lightDir =  normalize(-lt[i].dir.xyz);
	vec3 lightCol = lt[i].dif.rgb * vec3(caustic.r, caustic.g + .05, caustic.b + .1);
	vec3 ambient_color = amb.rgb;

	vec3 phong_lighting = vec3(0);

	// Ambient Light
	phong_lighting += ambient_color;

	// Diffusive Light
	phong_lighting += lightCol * baseColor.xyz * max(0, dot(lightDir, n));

	// Specular Light
	vec3 r = vec3(2 * dot(lightDir, n) * n - lightDir);
	phong_lighting += lightCol * specularStrength * pow(max(0, dot(v, r)), shininess);

	return phong_lighting;
}



vec3 compute_normal() {
	vec3 norm = vec3(0);

	// PART 2
	// Read normal from interpolated value

	norm = normalize(vtx_normal);

	/// PART 4
	/// Construct the tangent frame from normal, tangent and bitangent
	/// Sample the tangent space normal from the texture
	/// Transform the tangent space normal to world space

	vec3 bitangent = normalize(vtx_tangent.w * cross(norm, vtx_tangent.xyz));

	mat3 TBN = mat3(vtx_tangent.xyz, bitangent, norm);

	vec3 bump_norm = texture(normal, vtx_uv).xyz;
	bump_norm = normalize(2 * bump_norm - 1);

	norm = normalize(TBN * bump_norm);

	return norm;
}



vec3 compute_vertex_to_eye() {
	vec3 vertex_to_eye = vec3(0);

	/// Compute direction from fragment to camera
	vertex_to_eye = normalize(position.xyz - vtx_position);

	return vertex_to_eye;
}



void main()
{
	/// PART 3
	/// Read material properties from textures
	float shininess = (1 - texture(metallic, vtx_uv).g) * 175 + 5;
	float specularStrength = texture(roughness, vtx_uv).b;
	vec4 baseColor = texture(albedo, vtx_uv);
	vec3 emissiveColor = texture(emissive, vtx_uv).xyz;
	vec4 transparency = texture(alpha, vtx_uv);

	vec4 causticColor = .3*texture(caustic, vtx_caustic_uv)+.3*texture(caustic2, vtx_caustic_uv)+.2*texture(caustic3, vtx_caustic_uv)+.1*texture(caustic4, vtx_caustic_uv)+.05*texture(caustic5, vtx_caustic_uv);

	// Get necessary vectors
    //emissiveColor = vec3(0,0,0);
	vec3 norm = compute_normal();
	vec3 vertex_to_eye = compute_vertex_to_eye();

	// Calculate lighting
	vec3 color = /*emissiveColor +*/ lighting(baseColor, norm, vertex_to_eye, shininess, specularStrength, causticColor);

	frag_color=vec4(color,1.f);
    frag_color = mix(background_color, frag_color, visibility);
    frag_color.w = 1;
}
