// GLAD加载器实现 - 简化版本
// 注意：这是一个最小实现，仅用于Day 1-2的基础功能
// 实际项目中应该使用从GLAD生成器生成的完整文件
// 访问 https://glad.dav1d.de/ 生成完整的GLAD文件

// 防止 Windows 系统 gl.h 冲突
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdlib.h>

// 存储函数指针
static PFNGLGETSTRINGPROC glad_glGetString = NULL;
static PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
static PFNGLCLEARPROC glad_glClear = NULL;
static PFNGLVIEWPORTPROC glad_glViewport = NULL;
static PFNGLENABLEPROC glad_glEnable = NULL;
static PFNGLDEPTHFUNCPROC glad_glDepthFunc = NULL;
static PFNGLGETERRORPROC glad_glGetError = NULL;

// Shader相关函数指针
static PFNGLCREATESHADERPROC glad_glCreateShader = NULL;
static PFNGLSHADERSOURCEPROC glad_glShaderSource = NULL;
static PFNGLCOMPILESHADERPROC glad_glCompileShader = NULL;
static PFNGLGETSHADERIVPROC glad_glGetShaderiv = NULL;
static PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = NULL;
static PFNGLDELETESHADERPROC glad_glDeleteShader = NULL;
static PFNGLCREATEPROGRAMPROC glad_glCreateProgram = NULL;
static PFNGLATTACHSHADERPROC glad_glAttachShader = NULL;
static PFNGLLINKPROGRAMPROC glad_glLinkProgram = NULL;
static PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = NULL;
static PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = NULL;
static PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = NULL;
static PFNGLUSEPROGRAMPROC glad_glUseProgram = NULL;
static PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;
static PFNGLUNIFORM1IPROC glad_glUniform1i = NULL;
static PFNGLUNIFORM1FPROC glad_glUniform1f = NULL;
static PFNGLUNIFORM3FPROC glad_glUniform3f = NULL;
static PFNGLUNIFORM4FPROC glad_glUniform4f = NULL;
static PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv = NULL;

// VAO/VBO相关函数指针
static PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays = NULL;
static PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray = NULL;
static PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = NULL;
static PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
static PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
static PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
static PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = NULL;
static PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer = NULL;
static PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
static PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
static PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation = NULL;

// 加载OpenGL函数
int gladLoadGLLoader(GLADloadproc load) {
    if (load == NULL) {
        return 0;
    }

    // 加载基本函数
    glad_glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
    if (glad_glGetString == NULL) {
        return 0;
    }

    // 检查OpenGL版本
    const GLubyte* version = glad_glGetString(GL_VERSION);
    if (version == NULL) {
        return 0;
    }

    // 加载其他函数
    glad_glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
    glad_glClear = (PFNGLCLEARPROC)load("glClear");
    glad_glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
    glad_glEnable = (PFNGLENABLEPROC)load("glEnable");
    glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)load("glDepthFunc");
    glad_glGetError = (PFNGLGETERRORPROC)load("glGetError");
    
    // 加载Shader相关函数
    glad_glCreateShader = (PFNGLCREATESHADERPROC)load("glCreateShader");
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC)load("glShaderSource");
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC)load("glCompileShader");
    glad_glGetShaderiv = (PFNGLGETSHADERIVPROC)load("glGetShaderiv");
    glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load("glGetShaderInfoLog");
    glad_glDeleteShader = (PFNGLDELETESHADERPROC)load("glDeleteShader");
    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)load("glCreateProgram");
    glad_glAttachShader = (PFNGLATTACHSHADERPROC)load("glAttachShader");
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)load("glLinkProgram");
    glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load("glGetProgramiv");
    glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load("glGetProgramInfoLog");
    glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)load("glDeleteProgram");
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC)load("glUseProgram");
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load("glGetUniformLocation");
    glad_glUniform1i = (PFNGLUNIFORM1IPROC)load("glUniform1i");
    glad_glUniform1f = (PFNGLUNIFORM1FPROC)load("glUniform1f");
    glad_glUniform3f = (PFNGLUNIFORM3FPROC)load("glUniform3f");
    glad_glUniform4f = (PFNGLUNIFORM4FPROC)load("glUniform4f");
    glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load("glUniformMatrix4fv");
    
    // 加载VAO/VBO相关函数
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load("glGenVertexArrays");
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load("glBindVertexArray");
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load("glDeleteVertexArrays");
    glad_glGenBuffers = (PFNGLGENBUFFERSPROC)load("glGenBuffers");
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC)load("glBindBuffer");
    glad_glBufferData = (PFNGLBUFFERDATAPROC)load("glBufferData");
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load("glDeleteBuffers");
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load("glEnableVertexAttribArray");
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load("glVertexAttribPointer");
    glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)load("glDrawElements");
    glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)load("glDrawArrays");
    glad_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)load("glGetAttribLocation");

    return 1;
}

