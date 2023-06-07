#version 460 core

	out vec4 FragColor;

  //passed from vertex-shader
  in VS_OUT
  {
    vec2 tex_coords;
    vec3 frag_pos;
  } _in;
	
	uniform sampler2D tex;
	uniform vec3      tint;
	
	void main()
	{
    vec2 coords = _in.tex_coords;
    // coords.x = 1 - coords.x;
    coords.y = 1 - coords.y;
    FragColor = vec4(texture(tex, coords).x) * vec4(tint, 1.0);
	  // FragColor = vec4(1, 1, 0, 1);
  }
