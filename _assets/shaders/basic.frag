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

uniform vec3 tint;
uniform sampler2D tex;

void main()
{
  FragColor = texture(tex, _in.uv_coords.xy) * vec4(tint.rgb, 1.0);
  // FragColor = vec4(_in.uv_coords.xy, 0.0, 1.0);
  // FragColor = vec4(_in.normal, 1.0);
}
