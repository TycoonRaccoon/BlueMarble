#version 460 core

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec3 InColor;
layout (location = 3) in vec2 InUV;

uniform mat4 Normal;
uniform mat4 MVP;

out vec3 NormalM;
out vec3 Color;
out vec2 UV;

void main() {
	NormalM = vec3(Normal * vec4(InNormal, 0.0));
	Color = InColor;
	UV = InUV;
	gl_Position = MVP * vec4(InPosition, 1.0);
}