int gladLoadGLUserPtr(GLADuserptrloadfunc load, void* userptr) {
    if (load == NULL) {
        return 0;
    }

    // 使用用户提供的加载函数
    glad_glGetString = (PFNGLGETSTRINGPROC)load(userptr, "glGetString");
    if (glad_glGetString == NULL) {
        return 0;
    }

    const GLubyte* version = glad_glGetString(GL_VERSION);
    if (version == NULL) {
        return 0;
    }

    glad_glClearColor = (PFNGLCLEARCOLORPROC)load(userptr, "glClearColor");
    glad_glClear = (PFNGLCLEARPROC)load(userptr, "glClear");
    glad_glViewport = (PFNGLVIEWPORTPROC)load(userptr, "glViewport");
    glad_glEnable = (PFNGLENABLEPROC)load(userptr, "glEnable");
    glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)load(userptr, "glDepthFunc");
    glad_glGetError = (PFNGLGETERRORPROC)load(userptr, "glGetError");
    
    // 加载Shader相关函数
    glad_glCreateShader = (PFNGLCREATESHADERPROC)load(userptr, "glCreateShader");
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC)load(userptr, "glShaderSource");
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC)load(userptr, "glCompileShader");
    glad_glGetShaderiv = (PFNGLGETSHADERIVPROC)load(userptr, "glGetShaderiv");
    glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load(userptr, "glGetShaderInfoLog");
    glad_glDeleteShader = (PFNGLDELETESHADERPROC)load(userptr, "glDeleteShader");
    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)load(userptr, "glCreateProgram");
    glad_glAttachShader = (PFNGLATTACHSHADERPROC)load(userptr, "glAttachShader");
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)load(userptr, "glLinkProgram");
    glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load(userptr, "glGetProgramiv");
    glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load(userptr, "glGetProgramInfoLog");
    glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)load(userptr, "glDeleteProgram");
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC)load(userptr, "glUseProgram");
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load(userptr, "glGetUniformLocation");
    glad_glUniform1i = (PFNGLUNIFORM1IPROC)load(userptr, "glUniform1i");
    glad_glUniform1f = (PFNGLUNIFORM1FPROC)load(userptr, "glUniform1f");
    glad_glUniform3f = (PFNGLUNIFORM3FPROC)load(userptr, "glUniform3f");
    glad_glUniform4f = (PFNGLUNIFORM4FPROC)load(userptr, "glUniform4f");
    glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load(userptr, "glUniformMatrix4fv");
    
    // 加载VAO/VBO相关函数
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load(userptr, "glGenVertexArrays");
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load(userptr, "glBindVertexArray");
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load(userptr, "glDeleteVertexArrays");
    glad_glGenBuffers = (PFNGLGENBUFFERSPROC)load(userptr, "glGenBuffers");
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC)load(userptr, "glBindBuffer");
    glad_glBufferData = (PFNGLBUFFERDATAPROC)load(userptr, "glBufferData");
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load(userptr, "glDeleteBuffers");
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load(userptr, "glEnableVertexAttribArray");
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load(userptr, "glVertexAttribPointer");
    glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)load(userptr, "glDrawElements");
    glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)load(userptr, "glDrawArrays");
    glad_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)load(userptr, "glGetAttribLocation");

    return 1;
}

// OpenGL函数实现（使用加载的函数指针）
const GLubyte* glGetString(GLenum name) {
    if (glad_glGetString == NULL) {
        return NULL;
    }
    return glad_glGetString(name);
}

void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    if (glad_glClearColor != NULL) {
        glad_glClearColor(red, green, blue, alpha);
    }
}

void glClear(GLbitfield mask) {
    if (glad_glClear != NULL) {
        glad_glClear(mask);
    }
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    if (glad_glViewport != NULL) {
        glad_glViewport(x, y, width, height);
    }
}

void glEnable(GLenum cap) {
    if (glad_glEnable != NULL) {
        glad_glEnable(cap);
    }
}

void glDepthFunc(GLenum func) {
    if (glad_glDepthFunc != NULL) {
        glad_glDepthFunc(func);
    }
}

GLenum glGetError(void) {
    if (glad_glGetError != NULL) {
        return glad_glGetError();
    }
    return GL_NO_ERROR;
}

// Shader相关函数实现
GLuint glCreateShader(GLenum type) {
    if (glad_glCreateShader != NULL) {
        return glad_glCreateShader(type);
    }
    return 0;
}

