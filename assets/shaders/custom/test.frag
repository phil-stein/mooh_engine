#version 460 core
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 material;
layout (location = 2) out vec4 normal;
layout (location = 3) out vec4 position;

in VS_OUT
{
  vec2 uv_coords;
  vec3 frag_pos;
  vec3 normal;
  mat3 TBN;
} _in;

uniform sampler2D albedo;
uniform vec3 tint;

uniform sampler2D norm;

uniform sampler2D roughness;
uniform float roughness_f;

uniform sampler2D metallic;
uniform float metallic_f;

uniform vec3 fade_color;

void main()
{    
  // store the fragment position vector in the first gbuffer texture
  position = vec4(_in.frag_pos, 1.0);
  
  // also store the per-fragment normals into the gbuffer
  if (texture(norm, _in.uv_coords).rgb == vec3(1.0, 1.0, 1.0))
  {
    normal = vec4(normalize(_in.normal), 1.0);
  }
  else
  {
    vec3 _normal;
    _normal = texture(norm, _in.uv_coords).rgb;
    _normal = normalize(_normal * 2.0 - 1.0);
    _normal = normalize(_in.TBN * _normal);
    normal = vec4(_normal, 1.0);
  }

  float mask = max(1 - _in.frag_pos.y, 0);
  
  // store material info in material buffer
  material.r = texture(roughness, _in.uv_coords).r * roughness_f;
  material.g = texture(metallic,  _in.uv_coords).r * metallic_f * (1 - mask);
  material.b = 0.0;
  material.a = 1.0;

  // and the diffuse per-fragment color
  color.rgb = texture(albedo, _in.uv_coords).rgb * tint;
  color.a   = 1.0;
  color.rgb += fade_color * mask;
  
  // honstely no idea what this is supposed to do
  color.rgb = fade_color;


} 
