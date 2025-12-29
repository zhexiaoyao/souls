#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;  // 光源空间的片段位置

uniform bool useOverrideColor;
uniform vec3 overrideColor;

// 材质属性（符合标准Phong光照模型）
struct Material {
    vec3 ambient;      // k_a: 环境光反射系数 (0-1)
    vec3 diffuse;      // k_d: 漫反射系数 (0-1)
    vec3 specular;     // k_s: 镜面反射系数 (0-1)
    float shininess;   // n: 镜面反射指数（控制光泽度）
};
uniform Material material;

// 光源结构
struct Light {
    vec3 position;     // 光源位置
    vec3 color;        // I_p: 光源颜色和强度
    float intensity;   // 光源强度倍数
    // 距离衰减参数: attenuation = 1.0 / (constant + linear * distance + quadratic * distance^2)
    float constant;
    float linear;
    float quadratic;
};

// 支持最多8个光源（符合OpenGL标准）
#define MAX_LIGHTS 8
uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform vec3 viewPos;           // 观察者位置
uniform vec3 globalAmbient;     // I_a: 全局环境光强度（独立于光源）

// 阴影贴图
uniform sampler2D shadowMap;
uniform bool useShadows;  // 是否启用阴影

// 计算阴影因子（PCF - Percentage Closer Filtering，用于柔化阴影边缘）
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    if (!useShadows) {
        return 1.0;  // 不启用阴影时，返回1.0（完全照亮）
    }
    
    // 执行透视除法，将齐次坐标转换为NDC坐标
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // 转换到 [0,1] 范围
    projCoords = projCoords * 0.5 + 0.5;
    
    // 检查片段是否在阴影贴图范围内
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0) {
        return 1.0;  // 超出范围，不在阴影中
    }
    
    // 获取当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    
    // 阴影偏移（Shadow Bias）用于避免阴影痤疮
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // PCF（百分比接近滤波）- 采样周围像素，柔化阴影边缘
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
            shadow += currentDepth - bias > closestDepth ? 0.0 : 1.0;
        }
    }
    shadow /= 9.0;  // 9个采样点
    
    return shadow;
}

// 计算单个光源的贡献（改进的Blinn-Phong模型）
// 标准Phong公式: I = I_a * k_a + I_p * [k_d * (N·L) + k_s * (R·V)^n]
// Blinn-Phong改进: 使用半角向量H代替R，计算 (N·H)^n
vec3 CalculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow) {
    // 计算光源方向
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 计算距离衰减（只应用于直接光照，不包括环境光）
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                                light.quadratic * (distance * distance));
    
    // 光源颜色和强度
    vec3 lightColorIntensity = light.color * light.intensity;
    
    // 1. 漫反射分量（Lambertian反射）
    // I_d = I_p * k_d * (N·L)
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = material.diffuse * NdotL * lightColorIntensity;
    
    // 2. 镜面反射分量（Blinn-Phong模型）
    // I_s = I_p * k_s * (N·H)^n
    // 使用半角向量H代替反射向量R，计算更高效
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float spec = pow(NdotH, material.shininess);
    vec3 specular = material.specular * spec * lightColorIntensity;
    
    // 应用距离衰减（只对直接光照分量）
    diffuse *= attenuation;
    specular *= attenuation;
    
    // 应用阴影因子（阴影中的区域只有环境光）
    diffuse *= shadow;
    specular *= shadow;
    
    // 返回直接光照贡献（不包括环境光，环境光单独处理）
    return diffuse + specular;
}

void main()
{
    if (useOverrideColor) {
        FragColor = vec4(overrideColor, 1.0);
    } else {
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);
        
        // 标准Phong光照模型计算
        // I = I_a * k_a + Σ[I_p * (k_d * (N·L) + k_s * (N·H)^n)]
        
        // 1. 全局环境光分量（不受距离衰减影响）
        // I_ambient = I_a * k_a
        vec3 ambient = material.ambient * globalAmbient;
        
        // 2. 计算阴影因子（使用第一个光源作为主光源生成阴影）
        float shadow = 1.0;
        if (numLights > 0 && useShadows) {
            vec3 lightDir = normalize(lights[0].position - FragPos);
            shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);
        }
        
        // 3. 累加所有光源的直接光照贡献
        vec3 directLighting = vec3(0.0);
        for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
            directLighting += CalculateLight(lights[i], norm, FragPos, viewDir, shadow);
        }
        
        // 4. 最终颜色 = 环境光 + 直接光照
        vec3 result = ambient + directLighting;
        
        // 5. 将顶点颜色作为基色（用于棋盘格等顶点着色物体）
        result *= Color;
        
        // 6. 防止颜色值溢出（根据课程建议）
        // 方法1: 限制到有效范围 [0, 1]
        result = clamp(result, 0.0, 1.0);
        
        FragColor = vec4(result, 1.0);
    }
}
