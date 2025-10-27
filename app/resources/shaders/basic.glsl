//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 dirLightDir;
uniform vec3 dirLightAmbient;
uniform vec3 dirLightDiffuse;
uniform vec3 dirLightSpecular;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform SpotLight spotLight;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;


void main(){
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    //Direkciono
    vec3 lightDir = normalize(-dirLightDir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambientD = dirLightAmbient * color;
    vec3 diffuseD = dirLightDiffuse * diff * color;
    vec3 specularD = dirLightSpecular * spec;

    vec3 result = ambientD + diffuseD + specularD;

    //Spotlight
    vec3 lightDirSpot = normalize(spotLight.position - FragPos);
    float theta = dot(lightDirSpot, normalize(-spotLight.direction));

    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

    float distance = length(spotLight.position - FragPos);
    float attenuation = 1.0 / (spotLight.constant +
    spotLight.linear * distance +
    spotLight.quadratic * (distance * distance));

    vec3 ambientS  = spotLight.ambient * color;
    float diffS    = max(dot(norm, lightDirSpot), 0.0);
    vec3 diffuseS  = spotLight.diffuse * diffS * color;

    vec3 reflectDirS = reflect(-lightDirSpot, norm);
    float specS = pow(max(dot(viewDir, reflectDirS), 0.0), 32);
    vec3 specularS = spotLight.specular * specS;

    ambientS  *= attenuation;
    diffuseS  *= intensity * attenuation;
    specularS *= intensity * attenuation;

    result += ambientS + diffuseS + specularS;

    FragColor = vec4(result, 1.0);
}