void glShaderSource(GLuint shader, GLsizei count, const char* const* string, const GLint* length) {
    if (glad_glShaderSource != NULL) {
        glad_glShaderSource(shader, count, string, length);
    }
}

void glCompileShader(GLuint shader) {
    if (glad_glCompileShader != NULL) {
        glad_glCompileShader(shader);
    }
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint* params) {
    if (glad_glGetShaderiv != NULL) {
        glad_glGetShaderiv(shader, pname, params);
    }
}

void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, char* infoLog) {
    if (glad_glGetShaderInfoLog != NULL) {
        glad_glGetShaderInfoLog(shader, bufSize, length, infoLog);
    }
}

void glDeleteShader(GLuint shader) {
    if (glad_glDeleteShader != NULL) {
        glad_glDeleteShader(shader);
    }
}

GLuint glCreateProgram(void) {
    if (glad_glCreateProgram != NULL) {
        return glad_glCreateProgram();
    }
    return 0;
}

void glAttachShader(GLuint program, GLuint shader) {
    if (glad_glAttachShader != NULL) {
        glad_glAttachShader(program, shader);
    }
}

void glLinkProgram(GLuint program) {
    if (glad_glLinkProgram != NULL) {
        glad_glLinkProgram(program);
    }
}

void glGetProgramiv(GLuint program, GLenum pname, GLint* params) {
    if (glad_glGetProgramiv != NULL) {
        glad_glGetProgramiv(program, pname, params);
    }
}

void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, char* infoLog) {
    if (glad_glGetProgramInfoLog != NULL) {
        glad_glGetProgramInfoLog(program, bufSize, length, infoLog);
    }
}

void glDeleteProgram(GLuint program) {
    if (glad_glDeleteProgram != NULL) {
        glad_glDeleteProgram(program);
    }
}

void glUseProgram(GLuint program) {
    if (glad_glUseProgram != NULL) {
        glad_glUseProgram(program);
    }
}

GLint glGetUniformLocation(GLuint program, const char* name) {
    if (glad_glGetUniformLocation != NULL) {
        return glad_glGetUniformLocation(program, name);
    }
    return -1;
}

void glUniform1i(GLint location, GLint v0) {
    if (glad_glUniform1i != NULL) {
        glad_glUniform1i(location, v0);
    }
}

void glUniform1f(GLint location, GLfloat v0) {
    if (glad_glUniform1f != NULL) {
        glad_glUniform1f(location, v0);
    }
}

void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
    if (glad_glUniform3f != NULL) {
        glad_glUniform3f(location, v0, v1, v2);
    }
}

void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
    if (glad_glUniform4f != NULL) {
        glad_glUniform4f(location, v0, v1, v2, v3);
    }
}

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
    if (glad_glUniformMatrix4fv != NULL) {
        glad_glUniformMatrix4fv(location, count, transpose, value);
    }
}

// VAO/VBO相关函数实现
void glGenVertexArrays(GLsizei n, GLuint* arrays) {
    if (glad_glGenVertexArrays != NULL) {
        glad_glGenVertexArrays(n, arrays);
    }
}

void glBindVertexArray(GLuint array) {
    if (glad_glBindVertexArray != NULL) {
        glad_glBindVertexArray(array);
    }
}

void glDeleteVertexArrays(GLsizei n, const GLuint* arrays) {
    if (glad_glDeleteVertexArrays != NULL) {
        glad_glDeleteVertexArrays(n, arrays);
    }
}

void glGenBuffers(GLsizei n, GLuint* buffers) {
    if (glad_glGenBuffers != NULL) {
        glad_glGenBuffers(n, buffers);
    }
}

void glBindBuffer(GLenum target, GLuint buffer) {
    if (glad_glBindBuffer != NULL) {
        glad_glBindBuffer(target, buffer);
    }
}

void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
    if (glad_glBufferData != NULL) {
        glad_glBufferData(target, size, data, usage);
    }
}

void glDeleteBuffers(GLsizei n, const GLuint* buffers) {
    if (glad_glDeleteBuffers != NULL) {
        glad_glDeleteBuffers(n, buffers);
    }
}

void glEnableVertexAttribArray(GLuint index) {
    if (glad_glEnableVertexAttribArray != NULL) {
        glad_glEnableVertexAttribArray(index);
    }
}

void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
    if (glad_glVertexAttribPointer != NULL) {
        glad_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }
}

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    if (glad_glDrawElements != NULL) {
        glad_glDrawElements(mode, count, type, indices);
    }
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    if (glad_glDrawArrays != NULL) {
        glad_glDrawArrays(mode, first, count);
    }
}

GLint glGetAttribLocation(GLuint program, const char* name) {
    if (glad_glGetAttribLocation != NULL) {
        return glad_glGetAttribLocation(program, name);
    }
    return -1;
}

