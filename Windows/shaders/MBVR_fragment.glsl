#version 420																
smooth in vec3 out_Position;												
smooth in vec3 out_Normal;													
smooth in vec2 out_texcoord;	
smooth in vec3 out_headPos;
flat in vec2 edgeTexCoords;

struct LightInfo {															
	vec4 Position; // Light position in eye coords.							
	vec3 La; // Ambient light intensity										
	vec3 Ld; // Diffuse light intensity										
	vec3 Ls; // Specular light intensity										
};																			
																			
uniform LightInfo Light = LightInfo(										
	vec4(-100, -100, -100, 1),   // position							
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
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	10.0																	
	);																		
																			
out vec4 FragColor;														
																			
uniform sampler2D PaintTex;												
																			
vec4 MudColour = vec4(0.7, 0.55, 0.25, 1.0);								
																			
void main(void)															
{																			
	vec3 n = normalize( out_Normal );										

	//vec3 s = normalize( out_headPos - out_Position );
			
	vec3 s = normalize( vec3(Light.Position) - out_Position );		
	
	vec3 v = normalize(vec3(-out_Position));								
																			
	vec3 r = reflect( -s, n );												
																			
	vec3 lightColor = (														
		Light.La * Material.Ka +											
		Light.Ld * Material.Kd * max( dot(s, n), 0.0 ) +					
		Light.Ls * Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess));
																			
																			
	//FragColor  = vec4(lightColor, 1.0);

	if(edgeTexCoords.x + 0.05 > out_texcoord.x || out_texcoord.x > edgeTexCoords.x - 0.05 ||
	   edgeTexCoords.y + 0.05 > out_texcoord.y || out_texcoord.y > edgeTexCoords.y - 0.05)
	{
		FragColor = vec4 (0,0,0,1);
	}
	else
	{
		FragColor = vec4(texture(PaintTex, out_texcoord));// * vec4(lightColor, 1.0);
	}
	//FragColor  = MudColour;
}																			
