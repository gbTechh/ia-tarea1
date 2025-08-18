
#include "Game.h"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <memory>

void Game::Run() {
  m_Window = std::make_unique<GL::Window>(Nombre, width, height);
  m_Render = m_Window->CreateRender();

  m_Render->Viewport(0, 0, width, height);

  glViewport(0, 0, width, height);

  while (m_Window->Running()) {
    HandleUserInput();
    Update();
    Render();
  }
}

void Game::HandleUserInput() { m_Window->GetEvents(); }

void Game::Update() {
  // Aquí puedes agregar la lógica de actualización del juego
}

void Game::Render() {
  static float time = 0.0f;

  // Opción 1: Colores suaves con ondas sinusoidales
  float r = (glm::sin(time * 0.5f) + 1.0f) * 0.5f;
  float g = (glm::sin(time * 0.7f + 1.0f) + 1.0f) * 0.5f;
  float b = (glm::sin(time * 0.3f + 2.0f) + 1.0f) * 0.5f;

  // Aquí puedes agregar tu código de renderizado
  // Por ejemplo: dibujar triángulos, texturas, etc.
  m_Render->CleanWindow(glm::vec4(r, g, b, 0.1f));
  // Intercambiar los buffers para mostrar lo renderizado
  m_Window->SwapBuffers();

  time += 0.02f;
}