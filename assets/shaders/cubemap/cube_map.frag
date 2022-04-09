#version 460 core
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 material;
layout (location = 2) out vec4 normal;
layout (location = 3) out vec4 position;


out vec4 FragColor;
	
in vec3 dir;
uniform samplerCube cube_map;

void main()
{
	// FragColor = texture(cube_map, dir);
  color = texture(cube_map, dir);
  // color = textureLod(cube_map, dir, 1.0);
  color.a = 1.0;

  material.r = 0.0;
  material.g = 0.0;
  material.b = 1.0;
  material.a = 1.0;
  
  normal   = vec4(dir, 1.0);
  position = vec4(dir, 1.0);
 
  // @TODO: render cubemap after lighting using the old depth buffer or something

}
