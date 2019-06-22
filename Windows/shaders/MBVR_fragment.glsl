#version 420																
smooth in vec3 out_Position;												
smooth in vec3 out_Normal;													
smooth in vec2 out_texcoord;	
in vec3 out_headPos;
flat in int FragDrawMode;


struct LightInfo {															
	vec4 Position; // Light position in eye coords.							
	vec3 La; // Ambient light intensity										
	vec3 Ld; // Diffuse light intensity										
	vec3 Ls; // Specular light intensity										
};																			
																			
uniform LightInfo Light = LightInfo(										
	vec4(100, 0, 0, 1),   // position							
	vec3(0.2, 0.2, 0.2),        // La										
	vec3(1.0, 1.0, 1.0),        // Ld										
	vec3(1.0, 1.0, 1.0)         // Ls										
);																			
																			
																			
// The material properties of our object									
struct MaterialInfo {														
	vec3 Ka; // Ambient reflectivity										
	vec3 Kd; // Diffuse reflectivity										
	vec3 Ks; // Specular reflectivity										
	float Shininess; // Specular shininess factor							
};																			
																			
uniform MaterialInfo Material = MaterialInfo(
	vec3(0.1, 0.1, 0.1),
	vec3(0.1, 0.1, 0.1),
	vec3(0.1, 0.1, 0.1),
	128.0						
	);																		
																			
out vec4 FragColor;

uniform sampler2D PaintTex;	

uniform sampler2D FrameBuffer;												
																			
vec4 MudColour = vec4(0.7, 0.55, 0.25, 1.0);								
																			
//void main(void)															
//{																			
//	vec3 n = normalize( out_Normal );										
//
//	vec3 s = normalize( vec3(Light.Position) - out_Position );		
//	
//	vec3 v = normalize(vec3(-out_Position));								
//																			
//	vec3 r = reflect( -s, n );												
//																			
//	vec3 lightColor = (														
//		Light.La * Material.Ka +											
//		Light.Ld * Material.Kd * max( dot(s, n), 0.0 ) +					
//		Light.Ls * Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess));
//		
//		FragColor = vec4(texture(PaintTex, out_texcoord)) + vec4(lightColor, 1.0);
//		//FragColor = vec4(lightColor, 1.0);
//}
float near = 50; 
float far  = 2000.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


int SpecularMode = 3;

void main(void)															
{
	//if(gl_FragDepth < gl_FragCoord.z)
	//{
	//	//FragColor = vec4(1,0,0,1); 
	//	FragColor =texture(FrameBuffer,vec2( gl_FragCoord.x, gl_FragCoord.y));
	//	return;
	//}
	if(FragDrawMode == 0) 
	{
		vec3 normal = normalize( out_Normal );										

		vec3 lightDir = normalize( vec3(Light.Position) - out_Position );	
		
		float lambertian = max(dot(lightDir,normal), 0.0);

		float specular = 0.0;

		if(lambertian > 0.0) 
		{

			vec3 reflectDir = reflect(-lightDir, normal);
			vec3 viewDir = normalize(-out_Position);

			float specAngle = max(dot(reflectDir, viewDir), 0.0);
			specular = pow(specAngle, 4.0);

			// the exponent controls the shininess (try mode 2)
			if(SpecularMode == 2) specular = pow(specAngle, 16.0);

			// according to the rendering equation we would need to multiply
			// with the the "lambertian", but this has little visual effect
			if(SpecularMode == 3) specular *= lambertian;

			// switch to mode 4 to turn off the specular component
			if(SpecularMode == 4) specular *= 0.0;

		}
		vec3 specColor= Material.Ks;
		vec3 TexCol = vec3(texture(PaintTex, out_texcoord));

		FragColor = vec4( lambertian* TexCol +   specular*specColor, 1.0);
		return;
	}
	else if(FragDrawMode == 1) 
	{
		vec3 TexCol = vec3(texture(PaintTex, out_texcoord));
		FragColor = vec4(TexCol, 1.0);
	}
	else
	{
		float depth = LinearizeDepth(gl_FragCoord.z) / far;
		FragColor = vec4(vec3(depth), 1.0);
		return;
	}

}	

//varying vec3 normalInterp;
//varying vec3 vertPos;
//
//uniform int mode;
//
//const vec3 lightPos = vec3(1.0,1.0,1.0);
//const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
//const vec3 specColor = vec3(1.0, 1.0, 1.0);
//
//void main() {
//
//  vec3 normal = normalize(normalInterp); 
//  vec3 lightDir = normalize(lightPos - vertPos);
//
//  float lambertian = max(dot(lightDir,normal), 0.0);
//  float specular = 0.0;
//
//  if(lambertian > 0.0) {
//
//    vec3 reflectDir = reflect(-lightDir, normal);
//    vec3 viewDir = normalize(-vertPos);
//
//    float specAngle = max(dot(reflectDir, viewDir), 0.0);
//    specular = pow(specAngle, 4.0);
//
//    // the exponent controls the shininess (try mode 2)
//    if(mode == 2)  specular = pow(specAngle, 16.0);
//
//    // according to the rendering equation we would need to multiply
//    // with the the "lambertian", but this has little visual effect
//    if(mode == 3) specular *= lambertian;
//
//    // switch to mode 4 to turn off the specular component
//    if(mode == 4) specular *= 0.0;
//
//  }
//
//  gl_FragColor = vec4( lambertian*diffuseColor +
//                        specular*specColor, 1.0);
//}