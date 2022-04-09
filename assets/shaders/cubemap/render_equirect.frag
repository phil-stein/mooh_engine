#version 460 core
out vec4 FragColor;
in vec3 local_pos;

uniform sampler2D equirect_map;

const vec2 inv_atan = vec2(0.1591, 0.3183);

vec2 sample_spherical_map(vec3 v)
{
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= inv_atan;
  uv += 0.5;
  return uv;
}

void main()
{   
  vec2 uv = sample_spherical_map(normalize(local_pos)); // make sure to normalize localPos
  vec3 color = texture(equirect_map, uv).rgb;

  FragColor = vec4(color, 1.0);
}
