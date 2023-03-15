#version 460 core

out vec4 FragColor;

struct shadow_t
{
  vec3 direction;
  
  sampler2D shadow_map;
  mat4 view;
  mat4 proj;
};

in vec2 uv_coords;

uniform sampler2D normal;  	// position buffer
uniform sampler2D position; // normal buffer

uniform int      shadows_len;   
uniform shadow_t shadows[2];


float calc_shadow(shadow_t sh, vec3 pos, vec3 normal);

// const float PI = 3.14159265359;

void main()
{	
  vec3 _normal   = texture(normal, uv_coords).xyz;
  vec3 _position = texture(position, uv_coords).xyz;
  
  float shadow = 1.0;
  for (int i = 0; i < shadows_len; ++i)
  {
    shadow = calc_shadow(shadows[i], _position, _normal);
  }

  FragColor = vec4(vec3(shadow), 1.0);
}

float calc_shadow(shadow_t sh, vec3 pos, vec3 normal)
{
  vec4 frag_pos_light_space = sh.proj * sh.view * vec4(pos, 1.0);
  // vec4 frag_pos_light_space = sh.viewproj * vec4(pos, 1.0);
  // perspective divide
  vec3 proj_coord = frag_pos_light_space.xyz / frag_pos_light_space.w;
  // from range [-1, 1] to [0, 1]
  proj_coord = (proj_coord * 0.5) + 0.5;

  // discard frags outside the projected shadow map
  if (proj_coord.z > 1.0)
  {
    return 1.0;
  }

  // float bias = max(0.05 * (1.0 - dot(normal, light.direction)), 0.01); // fighting shadow acne
  float bias = max(0.01 * (1.0 - dot(normal, sh.direction)), 0.001); // fighting shadow acne
  // float bias = max(0.01 * (-dot(normal, light.direction)), 0.001); // fighting shadow acne

  float shadow = 0.0;
  vec2 texel_size = 1.0 / textureSize(sh.shadow_map, 0);
  for(float x = -1; x <= 1; x += 0.5)
  {
    for(float y = -1; y <= 1; y += 0.5)
    {
      float pcf_depth = texture(sh.shadow_map, proj_coord.xy + vec2(x, y) * texel_size).r; 
      // proj_coord.z is the current depth
      // shadow += proj_coord.z - bias > pcf_depth ? 0.0 : 1.2;        
      shadow += proj_coord.z - bias > pcf_depth ? 0.0 : 1.0;        
    }    
  }
  shadow /= 25.0;

  return shadow;
}


