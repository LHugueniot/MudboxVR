============================================MudBoxVR Readme===============================================

TODO:

-mbvrmESH solve dirty vertex 
-Solve depth testing issue
-Figure out whats hapening to the normals

BUGS:

-Compositor crashing sometimes, usually after steam VR is closed, potential solution:
https://community.viveport.com/t5/Technical-Support/Error-400-Compositor-not-available-Symptoms-and-Solutions/td-p/279

EXAMPLE CODE:

			"#version 330 core																	  "
			"uniform sampler2D texture1;														  "
			"in vec2 f_TexCoord0;																  "
			"in vec3 viewPos;																	  "
			"layout(location = 0) out vec4 fragColor;											  "
			"																					  "
			"uniform vec4 ambientMaterial2, diffuseMaterial2, specularMaterial2;				  "
			"uniform vec4 ambientLight, diffuseLight, specularLight;							  "
			"uniform mat4 viewMat;																  "
			"uniform vec4 lightPosition;														  "
			"uniform float shininess;															  "
			"																					  "
			"void main(void)																	  "
			"{																					  "
			"	vec4 ambientProduct = ambientLight * ambientMaterial2;							  "
			"	vec4 diffuseProduct = diffuseLight * diffuseMaterial2;							  "
			"	vec4 specularProduct = specularLight * specularMaterial2;						  "
			"																					  "
			"	vec3 pos = viewPos;																  "
			"	vec3 nv = cross(dFdx(pos), dFdy(pos));											  "
			"	nv = nv * sign(nv.z);															  "
			"	vec3 L = normalize((viewMat*lightPosition).xyz - pos);							  "
			"	vec3 E = normalize(-pos);														  "
			"	vec3 H = normalize(L + E);														  "
			"	vec3 N = normalize(nv);															  "
			"																					  "
			"	float Kd = max(dot(L, N), 0.0);													  "
			"	vec4 diffuse = Kd * diffuseProduct;												  "
			"																					  "
			"	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);										  "
			"	if (Kd > 0.0f)																	  "
			"	{																				  "
			"		float Ks = pow(max(dot(H, N), 1.0), shininess);								  "
			"		specular = Ks * specularProduct;											  "
			"	}																				  "
			"	vec4 fragColor_lightning = (ambientProduct + diffuse + specular);				  "
			"	vec4 fragColor_texture = texture2D(texture1, vec2(f_TexCoord0.x, f_TexCoord0.y)); "
			"	fragColor = fragColor_lightning + fragColor_texture;							  "
			"};