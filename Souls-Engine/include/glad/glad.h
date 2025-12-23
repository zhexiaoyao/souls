#ifndef GLAD_GLAD_H
#define GLAD_GLAD_H

// 防止 Windows 系统 gl.h 冲突 - 必须在包含任何其他头文件之前定义
#ifndef __gl_h_
#define __gl_h_  // 防止 Windows gl.h 被包含
#endif

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <KHR/khrplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

// GLAD类型定义（已在khrplatform.h中定义）
// GLADloadproc, GLADloadfunc, GLADuserptrloadfunc 已在 khrplatform.h 中定义

// 确保类型已定义
#ifndef GLADloadfunc
typedef void* (*GLADloadfunc)(void *userptr, const char *name);
#endif

// GLAD加载函数
GLAPI int gladLoadGLLoader(GLADloadproc load);
GLAPI int gladLoadGLUserPtr(GLADuserptrloadfunc load, void* userptr);

// OpenGL基本类型定义
#ifndef GLchar
typedef char GLchar;
#endif

// OpenGL基本函数指针类型
typedef const GLubyte* (*PFNGLGETSTRINGPROC)(GLenum name);
typedef void (*PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (*PFNGLCLEARPROC)(GLbitfield mask);
typedef void (*PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (*PFNGLENABLEPROC)(GLenum cap);
typedef void (*PFNGLDEPTHFUNCPROC)(GLenum func);
typedef GLenum (*PFNGLGETERRORPROC)(void);

// Shader相关函数指针类型
typedef GLuint (*PFNGLCREATESHADERPROC)(GLenum type);
typedef void (*PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef void (*PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (*PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
typedef void (*PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, char* infoLog);
typedef void (*PFNGLDELETESHADERPROC)(GLuint shader);
typedef GLuint (*PFNGLCREATEPROGRAMPROC)(void);
typedef void (*PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (*PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (*PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
typedef void (*PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei* length, char* infoLog);
typedef void (*PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void (*PFNGLUSEPROGRAMPROC)(GLuint program);
typedef GLint (*PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const char* name);
typedef void (*PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (*PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (*PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (*PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (*PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

// VAO/VBO相关函数指针类型
typedef void (*PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
typedef void (*PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (*PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint* arrays);
typedef void (*PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (*PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (*PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (*PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef void (*PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices);
typedef void (*PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef GLint (*PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const char* name);

// 纹理相关函数指针类型
typedef void (*PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
typedef void (*PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (*PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
typedef void (*PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
typedef void (*PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (*PFNGLTEXPARAMETERFVPROC)(GLenum target, GLenum pname, const GLfloat* params);
typedef void (*PFNGLACTIVETEXTUREPROC)(GLenum texture);

// 帧缓冲相关函数指针类型
typedef void (*PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
typedef void (*PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (*PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);
typedef void (*PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum (*PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
typedef void (*PFNGLDRAWBUFFERPROC)(GLenum mode);
typedef void (*PFNGLREADBUFFERPROC)(GLenum mode);
typedef void (*PFNGLCULLFACEPROC)(GLenum mode);

// OpenGL函数声明
GLAPI const GLubyte* glGetString(GLenum name);
GLAPI void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI void glClear(GLbitfield mask);
GLAPI void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void glEnable(GLenum cap);
GLAPI void glDepthFunc(GLenum func);
GLAPI GLenum glGetError(void);

// Shader相关函数声明
GLAPI GLuint glCreateShader(GLenum type);
GLAPI void glShaderSource(GLuint shader, GLsizei count, const char* const* string, const GLint* length);
GLAPI void glCompileShader(GLuint shader);
GLAPI void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
GLAPI void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, char* infoLog);
GLAPI void glDeleteShader(GLuint shader);
GLAPI GLuint glCreateProgram(void);
GLAPI void glAttachShader(GLuint program, GLuint shader);
GLAPI void glLinkProgram(GLuint program);
GLAPI void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
GLAPI void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, char* infoLog);
GLAPI void glDeleteProgram(GLuint program);
GLAPI void glUseProgram(GLuint program);
GLAPI GLint glGetUniformLocation(GLuint program, const char* name);
GLAPI void glUniform1i(GLint location, GLint v0);
GLAPI void glUniform1f(GLint location, GLfloat v0);
GLAPI void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLAPI void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLAPI void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

// VAO/VBO相关函数声明
GLAPI void glGenVertexArrays(GLsizei n, GLuint* arrays);
GLAPI void glBindVertexArray(GLuint array);
GLAPI void glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
GLAPI void glGenBuffers(GLsizei n, GLuint* buffers);
GLAPI void glBindBuffer(GLenum target, GLuint buffer);
GLAPI void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
GLAPI void glDeleteBuffers(GLsizei n, const GLuint* buffers);
GLAPI void glEnableVertexAttribArray(GLuint index);
GLAPI void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
GLAPI void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);
GLAPI void glDrawArrays(GLenum mode, GLint first, GLsizei count);
GLAPI GLint glGetAttribLocation(GLuint program, const char* name);

// 纹理相关函数声明
GLAPI void glGenTextures(GLsizei n, GLuint* textures);
GLAPI void glBindTexture(GLenum target, GLuint texture);
GLAPI void glDeleteTextures(GLsizei n, const GLuint* textures);
GLAPI void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
GLAPI void glTexParameteri(GLenum target, GLenum pname, GLint param);
GLAPI void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params);
GLAPI void glActiveTexture(GLenum texture);

// 帧缓冲相关函数声明
GLAPI void glGenFramebuffers(GLsizei n, GLuint* framebuffers);
GLAPI void glBindFramebuffer(GLenum target, GLuint framebuffer);
GLAPI void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers);
GLAPI void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLAPI GLenum glCheckFramebufferStatus(GLenum target);
GLAPI void glDrawBuffer(GLenum mode);
GLAPI void glReadBuffer(GLenum mode);
GLAPI void glCullFace(GLenum mode);

// OpenGL常量
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_DEPTH_TEST                     0x0B71
#define GL_LESS                           0x0201
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_NO_ERROR                       0
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STATIC_DRAW                    0x88E4
#define GL_TRIANGLES                      0x0004
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406

// 纹理相关常量
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_CLAMP_TO_BORDER                0x812D
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_DEPTH_COMPONENT                0x1902

// 帧缓冲相关常量
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_NONE                           0
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405

#ifdef __cplusplus
}
#endif

#endif // GLAD_GLAD_H