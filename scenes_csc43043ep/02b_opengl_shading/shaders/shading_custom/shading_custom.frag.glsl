#version 330 core


// Inputs coming from the vertex shader
in struct fragment_data
{
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;

// View matrix
uniform mat4 view;

struct material_structure
{
	vec3 color;  // Uniform color of the object
};
uniform material_structure material;

// Ambiant uniform controled from the GUI
uniform float ambiant;
uniform float diffuse;
uniform float specular;
uniform float shininess;
uniform float visibility;
uniform float light_range;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 fog_color;
uniform bool has_fog;
uniform bool has_attenuation;


float dot_plus(vec3 a, vec3 b) {
    return max(dot(normalize(a), normalize(b)), 0.0);
}

void main()
{
	vec3 current_color;

    mat3 O = transpose(mat3(view)); // get the orientation matrix
    vec3 last_col = vec3(view * vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
    vec3 camera_position = -O * last_col;

	vec3 phong_color = ((ambiant + diffuse * dot_plus(fragment.normal, light_position - fragment.position)) * material.color
                       + specular * pow(
                        dot_plus(reflect(fragment.position - light_position, fragment.normal), camera_position - fragment.position), shininess
                        )) * light_color;
    
    float fog_attenuation = has_fog ? min(length(camera_position - fragment.position)/visibility, 1) : 0.0;
    float dist_attenuation = has_attenuation ? min(length(light_position - fragment.position)/light_range, 1) : 0.0;

    current_color = (1 - fog_attenuation) * (1 - dist_attenuation) * phong_color + fog_attenuation * fog_color;
	FragColor = vec4(current_color, 1.0); 	// Note: the last alpha component is not used here

}