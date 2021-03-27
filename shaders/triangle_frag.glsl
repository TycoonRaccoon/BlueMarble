#version 460 core

uniform sampler2D EarthTextureSampler;
uniform sampler2D CloudTextureSampler;

uniform float Time;

uniform vec2 EarthSpeed = vec2(0.008, 0.0);
uniform vec2 CloudSpeed = vec2(0.017, 0.0);

uniform vec3 LightDirection;
uniform float LightIntensity;

in vec3 NormalM;
in vec3 Color;
in vec2 UV;

out vec4 OutColor;

void main() {
	vec3 N = normalize(NormalM);
	vec3 L = -normalize(LightDirection);

	float Lambertian = max(dot(N, L), 0.0);

	vec3 ViewDirection = vec3(0.0, 0.0, -1.0);
	vec3 V = -ViewDirection;
	vec3 R = reflect(-L, N);

	float Alpha = 100.0;
	float Specular = pow(dot(R, V), Alpha);
	Specular = max(Specular, 0.0);

	vec3 EarthColor = texture(EarthTextureSampler, UV + Time * EarthSpeed).rgb;
	vec3 CloudColor = texture(CloudTextureSampler, UV + Time * CloudSpeed).rgb;

	OutColor = vec4((EarthColor + CloudColor) * LightIntensity * Lambertian + Specular, 1.0);
}
