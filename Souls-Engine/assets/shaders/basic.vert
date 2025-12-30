#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 Color;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // 简化：假设法线就是顶点位置（对于中心在原点的几何体）
    // 实际应该从顶点数据中读取法线，但当前顶点格式只有位置和颜色
    Normal = normalize(vec3(model * vec4(aPos, 0.0)));
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Color = aColor;
}

