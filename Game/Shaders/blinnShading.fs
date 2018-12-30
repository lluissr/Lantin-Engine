#version 400 core

out vec4 color;

uniform mat4 view;

uniform int useDiffuseMap;
uniform sampler2D diffuseMap;

uniform int useEmissiveMap;
uniform sampler2D emissiveMap;

uniform int useOcclusionMap;
uniform sampler2D occlusionMap;

uniform int useSpecularMap;
uniform sampler2D specularMap;

uniform vec4 diffuseColor;
uniform vec4 emissiveColor;
uniform vec4 specularColor;

uniform vec3  light_pos;
uniform float ambient;
uniform float shininess;
uniform float k_ambient;
uniform float k_diffuse;
uniform float k_specular;

in vec3 position;
in vec3 normal; 
in vec2 uv0;

void main()
{
    /*Ambient */
	vec3 finalAmbient = k_ambient * ambient * vec3(texture(occlusionMap, uv0)); 
	
	
    /*Diffuse */
	vec3 normal = normalize(normal);
    vec3 lightDir = normalize(light_pos - position);
    float diffuse = max(0.0, dot(normal, lightDir));
	vec3 finalDiffuse = k_diffuse * diffuse * diffuseColor.rgb * vec3(texture(diffuseMap, uv0));
	
	
    /*Specular */
    vec3 viewPos = transpose(mat3(view))*(-view[3].xyz);
	vec3 viewDir = normalize(viewPos - position);
	vec3 half_dir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, half_dir), 0.0), shininess);
	vec4 textureSpecularColor = texture(specularMap, uv0);
	vec4 sp = vec4(textureSpecularColor.rgb*specularColor.rgb, max(textureSpecularColor.a*shininess*128.0f, 8.0f));
	vec3 finalSpecular = sp.rgb * k_specular * spec * specularColor.rgb;
    	

	/*Emission*/
    vec3 finalEmission = emissiveColor.rgb * texture(emissiveMap, uv0).rgb;
	
    color = vec4(finalAmbient + finalDiffuse + finalSpecular + finalEmission, 1.0f);
}
