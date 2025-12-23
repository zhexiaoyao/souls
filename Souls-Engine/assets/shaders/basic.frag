#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;  // 光源空间的片段位置

uniform bool useOverrideColor;
uniform vec3 overrideColor;

// 阴影贴图
uniform sampler2D shadowMap;
uniform bool useShadows;  // 是否启用阴影

// ============================================
// 材质属性（Material Properties）
// ============================================
// 根据 illumination.txt 文档，材质属性包括：
// - k_a: 环境光反射系数（RGB三个通道，范围0-1）
// - k_d: 漫反射系数（RGB三个通道，范围0-1）
// - k_s: 镜面反射系数（RGB三个通道，范围0-1）
// - n: 镜面反射指数（控制光泽度，值越大表面越光滑）
//
// 文档示例值：k_a=0.2, k_d=0.2, k_s=0.6, n=4
// 注意：镜面反射系数通常不决定物体颜色，物体颜色主要由 k_a 和 k_d 决定
struct Material {
    vec3 ambient;      // k_a: 环境光反射系数 (RGB, 0-1)
    vec3 diffuse;      // k_d: 漫反射系数 (RGB, 0-1)
    vec3 specular;     // k_s: 镜面反射系数 (RGB, 0-1)
    float shininess;   // n: 镜面反射指数（控制光泽度，值越大高光越集中）
};
uniform Material material;

// ============================================
// 光源结构（Light Source）
// ============================================
// 根据 illumination.txt 文档，点光源（Point Light）特性：
// - 位置：光源在3D空间中的位置
// - 强度：I_p，光源的颜色和强度
// - 衰减：距离衰减，模拟真实世界中光强度随距离减弱
struct Light {
    vec3 position;     // 光源位置（点光源）
    vec3 color;        // I_p: 光源颜色和强度（RGB）
    float intensity;   // 光源强度倍数（额外强度控制）
    // 距离衰减参数（Distance Attenuation）
    // attenuation = 1.0 / (constant + linear * distance + quadratic * distance^2)
    float constant;    // 常数项（通常为1.0）
    float linear;      // 线性项（控制线性衰减）
    float quadratic;   // 二次项（控制二次衰减）
};

// 支持最多8个光源（符合OpenGL标准，文档第346页提到）
// 每个光源可以有独立的环境光、漫反射和镜面反射分量
#define MAX_LIGHTS 8
uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform vec3 viewPos;           // V: 观察者位置（用于计算视线方向）
uniform vec3 globalAmbient;     // I_a: 全局环境光强度（独立于光源，模拟间接光照）

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
    // 对于墙体等大平面，需要更大的偏移以避免阴影痤疮
    float bias = max(0.1 * (1.0 - dot(normal, lightDir)), 0.01);
    
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

