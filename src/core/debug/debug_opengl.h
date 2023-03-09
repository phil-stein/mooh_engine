#ifndef DEBUG_OPENGL_H
#define DEBUG_OPENGL_H

#include "global/global.h"
#include "GLAD/glad.h"


// @DOC: turn opengl error into string
#define STR_GL_ERR(err)                                                                                                          \
                       (err == GL_NO_ERROR                      ? "no error"                      :                              \
                        err == GL_INVALID_ENUM                  ? "invalid enum"                  :                              \
                        err == GL_INVALID_VALUE                 ? "invalid value"                 :                              \
                        err == GL_INVALID_OPERATION             ? "invalid operation"             :                              \
                        err == GL_STACK_OVERFLOW                ? "stack overflow"                :                              \
                        err == GL_STACK_UNDERFLOW               ? "stack underflow"               :                              \
                        err == GL_OUT_OF_MEMORY                 ? "out of memory"                 :                              \
                        err == GL_INVALID_FRAMEBUFFER_OPERATION ? "invalid framebuffer operation" :                              \
                        "unknown error")
//                      err == GL_INVALID_INDEX // ? not in learnopengl list of debug defines

// @DOC: print a opengl error
#define P_GL_ERR(err)   PF("[GL_ERROR] %s: %d\n  -> file: \"%s\"\n  -> line: %d\n", STR_GL_ERR((err)), (err), __FILE__, __LINE__)

// @DOC: print all errors, before this call
#define GL_GET_ERR()    {                                                                                                         \
                          u32 err, count = 0; while ((err = glGetError()) != GL_NO_ERROR) { P_GL_ERR(err); count++; }             \
                          ERR_CHECK(count <= 0,"had %d opengl error%s\n", count, count > 1 ? "s" : "");                           \
                        }

// @DOC: print all errors, before this call, also add message
#define P_GL_ERR_INFO(erri, msg)    PF("[GL_ERROR] %s: %d\n  -> \"%s\"\n  -> file: \"%s\"\n  -> line: %d\n", STR_GL_ERR((err)), (err), msg,__FILE__, __LINE__)

#define GL_GET_ERR_INFO(msg)     {                                                                                                         \
                                    u32 err, count = 0; while ((err = glGetError()) != GL_NO_ERROR) { P_GL_ERR_INFO(err, msg); count++; }  \
                                    ERR_CHECK(count <= 0,"had %d opengl error%s\n", count, count > 1 ? "s" : "");                          \
                                  }

// @DOC: wrap function and print all error including the wrapped func call
#define GL_ERR_FUNC(func) { (func); GL_GET_ERR_INFO((#func)); }

// #define FRAMEBUFFER_ERR() 
// GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
// GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
// GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
// GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
// GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD



#endif
