R"(
// Small structures that should be defined before any backend logic
// Avoid arrays and sub-vec4 members because of std140 padding constraints
struct sampler_info
{
	float scale_x, scale_y, scale_z;  // 12
	float bias_x, bias_y, bias_z;     // 24
	float clamp_min_x, clamp_min_y;   // 32
	float clamp_max_x, clamp_max_y;   // 40
	uint remap;                       // 44
	uint flags;                       // 48
};

layout(push_constant) uniform CCC0
{
	float depth_min_bounds;
	float depth_max_bounds;
	float pad0;
	float pad1;
};
)"
