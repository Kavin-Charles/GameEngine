#version 460 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int o_EntityID;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 u_ObjectColor;
uniform sampler2D u_AlbedoMap;
uniform bool u_HasAlbedoMap;

uniform int u_EntityID;
uniform bool u_Selected;

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(norm, lightDir), 0.2);
    
    vec3 baseColor = u_HasAlbedoMap ? texture(u_AlbedoMap, v_TexCoord).rgb : u_ObjectColor;
    vec3 result = diff * baseColor;
    
    if (u_Selected) {
        result += vec3(0.3, 0.3, 0.3); // Additive highlight
    }
    
    FragColor = vec4(result, 1.0);
    o_EntityID = u_EntityID;
}
