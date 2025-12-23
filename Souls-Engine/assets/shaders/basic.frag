#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec3 FragPos;
in vec3 Normal;

uniform bool useOverrideColor;
uniform vec3 overrideColor;

// 材质属�?
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float alpha;
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
        // 环境�?
        vec3 ambient = material.ambient * lightColor;
        
        // 漫反�?
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = material.diffuse * diff * lightColor;
        
        // 镜面反射
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = material.specular * spec * lightColor;
        
        // 最终颜色 = 环境 + 漫反射 + 镜面反射
        vec3 result = ambient + diffuse + specular;
        // 将顶点颜色作为基色（用于棋盘格等顶点着色物体），
        // 这样黑白格子将不会被光照抹成纯灰色
        result *= Color;
        FragColor = vec4(result, material.alpha);
    }
}

