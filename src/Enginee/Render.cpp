#include "Render.h"
#include "../../include/glad/glad.h"

namespace GL {
void Render::Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  glViewport(x, y, width, height);
}

void Render::CleanWindow(const glm::vec4 &color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}
} // namespace GL