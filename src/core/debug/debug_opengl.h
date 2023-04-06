#ifndef DEBUG_OPENGL_H
#define DEBUG_OPENGL_H

#include "global/global.h"
#include "GLAD/glad.h"


#ifdef DEBUG_OPENGL

#define DEBUG_OPENGL_ABORT  // abort()

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
#define P_GL_ERR(err)   PF_COLOR(PF_RED); PF("[GL_ERROR]"); PF_COLOR(PF_WHITE); PF(" %s: %d\n  -> file: \"%s\"\n  -> line: %d\n", STR_GL_ERR((err)), (err), __FILE__, __LINE__)

// @DOC: print all errors, before this call
#define GL_GET_ERR()    {                                                                                                         \
                          u32 err = GL_NO_ERROR; u32 count = 0; while ((err = glGetError()) != GL_NO_ERROR) { P_GL_ERR(err); count++; }             \
                          if (count > 0) { PF("  -> had %d opengl error%s\n", count, count > 1 ? "s" : ""); DEBUG_OPENGL_ABORT; }            \
                        }

// @DOC: print all errors, before this call, also add message
#define P_GL_ERR_INFO(err, msg)    PF_COLOR(PF_RED); PF("[GL_ERROR]"); PF_COLOR(PF_WHITE); PF(" %s: %d\n  -> \"%s\"\n  -> file: \"%s\"\n  -> line: %d\n", STR_GL_ERR((err)), (err), msg,__FILE__, __LINE__)

#define GL_GET_ERR_INFO(msg)      {                                                                                                        \
                                    u32 err = GL_NO_ERROR; u32 count = 0; while ((err = glGetError()) != GL_NO_ERROR) { P_GL_ERR_INFO(err, msg); count++; }  \
                                    if (count > 0) { PF("  -> had %d opengl error%s\n", count, count > 1 ? "s" : ""); DEBUG_OPENGL_ABORT; }           \
                                  }

// @DOC: wrap function and print all error including the wrapped func call
#define GL_ERR_FUNC(func) { (func); GL_GET_ERR_INFO((#func)); }