// 计算单个光源的贡献（Blinn-Phong光照模型）
// 根据 illumination.txt 文档：
// 标准Phong公式: I = I_a * k_a + I_p * [k_d * (N·L) + k_s * (R·V)^n]
// Blinn-Phong改进: 使用半角向量H = (L+V)/|L+V|，用 (N·H)^n 代替 (R·V)^n，计算更高效
// 
// 对于多光源，公式为: I = I_a * k_a + Σ[I_p * (k_d * (N·L) + k_s * (N·H)^n)]
// 其中：
//   I_a: 环境光强度 (globalAmbient)
//   k_a: 材质环境光反射系数 (material.ambient)
//   I_p: 点光源强度 (light.color * light.intensity)
//   k_d: 材质漫反射系数 (material.diffuse)
//   k_s: 材质镜面反射系数 (material.specular)
//   n: 镜面反射指数 (material.shininess)
//   N: 表面法线向量
//   L: 光源方向向量（从表面指向光源）
//   H: 半角向量（光源方向和视线方向的平均值）
vec3 CalculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow) {
    // 计算光源方向 L（从表面指向光源）
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 计算距离衰减（只应用于直接光照，不包括环境光）
    // attenuation = 1.0 / (constant + linear * distance + quadratic * distance^2)
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                                light.quadratic * (distance * distance));
    
    // 光源颜色和强度 I_p
    vec3 lightColorIntensity = light.color * light.intensity;
    
    // 1. 漫反射分量（Lambertian反射）
    // I_d = I_p * k_d * (N·L)
    // 其中 N·L = cos(θ)，θ 是法线和光源方向的夹角
    // 使用 max() 确保 N·L >= 0（背面不受光照）
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = material.diffuse * NdotL * lightColorIntensity;
    
    // 2. 镜面反射分量（Blinn-Phong模型）
    // I_s = I_p * k_s * (N·H)^n
    // 其中 H = normalize(L + V) 是半角向量
    // 使用半角向量代替反射向量R，避免计算反射向量，提高效率
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float spec = pow(NdotH, material.shininess);  // (N·H)^n
    vec3 specular = material.specular * spec * lightColorIntensity;
    
    // 应用距离衰减（只对直接光照分量，环境光不受衰减影响）
    diffuse *= attenuation;
    specular *= attenuation;
    
    // 应用阴影因子（阴影中的区域只有环境光，直接光照被遮挡）
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
        
        // ============================================
        // Phong/Blinn-Phong 光照模型计算
        // ============================================
        // 根据 illumination.txt 文档，完整光照公式为：
        // I = I_a * k_a + Σ[I_p * (k_d * (N·L) + k_s * (N·H)^n)]
        //
        // 对于RGB颜色，每个通道分别计算：
        // I_r = I_a * k_ar + Σ[I_p * (k_dr * (N·L) + k_sr * (N·H)^n)]
        // I_g = I_a * k_ag + Σ[I_p * (k_dg * (N·L) + k_sg * (N·H)^n)]
        // I_b = I_a * k_ab + Σ[I_p * (k_db * (N·L) + k_sb * (N·H)^n)]
        //
        // 其中：
        //   - 环境光 I_a * k_a 模拟全局光照（多次反射的光）
        //   - 漫反射 k_d * (N·L) 模拟粗糙表面的反射（Lambertian反射）
        //   - 镜面反射 k_s * (N·H)^n 模拟光滑表面的高光（Blinn-Phong）
        // ============================================
        
        // 1. 全局环境光分量（不受距离衰减影响）
        // I_ambient = I_a * k_a
        // 环境光模拟来自周围环境的间接光照，不依赖于光源位置
        vec3 ambient = material.ambient * globalAmbient;
        
        // 2. 计算阴影因子（使用第一个光源作为主光源生成阴影）
        // 阴影计算使用PCF（Percentage Closer Filtering）柔化阴影边缘
        float shadow = 1.0;
        if (numLights > 0 && useShadows) {
            vec3 lightDir = normalize(lights[0].position - FragPos);
            shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);
        }
        
        // 3. 累加所有光源的直接光照贡献
        // 对于多光源，对每个光源的光照贡献求和
        // 根据文档，OpenGL最多支持8个光源（MAX_LIGHTS = 8）
        vec3 directLighting = vec3(0.0);
        for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
            directLighting += CalculateLight(lights[i], norm, FragPos, viewDir, shadow);
        }
        
        // 4. 最终颜色 = 环境光 + 直接光照
        // I = I_ambient + Σ(I_direct)
        vec3 result = ambient + directLighting;
        
        // 5. 防止颜色值溢出（根据 illumination.txt 文档第30页建议）
        // 文档提到：多光源时值可能超过最大显示值，需要处理
        // 方法1: 限制到有效范围 [0, 1]（当前使用）
        result = clamp(result, 0.0, 1.0);
        
        // 方法2: 归一化（如果值超过1.0，按最大值归一化）
        // 这种方法可以保持颜色比例，但会降低整体亮度
        // float maxComponent = max(max(result.r, result.g), result.b);
        // if (maxComponent > 1.0) {
        //     result = result / maxComponent;
        // }
        
        // 方法3: 通过合理选择材质属性避免溢出（推荐）
        // 确保 k_a + k_d + k_s 的总和合理，避免过度照明
        
        FragColor = vec4(result, 1.0);
    }
}
