#include "window.h"
#include "../../include/glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <cassert>
#include <iostream>
#include <memory>

namespace GL {

Window::Window(const char *name, uint32_t width, uint32_t height)
    : m_WindowHandle(nullptr), m_Running(true), m_OpenGLContext(nullptr) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
    return;
  }

  // Configurar atributos OpenGL antes de crear la ventana
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  m_WindowHandle =
      SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!m_WindowHandle) {
    std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }

  m_OpenGLContext = SDL_GL_CreateContext((SDL_Window *)m_WindowHandle);
  if (!m_OpenGLContext) {
    std::cerr << "Error creating OpenGL context: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow((SDL_Window *)m_WindowHandle);
    SDL_Quit();
    return;
  }

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << "Error initializing GLAD" << std::endl;
    SDL_GL_DeleteContext(m_OpenGLContext);
    SDL_DestroyWindow((SDL_Window *)m_WindowHandle);
    SDL_Quit();
    return;
  }

  // Opcional: Habilitar V-Sync para evitar tearing
  SDL_GL_SetSwapInterval(1);

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
}

Window::~Window() {
  if (m_OpenGLContext) {
    SDL_GL_DeleteContext(m_OpenGLContext);
  }
  if (m_WindowHandle) {
    SDL_DestroyWindow((SDL_Window *)m_WindowHandle);
  }
  SDL_Quit();
}

void Window::GetEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    switch (e.type) {
    case SDL_QUIT:
      std::cout << "Evento SDL_QUIT recibido" << std::endl;
      m_Running = false;
      break;
    case SDL_WINDOWEVENT:
      if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
        std::cout << "Evento SDL_WINDOWEVENT_CLOSE recibido" << std::endl;
        m_Running = false;
      }
      break;
    }
  }
  // REMOVIDO: Ya no hacemos operaciones de renderizado aquí
}

void Window::SwapBuffers() { SDL_GL_SwapWindow((SDL_Window *)m_WindowHandle); }

std::unique_ptr<Render> Window::CreateRender() {
  return std::make_unique<Render>();
}

} // namespace GL