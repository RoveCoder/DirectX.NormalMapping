#include "Header.hlsli"

float4 CalculateDirectionalLighting(float3 position, float3 normal)
{
	// Diffuse lighting
	float3 lightVec = -mDirectionalLight.mDirection.xyz;
	float4 diffuse_light = saturate(dot(lightVec, normal)) * mDirectionalLight.mDiffuse * mMaterial.mDiffuse;

	// Ambient lighting
	float4 ambient_light = mDirectionalLight.mAmbient * mMaterial.mAmbient;

	// Specular lighting
	float3 viewDir = normalize(mDirectionalLight.mCameraPos - position);
	float3 reflectDir = reflect(-lightVec, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mDirectionalLight.mSpecular.w * mMaterial.mSpecular.w);
	float4 specular_light = float4(spec * mDirectionalLight.mSpecular.xyz * mMaterial.mSpecular.xyz, 1.0f);

	// Combine all 3 lights
	return diffuse_light + ambient_light + specular_light;
}

float4 CalculatePointLighting(float3 position, float3 normal)
{
	// Diffuse lighting
	float3 lightVec = normalize((float3)mPointLight.mLightPos - position);
	float4 diffuse_light = saturate(dot(lightVec, normal)) * mPointLight.mDiffuse * mMaterial.mDiffuse;

	// Ambient lighting
	float4 ambient_light = mPointLight.mAmbient * mMaterial.mAmbient;

	// Specular lighting
	float3 viewDir = normalize(mPointLight.mCameraPos - position);
	float3 reflectDir = reflect(-lightVec, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mDirectionalLight.mSpecular.w * mMaterial.mSpecular.w);
	float4 specular_light = float4(spec * mPointLight.mSpecular.xyz * mMaterial.mSpecular.xyz, 1.0f);

	return diffuse_light + ambient_light + specular_light;
}

float4 main(PixelInput input) : SV_TARGET
{
	input.Normal = normalize(input.Normal);

	// Sample Texture
	float4 diffuse_texture = TextureDiffuse.Sample(SamplerAnisotropic, input.Texture);

	// Normal Texture
	float3 normalMapSample = TextureNormal.Sample(SamplerAnisotropic, input.Texture).rgb;

	// Uncompress each component from [0,1] to [-1,1].
	float3 normalT = 2.0f * normalMapSample - 1.0f;

	// Build orthonormal basis.
	float3 N = input.Normal;
	float3 T = normalize(input.Tangent - dot(input.Tangent, N) * N);
	float3 B = cross(N, T);
	 
	float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);

	input.Normal = bumpedNormalW;

	// Directional Light
	float4 directional_light = CalculateDirectionalLighting(input.Position, input.Normal);

	// Point Light
	float4 point_light = CalculatePointLighting(input.Position, input.Normal);

	// Combine all pixels
	float4 finalColour = (directional_light + point_light) * diffuse_texture;
	return finalColour;
}