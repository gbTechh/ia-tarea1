#pragma once

#include "../Algorithms/Grafo.h"
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

  void Run(int algorithm); // 1 ba, 2bf, 3 greedy, 4 A*

  void HighlightNodes(const std::vector<uint32_t> &nodeIds,
                      const glm::vec3 &color = glm::vec3(1.0f, 0.0f, 0.0f),
                      float size = 4.0f);
  void InitializeGrid();

  void SetRemovalFraction(float fraction) { m_RemovalFraction = fraction; }

  void RunBFS();
  void RunBP();
  void RunHillC();
  void RunAstart();

private:
  std::unique_ptr<GL::Window> m_Window{};
  std::unique_ptr<GL::Render> m_Render{};

  // Sistema de cuadrícula
  std::unique_ptr<GridGenerator> m_GridGenerator{};
  std::unique_ptr<GridRenderer> m_GridRenderer{};

  // Configuración de la cuadrícula

  GridRenderConfig m_RenderConfig{};

  // configuarr click y nodos uiniciales
  int m_StartNodeId = -1;
  int m_EndNodeId = -1;

  // 👇 NUEVO: máscara de nodos activos (1 activo, 0 bloqueado)
  std::vector<uint8_t> m_NodeActive;

  // 👇 NUEVO: % a eliminar (por defecto 30%)
  float m_RemovalFraction = 0.30f;
  int nAlgorithm;
  //*********************ALGORITMOSSSS*********************
  // BA:
  Grafo::Result m_LastSearchResult;
  bool m_ShowSearchResult = false;

private:
  void HandleUserInput();
  void Update();
  void Render();

  void RandomlyDeactivateNodes(float fraction);
  void StartSearch(); // stub/placeholder
  int GetNodeAtMouse(float mouseX, float mouseY);
};