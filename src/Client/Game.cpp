#include "Game.h"
#include <memory>

void Game::Run() {
  m_Window = std::make_unique<GL::Window>(Nombre, width, height);

  while (m_Window->Running()) {
    HandleUserInput();
    Update();
    Render();
  }
}

void Game::HandleUserInput() { m_Window->GetEvents(); }
void Game::Update() {}
void Game::Render() {}