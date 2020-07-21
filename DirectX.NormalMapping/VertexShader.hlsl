#include "Header.hlsli"

PixelInput main(VertexInput input)
{
	PixelInput output;

	// Transform to homogeneous clip space.
	output.PositionH = mul(float4(input.Position, 1.0f), World);
	output.PositionH = mul(output.PositionH, View);
	output.PositionH = mul(output.PositionH, Projection);

	// Transform to world space.
	output.Position = mul(float4(input.Position, 1.0f), World).xyz;

	// Copy UVs
	output.Texture = input.Texture;

	// Transform normals by inverse world
	output.Normal = mul(input.Normal, (float3x3)InverseWorld).xyz;

	return output;
}