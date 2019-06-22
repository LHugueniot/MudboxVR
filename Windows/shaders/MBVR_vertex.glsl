#version 420 core

uniform mat4 MVP;
uniform mat4 ModelMat;
uniform mat4 InvModelMat;
uniform vec3 headPos;
uniform int DrawMode;

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoords;

smooth out vec2 out_texcoord;
smooth out vec3 out_Position;
smooth out vec3 out_Normal;

out vec3 out_headPos;
flat out int FragDrawMode;

void main()
{
	FragDrawMode= DrawMode;
	out_headPos = headPos;
	
	out_Normal=normalize( transpose( inverse(mat3( InvModelMat )  ) )*in_Normal);

	//out_Normal = vec3(normalize(ModelMat * vec4(,0)));
	//out_Normal = in_Normal;
	vec4 vertPos4 =ModelMat * vec4(in_Position, 1.0);
	out_Position = vec3(vertPos4) / vertPos4.w;

	out_texcoord = in_TexCoords;
	gl_Position = MVP * vec4(in_Position, 1.0);
}





//attribute vec3 inputPosition;
//attribute vec2 inputTexCoord;
//attribute vec3 inputNormal;
//
//uniform mat4 projection, modelview, normalMat;
//
//varying vec3 normalInterp;
//varying vec3 vertPos;
//
//void main(){
//    gl_Position = projection * modelview * vec4(inputPosition, 1.0);
//    vec4 vertPos4 = modelview * vec4(inputPosition, 1.0);
//    vertPos = vec3(vertPos4) / vertPos4.w;
//    normalInterp = vec3(normalMat * vec4(inputNormal, 0.0));
//}