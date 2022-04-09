#version 460 core
layout (location = 0) out vec4 _color;
layout (location = 1) out vec4 _material;
layout (location = 2) out vec4 _normal;
layout (location = 3) out vec4 _position;

in VS_OUT
{
  vec2  uv_coords;
  vec3  frag_pos;
  vec3  normal;
  mat3  TBN;
  float material;

  // tmp
  vec3 tangent;
} _in;

struct material_t
{
  sampler2D albedo;
  vec3 tint;
  
  sampler2D normal;
  
  sampler2D roughness;
  float roughness_f;
  
  sampler2D metallic;
  float metallic_f;
};

uniform material_t materials[2];


void main()
{    
  // store the fragment position vector in the first gbuffer texture
  _position = vec4(_in.frag_pos, 1.0);
  
  vec3 albedo0    = texture(materials[0].albedo, _in.uv_coords).rgb;
  vec3 normal0    = texture(materials[0].normal, _in.uv_coords).xyz;
  vec3 roughness0 = texture(materials[0].roughness, _in.uv_coords).rgb;
  vec3 metallic0  = texture(materials[0].metallic, _in.uv_coords).rgb;
  vec3 albedo1    = texture(materials[1].albedo, _in.uv_coords).rgb;
  vec3 normal1    = texture(materials[1].normal, _in.uv_coords).xyz;
  vec3 roughness1 = texture(materials[1].roughness, _in.uv_coords).rgb;
  vec3 metallic1  = texture(materials[1].metallic, _in.uv_coords).rgb;
  
  vec3 albedo    = (albedo0    * (1.0 - _in.material)) + (albedo1    * _in.material);
  vec3 normal    = (normal0    * (1.0 - _in.material)) + (normal1    * _in.material);
  vec3 roughness = (roughness0 * (1.0 - _in.material)) + (roughness1 * _in.material);
  vec3 metallic  = (metallic0  * (1.0 - _in.material)) + (metallic1  * _in.material);

  // also store the per-fragment normals into the gbuffer
  // _normal = vec4(normalize(_in.normal), 1.0);
  if (normal == vec3(1.0, 1.0, 1.0))
  {
    _normal = vec4(normalize(_in.normal), 1.0);
  }
  else
  {
    vec3 _norm;
    _norm = normal;
    _norm = normalize(_norm * 2.0 - 1.0);
    _norm = normalize(_in.TBN * _norm);
    _normal = vec4(_norm, 1.0);
  }
  // _normal = vec4(0, 1, 0, 1);  // tmp

  // store material info in material buffer
  _material.r = roughness.r * materials[0].roughness_f;
  _material.g = metallic.r * materials[0].metallic_f;
  _material.b = 0.0;
  _material.a = 1.0;

  // and the diffuse per-fragment color
  _color.rgb = albedo.rgb * materials[0].tint;
  _color.a   = 1.0;
  // _color.rgb = _in.tangent; // tmp
  // _color.rgb = _in.normal; // tmp
} 
