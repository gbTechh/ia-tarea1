#include "Render.h"
#include "../../include/glad/glad.h"
#include <GL/glext.h>

static void HandleErros(GLenum src, GLenum type, GLuint id, GLenum severity,
                        GLsizei length, GLchar const *msg,
                        void const *user_param);

namespace GL {

Render::Render() {
  glEnable(GL_DEBUG_OUTPUT);
  // glDebugMessageCallback(HandleErros, nullptr);
}

void Render::Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  glViewport(x, y, width, height);
}

void Render::CleanWindow(const glm::vec4 &color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}
} // namespace GL