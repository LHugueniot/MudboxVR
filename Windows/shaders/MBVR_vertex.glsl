#version 420 core

uniform mat4 MVP;
uniform mat4 ModelMat;
uniform mat3 InvModelMat;
uniform vec3 headPos;

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoords;

flat out vec2 edgeTexCoords;
smooth out vec2 out_texcoord;
smooth out vec3 out_Position;
smooth out vec3 out_Normal;
smooth out vec3 out_headPos;

void main()
{
	out_headPos = headPos;

	//out_Normal = InvModelMat * normalize(in_Normal);
	out_Normal = in_Normal;

	out_Position = vec3(ModelMat * vec4(in_Position, 1.0));

	edgeTexCoords= in_TexCoords;
	out_texcoord = in_TexCoords;
	gl_Position = MVP * vec4(in_Position, 1.0);
}