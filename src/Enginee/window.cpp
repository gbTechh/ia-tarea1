#include "window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <cassert>
#include <iostream>

namespace GL {

Window::Window(const char *name, uint32_t width, uint32_t height)
    : m_WindowHandle(nullptr), m_Running(true) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
    assert(false && "No se pudo inicializar SDL");
  }

  m_WindowHandle =
      SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_SHOWN);
  if (!m_WindowHandle) {
    std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    assert(false && "No se pudo inicializar la ventana SDL");
  }

  SDL_Renderer *renderer = SDL_CreateRenderer((SDL_Window *)m_WindowHandle, -1,
                                              SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Error creating renderer: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow((SDL_Window *)m_WindowHandle);
    SDL_Quit();
    assert(false && "No se pudo crear el renderizador SDL");
  }
}

Window::~Window() {
  SDL_Renderer *renderer = SDL_GetRenderer((SDL_Window *)m_WindowHandle);
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }
  if (m_WindowHandle) {
    SDL_DestroyWindow((SDL_Window *)m_WindowHandle);
  }
  SDL_Quit();
}

void Window::GetEvents() {
  SDL_Renderer *renderer = SDL_GetRenderer((SDL_Window *)m_WindowHandle);
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    switch (e.type) {
    case SDL_QUIT:
      m_Running = false;
      break;
    }
  }

  // Limpiar pantalla (negro)
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

} // namespace GL