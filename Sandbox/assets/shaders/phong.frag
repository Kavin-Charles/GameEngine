#version 460 core
out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 u_ObjectColor;
uniform sampler2D u_AlbedoMap;
uniform bool u_HasAlbedoMap;

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(norm, lightDir), 0.2);
    
    vec3 baseColor = u_HasAlbedoMap ? texture(u_AlbedoMap, v_TexCoord).rgb : u_ObjectColor;
    vec3 result = diff * baseColor;
    
    FragColor = vec4(result, 1.0);
}
