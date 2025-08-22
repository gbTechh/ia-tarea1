#pragma once

#include "../Enginee/GridGenerator.h"
#include "../Enginee/GridRenderer.h"
#include "../Enginee/Render.h"
#include "../Enginee/window.h"
#include <cstdint>
#include <memory>

class Game {
public:
  inline static constexpr const char *Nombre = "Grid System Demo";
  inline static constexpr const uint32_t width = 1800;
  inline static constexpr const uint32_t height = 950;

  GridGenerator::GridConfig m_GridConfig{};

public:
  Game();
  virtual ~Game() = default;

  Game(const Game &other) = delete;
  Game &operator=(const Game &other) = delete;

  void Run();

  void HighlightNodes(const std::vector<uint32_t> &nodeIds,
                      const glm::vec3 &color = glm::vec3(1.0f, 0.0f, 0.0f),
                      float size = 4.0f);
  void InitializeGrid();

private:
  std::unique_ptr<GL::Window> m_Window{};
  std::unique_ptr<GL::Render> m_Render{};

  // Sistema de cuadrícula
  std::unique_ptr<GridGenerator> m_GridGenerator{};
  std::unique_ptr<GridRenderer> m_GridRenderer{};

  // Configuración de la cuadrícula

  GridRenderConfig m_RenderConfig{};

private:
  void HandleUserInput();
  void Update();
  void Render();
};