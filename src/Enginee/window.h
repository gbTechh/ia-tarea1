#pragma once

#include <cstdint>
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

private:
  void *m_WindowHandle = nullptr;
  bool m_Running = true;
};
} // namespace GL