#define FRAMEBUFFER_ERR_CHECK(msg)  FRAMEBUFFER_ERR_CHECK_dbg((msg), __FILE__, __LINE__)
INLINE void FRAMEBUFFER_ERR_CHECK_dbg(const char* msg, const char* _file, const int _line)
{
  u32 _status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (_status != GL_FRAMEBUFFER_COMPLETE)
  {
    char _status_str[45]; // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, longest at 44
    char _help_str[1];
    switch (_status)
    {
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT :              
        strcpy(_status_str, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :
        strcpy(_status_str, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER :            
        strcpy(_status_str, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER :                 
        strcpy(_status_str, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE :                  
        strcpy(_status_str, "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED :                       
        strcpy(_status_str, "GL_FRAMEBUFFER_UNSUPPORTED");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS :                
        strcpy(_status_str, "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
        break;
      case GL_INVALID_ENUM :
        strcpy(_status_str, "GL_INVALID_ENUM");
        break;
      default:
        strcpy(_status_str, "unknonwn");
        break;
    }
    PF_COLOR(PF_RED); PF("[FRAMEBUFFER ERROR] ");PF_COLOR(PF_WHITE);
    PF("\"%s\", %d\n", _status_str, _status);
    PF("  %s\n", msg);
    PF("  %s\n", _help_str);
    PF_STYLE(PF_DIM, PF_WHITE); PF_STYLE(PF_ITALIC, PF_WHITE);
    PF("  -> file: \"%s\", line: %d\n", _file, _line);
    PF_STYLE(PF_NORMAL, PF_WHITE);
  }
}

#else

#define STR_GL_ERR(err)
#define P_GL_ERR(err)   
#define GL_GET_ERR()
#define P_GL_ERR_INFO(err, msg)
#define GL_GET_ERR_INFO(msg)
#define GL_ERR_FUNC(func)       (func)
#define FRAMEBUFFER_ERR_CHECK(msg)  

#endif  // DEBUG_OPENGL

// --- func wrappers ---

// -- basics --
#define _glGetError(...)                        glGetError(__VA_ARGS__)       // cant wrap because it return a value
#define _glEnable(...)                          GL_ERR_FUNC( glEnable(__VA_ARGS__) )
#define _glDisable(...)                         GL_ERR_FUNC( glDisable(__VA_ARGS__) )
#define _glClear(...)                           GL_ERR_FUNC( glClear(__VA_ARGS__) )
#define _glClearColor(...)                      GL_ERR_FUNC( glClearColor(__VA_ARGS__) )
#define _glViewport(...)                        GL_ERR_FUNC( glViewport(__VA_ARGS__) )

// -- modes & settings --
#define _glPolygonMode(...)                     GL_ERR_FUNC( glPolygonMode(__VA_ARGS__) ) 
#define _glCullFace(...)                        GL_ERR_FUNC( glCullFace(__VA_ARGS__) )
#define _glDepthFunc(...)                       GL_ERR_FUNC( glDepthFunc(__VA_ARGS__) )
#define _glBlendFunc(...)                       GL_ERR_FUNC( glBlendFunc(__VA_ARGS__) )
#define _glBlendFuncSeparate(...)               GL_ERR_FUNC( glBlendFuncSeparate(__VA_ARGS__) )
#define _glLineWidth(...)                       GL_ERR_FUNC( glLineWidth(__VA_ARGS__) )

// -- vert array --
#define _glGenVertexArrays(...)                 GL_ERR_FUNC( glGenVertexArrays(__VA_ARGS__) )       
#define _glBindVertexArray(...)                 GL_ERR_FUNC( glBindVertexArray(__VA_ARGS__) )       
#define _glVertexAttribPointer(...)             GL_ERR_FUNC( glVertexAttribPointer(__VA_ARGS__) )       
#define _glEnableVertexAttribArray(...)         GL_ERR_FUNC( glEnableVertexAttribArray(__VA_ARGS__) )       
#define _glDrawElements(...)                    GL_ERR_FUNC( glDrawElements(__VA_ARGS__) )     
#define _glDrawArrays(...)                      GL_ERR_FUNC( glDrawArrays(__VA_ARGS__) )      

// -- buffers --
#define _glGenBuffers(...)                      GL_ERR_FUNC( glGenBuffers(__VA_ARGS__) )
#define _glBindBuffer(...)                      GL_ERR_FUNC( glBindBuffer(__VA_ARGS__) )
#define _glReadBuffer(...)                      GL_ERR_FUNC( glReadBuffer(__VA_ARGS__) )
#define _glDrawBuffer(...)                      GL_ERR_FUNC( glDrawBuffer(__VA_ARGS__) )
#define _glDrawBuffers(...)                     GL_ERR_FUNC( glDrawBuffers(__VA_ARGS__) )
#define _glBufferData(...)                      GL_ERR_FUNC( glBufferData(__VA_ARGS__) )
#define _glBufferSubData(...)                   GL_ERR_FUNC( glBufferSubData(__VA_ARGS__) )
#define _glFramebufferRenderbuffer(...)         GL_ERR_FUNC( glFramebufferRenderbuffer(__VA_ARGS__) )
// - framebuffers -
#define _glGenFramebuffers(...)                 GL_ERR_FUNC( glGenFramebuffers(__VA_ARGS__) )
#define _glBindFramebuffer(...)                 GL_ERR_FUNC( glBindFramebuffer(__VA_ARGS__) )
#define _glDeleteFramebuffers(...)              GL_ERR_FUNC( glDeleteFramebuffers(__VA_ARGS__) )
#define _glBlitFramebuffer(...)                 GL_ERR_FUNC( glBlitFramebuffer(__VA_ARGS__) )
#define _glFramebufferTexture2D(...)            GL_ERR_FUNC( glFramebufferTexture2D(__VA_ARGS__) )
#define _glCheckFramebufferStatus(...)          glCheckFramebufferStatus(__VA_ARGS__)       // cant wrap because it return a value
// - renderbuffers -
#define _glGenRenderbuffers(...)                GL_ERR_FUNC( glGenRenderbuffers(__VA_ARGS__) )
#define _glBindRenderbuffer(...)                GL_ERR_FUNC( glBindRenderbuffer(__VA_ARGS__) )
#define _glDeleteRenderbuffers(...)             GL_ERR_FUNC( glDeleteRenderbuffers(__VA_ARGS__) )
#define _glRenderbufferStorage(...)             GL_ERR_FUNC( glRenderbufferStorage(__VA_ARGS__) )
#define _glRenderbufferStorageMultisample(...)  GL_ERR_FUNC( glRenderbufferStorageMultisample(__VA_ARGS__) )

// -- textures --
#define _glGenTextures(...)                     GL_ERR_FUNC( glGenTextures(__VA_ARGS__) )
#define _glBindTexture(...)                     GL_ERR_FUNC( glBindTexture(__VA_ARGS__) )                       
#define _glDeleteTextures(...)                  GL_ERR_FUNC( glDeleteTextures(__VA_ARGS__) )
#define _glTexParameteri(...)                   GL_ERR_FUNC( glTexParameteri(__VA_ARGS__) )
#define _glTexImage2D(...)                      GL_ERR_FUNC( glTexImage2D(__VA_ARGS__) )
#define _glTexImage2DMultisample(...)           GL_ERR_FUNC( glTexImage2DMultisample(__VA_ARGS__) )
#define _glGenerateMipmap(...)                  GL_ERR_FUNC( glGenerateMipmap(__VA_ARGS__) )
#define _glActiveTexture(...)                   GL_ERR_FUNC( glActiveTexture(__VA_ARGS__) )
#define _glReadPixels(...)                      GL_ERR_FUNC( glReadPixels(__VA_ARGS__) )
#define _glGetTexImage(...)                     GL_ERR_FUNC( glGetTexImage(__VA_ARGS__) )

// -- shaders --
#define _glCreateShader(...)                    glCreateShader(__VA_ARGS__); GL_GET_ERR()
#define _glDeleteShader(...)                    GL_ERR_FUNC( glDeleteShader(__VA_ARGS__) )
#define _glShaderSource(...)                    GL_ERR_FUNC( glShaderSource(__VA_ARGS__) )
#define _glCompileShader(...)                   GL_ERR_FUNC( glCompileShader(__VA_ARGS__) )
#define _glGetShaderiv(...)                     GL_ERR_FUNC( glGetShaderiv(__VA_ARGS__) )
#define _glAttachShader(...)                    GL_ERR_FUNC( glAttachShader(__VA_ARGS__) )
#define _glGetShaderInfoLog(...)                GL_ERR_FUNC( glGetShaderInfoLog(__VA_ARGS__) )
// - program -
#define _glCreateProgram(...)                   glCreateProgram(__VA_ARGS__); GL_GET_ERR()
#define _glDeleteProgram(...)                   GL_ERR_FUNC( glDeleteProgram(__VA_ARGS__) )
#define _glLinkProgram(...)                     GL_ERR_FUNC( glLinkProgram(__VA_ARGS__) )
#define _glUseProgram(...)                      GL_ERR_FUNC( glUseProgram(__VA_ARGS__) )
#define _glGetProgramiv(...)                    GL_ERR_FUNC( glGetProgramiv(__VA_ARGS__) )
#define _glGetProgramInfoLog(...)               GL_ERR_FUNC( glGetProgramInfoLog(__VA_ARGS__) )
// - uniforms -
#define _glGetUniformLocation(...)              glGetUniformLocation(__VA_ARGS__); GL_GET_ERR() 
#define _glUniform1i(...)                       GL_ERR_FUNC( glUniform1i(__VA_ARGS__) )
#define _glUniform1f(...)                       GL_ERR_FUNC( glUniform1f(__VA_ARGS__) )
#define _glUniform2f(...)                       GL_ERR_FUNC( glUniform2f(__VA_ARGS__) )
#define _glUniform3f(...)                       GL_ERR_FUNC( glUniform3f(__VA_ARGS__) )
#define _glUniformMatrix4fv(...)                GL_ERR_FUNC( glUniformMatrix4fv(__VA_ARGS__) )

#endif // DEBUG_OPENGL_H
