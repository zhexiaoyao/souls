cmake_minimum_required(VERSION 3.15)
project(SoulsEngine)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 修复MSVC并行编译时的PDB写入冲突，并设置UTF-8编码
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS /utf-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /FS /utf-8")
endif()

# 确保构建目录不与源码目录相同
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# 查找GLFW库
# 优先使用系统已安装的GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    # 如果没有找到，使用子项目方式
    message(STATUS "GLFW not found, will build from source")
    add_subdirectory(extern/glfw)
endif()

# 配置GLAD
# 假设我们将在include目录下放置glad头文件
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/KHR)
# 添加src目录到包含路径，以便使用相对路径包含头文件
include_directories(${CMAKE_SOURCE_DIR}/src)

# 查找GLM库
find_package(glm QUIET)
if(NOT glm_FOUND)
    message(STATUS "GLM not found, will build from source")
    add_subdirectory(extern/glm)
endif()

# 包含stb_image头文件
include_directories(${CMAKE_SOURCE_DIR}/extern/stb)

# 明确指定源码文件（避免包含构建目录中的文件）
set(SOURCES
    src/main.cpp
    src/core/Window.cpp
    src/core/OpenGLContext.cpp
    src/core/Shader.cpp
    src/core/Camera.cpp
    src/core/glad_loader.c
    src/core/Node.cpp
    src/core/Scene.cpp
    src/core/SceneNode.cpp
    src/core/ObjectManager.cpp
    src/core/Transform.cpp
    src/core/SelectionSystem.cpp
    src/geometry/Mesh.cpp
    src/geometry/Cube.cpp
    src/geometry/Sphere.cpp
    src/geometry/Cylinder.cpp
    src/geometry/Cone.cpp
    src/geometry/Prism.cpp
    src/geometry/Frustum.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 修复MSVC并行编译时的PDB写入冲突，并设置UTF-8编码（在目标上设置）
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /FS /utf-8)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:/FS /utf-8>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/FS /utf-8>)
endif()

# 链接库
target_link_libraries(${PROJECT_NAME} 
    glfw
    ${CMAKE_DL_LIBS}  # 包含GLAD的动态链接
)

# 链接C++17 filesystem库（Windows需要）
if(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

# 如果GLM作为子项目，需要链接
if(TARGET glm::glm_static)
    target_link_libraries(${PROJECT_NAME} glm::glm_static)
elseif(TARGET glm::glm)
    target_link_libraries(${PROJECT_NAME} glm::glm)
endif()

# 复制资源文件到构建目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# 设置输出目录
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# 调试配置
if(MSVC)
    # 设置Visual Studio的工作目录，方便调试时找到资源文件
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()