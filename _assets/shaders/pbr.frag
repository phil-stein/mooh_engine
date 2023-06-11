#version 460 core

out vec4 FragColor;

// lights
struct point_light_t
{
  vec3  pos;
  vec3  color;
  float intensity;
};

struct dir_light_t
{
  vec3 direction;
  vec3 color;

  int shadow_idx;

};

in vec2 uv_coords;

uniform sampler2D color;  	// color buffer
uniform sampler2D material; // material buffer
uniform sampler2D normal;  	// position buffer
uniform sampler2D position; // normal buffer
uniform sampler2D shadow;   // shadow buffer

uniform vec3 view_pos;

uniform samplerCube irradiance_map;
uniform samplerCube prefilter_map;
uniform sampler2D   brdf_lut;
uniform float       cube_map_intensity; 

uniform int point_lights_len;
uniform point_light_t point_lights[8];

uniform int dir_lights_len;
uniform dir_light_t dir_lights[2];


float distribution_ggx(vec3 N, vec3 H, float _roughness);
float geometry_schlick_ggx(float NdotV, float _roughness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float _roughness);
vec3  fresnel_schlick(float cos_theta, vec3 F0);
vec3  fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness);

vec3  calc_directional_light(dir_light_t light, vec3 albedo, vec3 position, vec3 norm, float roughness, float metallic, vec3 F0);

const float PI = 3.14159265359;

void main()
{	
  float roughness = texture(material, uv_coords).r;
  float metallic  = texture(material, uv_coords).g;
  float ao = 0.0;

  vec3 albedo    = texture(color, uv_coords).rgb;
  vec3 _normal   = texture(normal, uv_coords).xyz;
  vec3 _position = texture(position, uv_coords).xyz;
  vec3 N = normalize(_normal);
  vec3 V = normalize(view_pos - _position);
  vec3 R = reflect(-V, N); 

  vec3 F0 = vec3(0.04); 
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);
  for (int i = 0; i < point_lights_len; ++i)
  {
    // calculate per-light radiance
    vec3  L = normalize(point_lights[i].pos - _position);
    vec3  H = normalize(V + L);
    float distance    = length(point_lights[i].pos - _position);
    float attenuation = point_lights[i].intensity / (distance * distance);
    vec3  radiance   = point_lights[i].color * attenuation;        

    // cook-torrance brdf
    float NDF = distribution_ggx(N, H, roughness);        
    float G   = geometry_smith(N, V, L, roughness);      
    vec3  F   = fresnel_schlick(max(dot(H, V), 0.0), F0);       

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  

    vec3  numerator   = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3  specular    = numerator / denominator;  

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
  } 
  for (int i = 0; i < dir_lights_len; ++i)
  {
    Lo += calc_directional_light(dir_lights[i], albedo, _position, _normal, roughness, metallic, F0); 
  }
  Lo *= texture(shadow, uv_coords).r;

  // ambient ---------------------------------------------
  // ambient lighting (we now use IBL as the ambient term)
  vec3 F = fresnel_schlick_roughness(max(dot(N, V), 0.0), F0, roughness);

  vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;    

  vec3 irradiance = texture(irradiance_map, -N).rgb;  // -N to flip
  vec3 diffuse      = irradiance * albedo;

  // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefiltered_color = textureLod(prefilter_map, -R,  roughness * MAX_REFLECTION_LOD).rgb; // -R to flip reflection 
  vec2 brdf  = texture(brdf_lut, vec2(max(dot(N, V), 0.0), roughness)).rg;
  vec3 specular = prefiltered_color * (F * brdf.x + brdf.y);

  vec3 ambient = (kD * diffuse + specular) * 0.3 * cube_map_intensity; //  * ao;

  vec3 col = ambient + Lo;

  if (texture(material, uv_coords).b >= 1.0)
  {
    FragColor = vec4(albedo, 1.0);
  }
  else
  {
    // experiment
    // col *= 20;
    // col = round(col);
    // col *= 0.05;
    
    FragColor = vec4(col, 1.0);
    // FragColor = vec4(albedo, 1.0);
  }
}


vec3 calc_directional_light(dir_light_t light, vec3 albedo, vec3 position, vec3 norm, float roughness, float metallic, vec3 F0)
{
  vec3 L, V, H, Ir;
  float NoL, NoV, NoH, LoH;

  L = light.direction;
  vec3 radiance = light.color;

  // test if light hitting at below 90 degree angle
  NoL = dot(norm, L);
  if (NoL <= 0.0) { return vec3(0.0); }

  V = normalize(view_pos - position); // maybe view_dir
  H = normalize(V + L);

  // cook-torrance brdf
  float NDF = distribution_ggx(norm, H, roughness);        
  float G   = geometry_smith(norm, V, L, roughness);      
  vec3  F   = fresnel_schlick(max(dot(H, V), 0.0), F0);       

  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic;	  

  vec3  numerator   = NDF * G * F;
  float denominator = 4.0 * max(dot(norm, V), 0.0) * max(dot(norm, L), 0.0) + 0.0001;
  vec3  specular    = numerator / denominator;  

  // add to outgoing radiance Lo
  float NdotL = max(dot(norm, L), 0.0);                
  return (kD * albedo / PI + specular) * radiance * NdotL;  
}

vec3 fresnel_schlick(float cos_theta, vec3 F0)
{
  return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}
vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness)
{
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

float distribution_ggx(vec3 N, vec3 H, float _roughness)
{
  float a      = _roughness * _roughness;
  float a2     = a * a;
  float NdotH  = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float num   = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return num / denom;
}

float geometry_schlick_ggx(float NdotV, float _roughness)
{
  float r = (_roughness + 1.0);
  float k = (r * r) / 8.0;

  float num   = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return num / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float _roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2  = geometry_schlick_ggx(NdotV, _roughness);
  float ggx1  = geometry_schlick_ggx(NdotL, _roughness);

  return ggx1 * ggx2;
}

