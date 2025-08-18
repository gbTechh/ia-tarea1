#pragma once

#include "Render.h"
#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>

namespace GL {
class Window {
public:
  Window(const char *name, uint32_t width, uint32_t height);
  virtual ~Window();

  Window(const Window &other) = delete;
  Window &operator==(const Window &other) = delete;

  inline bool Running() const { return m_Running; };
  inline void Close() { m_Running = false; };
  void GetEvents();
  void SwapBuffers();

  std::unique_ptr<Render> CreateRender();

private:
  void *m_WindowHandle = nullptr;
  void *m_OpenGLContext = nullptr;
  bool m_Running = true;
};
} // namespace GL