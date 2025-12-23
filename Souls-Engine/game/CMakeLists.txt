cmake_minimum_required(VERSION 3.15)
project(CollectGame)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 修复MSVC并行编译时的PDB写入冲突，并设置UTF-8编码
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS /utf-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /FS /utf-8")
endif()

# 获取父目录路径（Souls Engine根目录）
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

# 包含路径
include_directories(${PARENT_DIR}/include)
include_directories(${PARENT_DIR}/include/KHR)
include_directories(${PARENT_DIR}/src)
include_directories(${PARENT_DIR}/extern/imgui)
include_directories(${PARENT_DIR}/extern/imgui/backends)
include_directories(${PARENT_DIR}/extern/stb)

# 查找或构建GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    add_subdirectory(${PARENT_DIR}/extern/glfw)
endif()

# 查找或构建GLM
find_package(glm QUIET)
if(NOT glm_FOUND)
    add_subdirectory(${PARENT_DIR}/extern/glm)
endif()

# 游戏源文件
set(GAME_SOURCES
    main.cpp
    ${PARENT_DIR}/src/core/Window.cpp
    ${PARENT_DIR}/src/core/OpenGLContext.cpp
    ${PARENT_DIR}/src/core/Shader.cpp
    ${PARENT_DIR}/src/core/Camera.cpp
    ${PARENT_DIR}/src/core/glad_loader.c
    ${PARENT_DIR}/src/core/Node.cpp
    ${PARENT_DIR}/src/core/Scene.cpp
    ${PARENT_DIR}/src/core/SceneNode.cpp
    ${PARENT_DIR}/src/core/ObjectManager.cpp
    ${PARENT_DIR}/src/core/Transform.cpp
    ${PARENT_DIR}/src/core/GameManager.cpp
    ${PARENT_DIR}/src/core/Light.cpp
    ${PARENT_DIR}/src/core/LightManager.cpp
    ${PARENT_DIR}/src/core/Material.cpp
    ${PARENT_DIR}/extern/imgui/imgui.cpp
    ${PARENT_DIR}/extern/imgui/imgui_draw.cpp
    ${PARENT_DIR}/extern/imgui/imgui_tables.cpp
    ${PARENT_DIR}/extern/imgui/imgui_widgets.cpp
    ${PARENT_DIR}/extern/imgui/backends/imgui_impl_glfw.cpp
    ${PARENT_DIR}/extern/imgui/backends/imgui_impl_opengl3.cpp
    ${PARENT_DIR}/src/geometry/Mesh.cpp
    ${PARENT_DIR}/src/geometry/Cube.cpp
    ${PARENT_DIR}/src/geometry/Sphere.cpp
    ${PARENT_DIR}/src/geometry/Cylinder.cpp
    ${PARENT_DIR}/src/geometry/Cone.cpp
    ${PARENT_DIR}/src/geometry/Prism.cpp
    ${PARENT_DIR}/src/geometry/Frustum.cpp
)

# 创建游戏可执行文件
add_executable(${PROJECT_NAME} ${GAME_SOURCES})

# 编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /FS /utf-8)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:/FS /utf-8>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/FS /utf-8>)
endif()

# 链接库
target_link_libraries(${PROJECT_NAME} 
    glfw
    ${CMAKE_DL_LIBS}
)

# 链接GLM
if(TARGET glm::glm_static)
    target_link_libraries(${PROJECT_NAME} glm::glm_static)
elseif(TARGET glm::glm)
    target_link_libraries(${PROJECT_NAME} glm::glm)
endif()

# 复制资源文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PARENT_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# 设置输出目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# 调试配置
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()





