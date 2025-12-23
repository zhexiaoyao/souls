#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 Color;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;  // 光源空间的片段位置（用于阴影计算）

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;  // 光源空间矩阵

void main()
{
    // 计算片段在世界空间中的位置
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // ============================================
    // 法线计算（Normal Calculation）
    // ============================================
    // 根据 illumination.txt 文档，法线向量 N 在光照计算中至关重要：
    // - 漫反射：I_d = I_p * k_d * (N·L)，需要法线计算 N·L
    // - 镜面反射：I_s = I_p * k_s * (N·H)^n，需要法线计算 N·H
    //
    // 法线必须转换到世界空间，以便与光源方向（世界空间）进行点积计算
    // 法线矩阵 = transpose(inverse(model)) 的左上3x3部分
    // 这确保了法线在非均匀缩放时仍能正确计算
    //
    // 注意：对于中心在原点的几何体（如球体），顶点位置方向就是法线方向
    vec3 normal = normalize(aPos);
    
    // 将法线转换到世界空间
    // 使用法线矩阵（Normal Matrix）确保只考虑旋转，正确处理缩放
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    Normal = normalize(normalMatrix * normal);
    
    // 计算光源空间的片段位置（用于阴影映射）
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Color = aColor;
}

