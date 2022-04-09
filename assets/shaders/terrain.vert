#version 460 core

layout (location = 0) in vec3   aPosition;   
layout (location = 1) in vec3   aNormal;   
layout (location = 2) in vec2   aTexCoords;   
layout (location = 3) in vec3   aTangent;
layout (location = 4) in float  aMaterial;


out VS_OUT
{
  vec2  uv_coords;
  vec3  frag_pos;
  vec3  normal;
  mat3  TBN;
  float material;
  // tmp
  vec3 tangent;
} _out;

uniform mat4 proj;          // matrix for camera projection
uniform mat4 view;          // matrix for view transformation
uniform mat4 model;         // matrix for applying the objects transform

void main() 
{
  _out.frag_pos   = vec3(model * vec4(aPosition.xyz, 1.0));
  _out.uv_coords  = aTexCoords;
  // _out.normal     = aNormal;
  _out.normal     = mat3(transpose(inverse(model))) * aNormal;
  
  // tmp
  _out.tangent    = aTangent;

  // TBN matrix for normal mapping
  vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
  vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  _out.TBN = mat3(T, B, N);

  _out.material = aMaterial;

  // the MVP matrix model view projection
  // don't change the order in the multiplication!
  // matrices are multiplied in reverse
  gl_Position =  proj * view * model * vec4(aPosition.xyz, 1.0);
}
