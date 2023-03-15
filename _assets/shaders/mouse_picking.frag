#version 460 core

out vec4 FragColor;

//passed from vertex-shader
in VS_OUT
{
  vec2 uv_coords;
  vec3 frag_pos;
  vec3 normal;
  mat3 TBN;
} _in;

uniform float id;

void main()
{
  FragColor = vec4(id +1, 1.0, 1.0, 1.0);
}
