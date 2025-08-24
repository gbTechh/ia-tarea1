#include "Game.h"
#include "../../include/glad/glad.h"
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>

Game::Game() {
  std::cout << "Inicializando Game..." << std::endl;

  m_Window = std::make_unique<GL::Window>(Nombre, width, height);
  m_Render = m_Window->CreateRender();

  // Crear sistemas de cuadrícula
  m_GridGenerator = std::make_unique<GridGenerator>();
  m_GridRenderer = std::make_unique<GridRenderer>();

  // Inicializar sistema de renderizado de cuadrícula
  if (!m_GridRenderer->Initialize()) {
    std::cerr << "Error inicializando GridRenderer" << std::endl;
    return;
  }

  InitializeGrid();
}

void Game::InitializeGrid() {
  std::cout << "Configurando cuadrícula..." << std::endl;

  // Configurar cuadrícula (ajustamos el tamaño para que sea visible)
  m_GridConfig.width = 100; // Reducido para mejor rendimiento
  m_GridConfig.height = 100;
  m_GridConfig.spacing = 0.02f;                    // Espaciado entre nodos
  m_GridConfig.offset = glm::vec2(-0.99f, -0.99f); // Centrar aproximadamente

  // Configurar renderizado
  m_RenderConfig.nodeColor = glm::vec3(0.6f, 0.7f, 0.8f); // Nodos blancos
  m_RenderConfig.lineColor = glm::vec3(0.3f, 0.5f, 0.6f); // Líneas azul claro
  m_RenderConfig.nodeSize = 1.2f;                         // Tamaño de nodos
  m_RenderConfig.lineWidth = 0.5f;                        // Grosor de líneas

  // Generar cuadrícula
  m_GridGenerator->GenerateGrid(m_GridConfig);

  // Actualizar renderer con los datos generados
  m_GridRenderer->UpdateGrid(m_GridGenerator->GetNodes(),
                             m_GridGenerator->GetConnections());

  std::cout << "Cuadrícula configurada correctamente" << std::endl;
}

void Game::Run() {
  std::cout << "Iniciando loop principal..." << std::endl;

  m_Render->Viewport(0, 0, width, height);
  glViewport(0, 0, width, height);

  // Habilitar anti-aliasing para líneas más suaves
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  while (m_Window->Running()) {
    HandleUserInput();
    Update();
    Render();
  }
}

void Game::HandleUserInput() { m_Window->GetEvents(); }

void Game::Update() {
  // // Animación sutil de colores
  // static float time = 0.0f;

  // // Animar el color de las líneas
  // float intensity = (glm::sin(time * 0.5f) + 1.0f) * 0.5f;
  // m_RenderConfig.lineColor =
  //     glm::vec3(0.2f + intensity * 0.3f, 0.4f + intensity * 0.2f,
  //               0.6f + intensity * 0.4f);

  // time += 0.016f; // ~60 FPS
}

void Game::Render() {
  // Color de fondo oscuro para mejor contraste
  m_Render->CleanWindow(glm::vec4(0.1f, 0.1f, 0.15f, 1.0f));

  // Renderizar la cuadrícula
  m_GridRenderer->Render(m_RenderConfig);
  
  if (m_StartNodeId >= 0)
    HighlightNodes({(uint32_t)m_StartNodeId}, glm::vec3(0.0f, 1.0f, 0.0f), 8.0f);

  if (m_EndNodeId >= 0)
    HighlightNodes({(uint32_t)m_EndNodeId}, glm::vec3(1.0f, 0.0f, 0.0f), 8.0f);

  // Intercambiar buffers
  m_Window->SwapBuffers();
}

void Game::HighlightNodes(const std::vector<uint32_t> &nodeIds,
                          const glm::vec3 &color, float size) {
  m_GridRenderer->HighlightNodes(m_GridGenerator->GetNodes(), nodeIds, color,
                                 size);
}