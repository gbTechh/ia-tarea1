#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace GL {

struct Vec4 {
  float r, g, b, a;

  Vec4() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
  Vec4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
};

class Render {
public:
  Render() = default;
  virtual ~Render() = default;

  Render(const Render &other) = delete;
  Render &operator==(const Render &other) = delete;

  void Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
  void CleanWindow(const glm::vec4 &color);
};
} // namespace GL