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
uniform sampler2D emissive;
uniform sampler2D normal;
uniform sampler2D spectral;

uniform sampler2D caustic;
uniform sampler2D caustic2;
uniform sampler2D caustic3;
uniform sampler2D caustic4;
uniform sampler2D caustic5;

// Input attributes, written in the vertex shader and interpolated
in vec3 vtx_normal;
in vec3 vtx_position;
in vec2 vtx_uv;
in vec2 vtx_normalized_uv;
in vec2 vtx_caustic_uv;
in vec4 vtx_tangent;
in float visibility;
in vec4 background_color;

// Output attributes, final color of the fragment
out vec4 frag_color;

vec3 lamb(int i, vec4 baseColor, vec3 n, vec3 v, vec4 caustic) {
	vec3 lightDir =  normalize(-lt[i].dir.xyz);
	vec3 lightCol = lt[i].dif.rgb * vec3(caustic.r, caustic.g + .05, caustic.b + .1);
	vec3 ambient_color = amb.rgb;
	float diff = max(dot(n, lightDir), 0.0);
	vec3 ambient = ambient_color;
	vec3 lamb_lighting = ambient + 0.5 * diff * lightCol* baseColor.rgb;
	return lamb_lighting;
}

vec3 lighting(vec4 baseColor, vec3 n, vec3 v, vec4 caustic) {
	vec3 color = vec3(0);
	for(int i=0;i<lt_att[0];i++) {
		color+=lamb(i,baseColor,n,v, caustic);
	}
	return color;
}


vec3 compute_normal() {
    vec3 norm = vec3(0);

    norm = normalize(vtx_normal);

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

void main()
{
    float shininess = (1 - texture(spectral, vtx_uv).g) * 175 + 5;
    float specularStrength = texture(spectral, vtx_uv).b;
    vec4 baseColor = texture(tex, vtx_uv);
    vec3 emissiveColor = texture(emissive, vtx_uv).xyz;

		vec4 causticColor = .35*texture(caustic, vtx_caustic_uv)+.3*texture(caustic2, vtx_caustic_uv)+.2*texture(caustic3, vtx_caustic_uv)+.1*texture(caustic4, vtx_caustic_uv)+.05*texture(caustic5, vtx_caustic_uv);
		vec4 fake = vec4(1.f, 1.f, 1.f, 1.f);
    vec3 norm = compute_normal();
		vec3 vertex_to_eye = normalize(position.xyz - vtx_position);
		vec3 color = length(emissiveColor)*vec3(.7, .4, 0) + lighting(baseColor, norm, vertex_to_eye, fake);
		//color += causticColor.rgb;
		frag_color=vec4(color.rgb,1.f);

    // Implement fog as mix of background water color and visibility
    frag_color = mix(background_color, frag_color, visibility);
    frag_color.w = 1;
}
