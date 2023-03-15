#version 460 core

struct directional_light 
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    int use_shadow;
    sampler2D shadow_map;
    mat4 view;
    mat4 proj;
};

out vec4 FragColor;

in vec2 uv_coords;
in vec3 view_pos;

uniform sampler2D color;  	// color buffer
uniform sampler2D normal;  	// position buffer
uniform sampler2D position; // normal buffer

uniform int num_dir_lights;
uniform directional_light dir_lights[6];

float calc_shadow(directional_light light, vec3 normal);
vec3 calc_directional_light(directional_light light, vec3 col, vec3 norm, vec3 view_dir, float shadow);

void main()
{	
  vec3 result = vec3(0.0); // vec4(texture(color, uv_coords).rgb, 1.0);

  //get the angle between the reflected light-ray and the view-direction        
  vec3 view_dir = normalize(view_pos - texture(position, uv_coords).xyz);

  vec3 _normal = texture(normal, uv_coords).xyz;
  vec3 _color  = texture(color, uv_coords).rgb;

  for(int i = 0; i < num_dir_lights; i++)
  {
    float shadow = 1.0;
    if (dir_lights[i].use_shadow == 1)
    {
      shadow = calc_shadow(dir_lights[i], _normal);
    }
    // @BUGG: this is hiding the problem that the edges of objects get shadows as well
    // shadow = shadow <= 1.5 ? 1 : shadow;
    // shadow = shadow <= 0 ? 1 : shadow;
    shadow += 0.8;  
    result += calc_directional_light(dir_lights[i], _color, _normal, view_dir, shadow);
  }

  FragColor = vec4(result, 1.0);	// alpha
  // FragColor = vec4(vec3(gl_FragCoord.z), 1.0); // depth-buffer

}

float calc_shadow(directional_light light, vec3 normal)
{
  vec4 frag_pos_light_space = light.proj * light.view * vec4(texture(position, uv_coords).xyz, 1.0);
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
  float bias = max(0.01 * (1.0 - dot(normal, light.direction)), 0.001); // fighting shadow acne
  // float bias = max(0.01 * (-dot(normal, light.direction)), 0.001); // fighting shadow acne

  float shadow = 0.0;
  vec2 texel_size = 1.0 / textureSize(light.shadow_map, 0);
  for(float x = -1; x <= 1; x += 0.5)
  {
    for(float y = -1; y <= 1; y += 0.5)
    {
      float pcf_depth = texture(light.shadow_map, proj_coord.xy + vec2(x, y) * texel_size).r; 
      // proj_coord.z is the current depth
      shadow += proj_coord.z - bias > pcf_depth ? 0.0 : 1.2;        
    }    
  }
  shadow /= 25.0;

  return shadow;
}

vec3 calc_directional_light(directional_light light, vec3 col, vec3 norm, vec3 view_dir, float shadow)
{
    //explanaition: https://learnopengl.com/Lighting/Light-casters, LearnOpenGL page 137

    //diffuse----------------------------------
    // vec3 lightDir = normalize(light.position - FragPos);
    vec3 light_dir = normalize(-light.direction);

    //dot product between surface-normal and light-dir, the clamped to get a value between 0-1, would otherwise be neg. if the angle was greater than 90° 
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * col;

    //ambient----------------------------------
    vec3 ambient = light.ambient * col; //light.ambient * material.ambient;

    //the shininess-value dictates how focused the spot of reflected light is
    // vec3 halfwayDir = normalize(lightDir + viewDir);
    // float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess * 128);
    // vec3 specular = light.specular * spec * vec3(texture(mat.specular, texCoords));

    return ambient + (diffuse * shadow);

}

