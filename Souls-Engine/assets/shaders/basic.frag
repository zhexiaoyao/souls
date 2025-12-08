#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec3 FragPos;
in vec3 Normal;

uniform bool useOverrideColor;
uniform vec3 overrideColor;

// 材质属性
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// 光源属性（简化版，使用固定光源）
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    if (useOverrideColor) {
        FragColor = vec4(overrideColor, 1.0);
    } else {
        // 环境光
        vec3 ambient = material.ambient * lightColor;
        
        // 漫反射
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = material.diffuse * diff * lightColor;
        
        // 镜面反射
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = material.specular * spec * lightColor;
        
        // 最终颜色 = 环境光 + 漫反射 + 镜面反射
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
}

