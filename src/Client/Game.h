#pragma once

#include "../Enginee/Render.h"
#include "../Enginee/window.h"
#include <cstdint>
#include <memory>

class Game {
public:
  inline static constexpr const char *Nombre = "Juego de Prueba";
  inline static constexpr const uint32_t width = 800;
  inline static constexpr const uint32_t height = 600;

public:
  Game() = default;
  virtual ~Game() = default;

  Game(const Game &other) = delete;
  Game &operator==(const Game &other) = delete;

  void Run();

private:
  std::unique_ptr<GL::Window> m_Window{};
  std::unique_ptr<GL::Render> m_Render{};

private:
  void HandleUserInput();
  void Update();
  void Render();
};
