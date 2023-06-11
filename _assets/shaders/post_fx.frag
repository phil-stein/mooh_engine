#version 460 core

out vec4 FragColor;

in vec2 uv_coords;

uniform sampler2D tex;  	// color buffer
uniform sampler2D outline;	// outline buffer

uniform float exposure;


// @NOTE: doesnt work
// vec4 quantize(vec4 fragColor, vec2 fragCoord ) 
// {
//   float factor = 3.0; // [1(max)..255(min)]
//   vec2 uv = fragCoord.xy / iResolution.xy;
//   vec4 src = texture(iChannel0, uv);
//   return vec4(floor(src.rgb * factor + 0.5) / factor, src.a);
// }


// func declarations
vec3 aces_tone_mapping(vec3 col);

void main()
{	

	// tone mapping
	vec4 col_hdr = texture(tex, uv_coords).rgba;
	vec3 col = aces_tone_mapping(col_hdr.rgb);

	// gamma correction
	// float inv_gamma = 0.4545454545454545; // 1/2.2
	float gamma = 2.2;
	col = pow(col, vec3(1 / gamma));

	// ---- outline ----
	const float width = 1.0 / 800.0; 
	const vec3  outline_color = vec3(11.0f / 255.0f, 1.0, 249.0f / 255.0f);
	int use_outline_color = 0;
	float x = uv_coords.x;
	float y = uv_coords.y;
	vec2 coord = vec2(x, y);	

	// sample around the current fragment and check if outside the mesh

	float base = texture(outline, coord).r;
	x = uv_coords.x + width;
	y = uv_coords.y;
	coord = vec2(x, y);
	float c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = uv_coords.x - width;
	y = uv_coords.y;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = uv_coords.x;
	y = uv_coords.y + width;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = uv_coords.x;
	y = uv_coords.y - width;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = uv_coords.x + width;
	y = uv_coords.y + width;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }

	x = uv_coords.x - width;
	y = uv_coords.y - width;
	coord = vec2(x, y);
	c = texture(outline, coord).r;
	if (c > 0.0 && base == 0.0) { use_outline_color = 1; }


	if (use_outline_color == 1)
	{
		col = outline_color;
	}

  // col = quantize(col, uv_coords); 
  
  FragColor = vec4(col, 1.0); // col_hdr.a
  // FragColor = col_hdr;
  // FragColor = vec4(texture(tex, uv_coords).r);  // shadowmap

}

// taken from https://64.github.io/tonemapping/
vec3 aces_tone_mapping(vec3 col)
{
	col *= 0.6 * exposure;
	const float a = 2.51; 
	const float b = 0.03; 
	const float c = 2.43; 
	const float d = 0.59; 
	const float e = 0.14;
	return clamp((col * (a * col + b)) / (col * (c * col + d) +e), 0.0, 1.0); 
}
