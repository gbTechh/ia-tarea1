#include "Game.h"
#include "../../include/glad/glad.h"
#include <GL/gl.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>

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

  m_NodeActive.assign(m_GridGenerator->GetNodes().size(), 1);

  std::cout << "Cuadrícula configurada correctamente" << std::endl;
}

void Game::Run(int n) {
  nAlgorithm = n;
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

void Game::HandleUserInput() {
  m_Window->GetEvents();

  if (m_Window->mouseClicked) {
    int nodeId =
        GetNodeAtMouse((float)m_Window->mouseX, (float)m_Window->mouseY);

    if (nodeId != -1 && m_NodeActive[nodeId] == 1) {
      if (m_StartNodeId == -1) {
        m_StartNodeId = nodeId;
        std::cout << "Nodo inicial: " << nodeId << std::endl;
      } else if (m_EndNodeId == -1 && nodeId != m_StartNodeId) {
        m_EndNodeId = nodeId;
        std::cout << "Nodo final: " << nodeId << std::endl;
      } else {
        // Reiniciar selección
        m_StartNodeId = nodeId;
        m_EndNodeId = -1;
        std::cout << "Reiniciando selección, nuevo nodo inicial: " << nodeId
                  << std::endl;
      }
    }
  }

  if (m_Window->keyRightPressed) {
    nAlgorithm = (nAlgorithm % 4) + 1; // 1→2→3→4→1
    std::cout << "Algoritmo cambiado a: " << nAlgorithm << std::endl;
  }

  if (m_Window->keyLeftPressed) {
    nAlgorithm = (nAlgorithm - 2 + 4) % 4 + 1; // 1→4→3→2→1
    std::cout << "Algoritmo cambiado a: " << nAlgorithm << std::endl;
  }

  if (m_Window->keySpacePressed) {
    RandomlyDeactivateNodes(m_RemovalFraction);
  }

  if (m_Window->keyEnterPressed) {
    StartSearch(); // Solo iniciar búsqueda con Enter
  }
}

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
  const auto &nodes = m_GridGenerator->GetNodes();
  const auto &conns = m_GridGenerator->GetConnections();

  bool hasBlocked = !m_NodeActive.empty() &&
                    std::any_of(m_NodeActive.begin(), m_NodeActive.end(),
                                [](uint8_t v) { return v == 0; });

  // Renderizar la cuadrícula
  if (hasBlocked) {
    m_GridRenderer->RenderMasked(nodes, conns, m_NodeActive, m_RenderConfig);
  } else {
    // Render normal (todo activo)
    m_GridRenderer->Render(m_RenderConfig);
  }

  // Mostrar resultados de búsqueda si existen
  if (m_ShowSearchResult) {
    // Mostrar nodos visitados (color amarillo)
    if (!m_LastSearchResult.visited.empty()) {
      m_GridRenderer->HighlightNodes(nodes, m_LastSearchResult.visited,
                                     glm::vec3(0.2f, 1.0f, 0.5f), 4.0f);
      // m_GridRenderer->HighlightEdges(nodes, m_LastSearchResult.visited,
      // glm::vec3(0.2f, 1.0f, 0.5f), 1.0f);
    }

    // Mostrar camino encontrado (color verde más grande)
    if (!m_LastSearchResult.path.empty()) {
      m_GridRenderer->HighlightNodes(nodes, m_LastSearchResult.path,
                                     glm::vec3(1.0, 1.0, 1.0), 6.0f);
      m_GridRenderer->HighlightEdges(nodes, m_LastSearchResult.path,
                                     glm::vec3(1.0, 1.0, 1.0), 1.0f);
    }
  }

  if (m_StartNodeId != -1) {
    m_GridRenderer->HighlightNodes(nodes, {(uint32_t)m_StartNodeId},
                                   glm::vec3(0.0f, 1.0f, 0.0f), 8.0f);
  }
  if (m_EndNodeId != -1) {
    m_GridRenderer->HighlightNodes(nodes, {(uint32_t)m_EndNodeId},
                                   glm::vec3(1.0f, 0.0f, 0.0f), 8.0f);
  }

  m_Window->SwapBuffers();
}

void Game::HighlightNodes(const std::vector<uint32_t> &nodeIds,
                          const glm::vec3 &color, float size) {
  m_GridRenderer->HighlightNodes(m_GridGenerator->GetNodes(), nodeIds, color,
                                 size);
}

int Game::GetNodeAtMouse(float mouseX, float mouseY) {
  // Convertir a NDC (Normalized Device Coordinates) en rango [-1, 1]
  float ndcX = (2.0f * mouseX) / width - 1.0f;
  float ndcY = 1.0f - (2.0f * mouseY) / height; // invertimos Y

  const auto &nodes = m_GridGenerator->GetNodes();

  int closestId = -1;
  float minDist = 0.03f; // umbral de selección
  for (const auto &node : nodes) {
    float dx = ndcX - node.position.x;
    float dy = ndcY - node.position.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < minDist) {
      minDist = dist;
      closestId = node.id;
    }
  }

  return closestId;
}

void Game::RandomlyDeactivateNodes(float fraction) {
  const auto &nodes = m_GridGenerator->GetNodes();
  size_t N = nodes.size();
  if (N == 0)
    return;
  if (fraction < 0.0f)
    fraction = 0.0f;
  if (fraction > 1.0f)
    fraction = 1.0f;

  // Todos activos y luego apagamos un subconjunto
  m_NodeActive.assign(N, 1);

  size_t toRemove = (size_t)std::floor(N * fraction);
  if (toRemove == 0)
    return;

  // Lista de índices candidatos
  std::vector<size_t> idx(N);
  std::iota(idx.begin(), idx.end(), 0);

  // Evitar desactivar el inicio/fin si existen
  auto isProtected = [&](size_t i) {
    return (m_StartNodeId >= 0 && (size_t)m_StartNodeId == i) ||
           (m_EndNodeId >= 0 && (size_t)m_EndNodeId == i);
  };

  // Barajar
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(idx.begin(), idx.end(), gen);

  size_t removed = 0;
  for (size_t i : idx) {
    if (removed >= toRemove)
      break;
    if (isProtected(i))
      continue;
    m_NodeActive[i] = 0; // bloquear
    ++removed;
  }

  std::cout << "Bloqueados " << removed << " nodos (" << (fraction * 100.0f)
            << "% aprox)." << std::endl;
}

void Game::StartSearch() {
  if (m_StartNodeId == -1 || m_EndNodeId == -1) {
    std::cout << "[ENTER] Selecciona nodo inicial y final antes de iniciar la "
                 "búsqueda."
              << std::endl;
    return;
  }
  // Aquí NO implementamos A* aún. Solo dejamos el “hook”.
  switch (nAlgorithm) {
  case 1:
    RunBFS();
    break;
  case 2:
    RunBP();
    break;
  case 3:
    RunHillC();
    break;
  default:
    RunAstart();
    break;
  }

  // En el futuro:
  // - Construir vecinos usando m_GridConfig y descartando nodos con
  // m_NodeActive[i] == 0
  // - Correr tu algoritmo (BFS/Dijkstra/A*) y almacenar el path (lista de ids)
  // - Dibujar el path con un Highlight especial o líneas custom
}

void Game::RunBFS() {
  std::cout << "Ejecutando BFS desde " << m_StartNodeId << " hasta "
            << m_EndNodeId << std::endl;

  m_LastSearchResult = Grafo::FindPathBFS(
      m_GridGenerator->GetNodes(), m_GridGenerator->GetAdjacencyList(),
      m_NodeActive, static_cast<uint32_t>(m_StartNodeId),
      static_cast<uint32_t>(m_EndNodeId));

  m_ShowSearchResult = true;

  if (m_LastSearchResult.found) {
    std::cout << "Camino encontrado! Longitud: "
              << m_LastSearchResult.path.size() << " nodos" << std::endl;
    std::cout << "Nodos visitados: " << m_LastSearchResult.visited.size()
              << std::endl;
  } else {
    std::cout << "No se encontró camino entre los nodos seleccionados"
              << std::endl;
  }
}
void Game::RunBP() {
  std::cout << "Ejecutando BP desde " << m_StartNodeId << " hasta "
            << m_EndNodeId << std::endl;

  m_LastSearchResult = Grafo::FindPathBP(
      m_GridGenerator->GetNodes(), m_GridGenerator->GetAdjacencyList(),
      m_NodeActive, static_cast<uint32_t>(m_StartNodeId),
      static_cast<uint32_t>(m_EndNodeId));

  m_ShowSearchResult = true;

  if (m_LastSearchResult.found) {
    std::cout << "Camino encontrado! Longitud: "
              << m_LastSearchResult.path.size() << " nodos" << std::endl;
    std::cout << "Nodos visitados: " << m_LastSearchResult.visited.size()
              << std::endl;
  } else {
    std::cout << "No se encontró camino entre los nodos seleccionados"
              << std::endl;
  }
}
void Game::RunHillC() {
  std::cout << "Ejecutando BP desde " << m_StartNodeId << " hasta "
            << m_EndNodeId << std::endl;

  m_LastSearchResult = Grafo::FindPathHillC(
      m_GridGenerator->GetNodes(), m_GridGenerator->GetAdjacencyList(),
      m_NodeActive, static_cast<uint32_t>(m_StartNodeId),
      static_cast<uint32_t>(m_EndNodeId));

  m_ShowSearchResult = true;

  if (m_LastSearchResult.found) {
    std::cout << "Camino encontrado! Longitud: "
              << m_LastSearchResult.path.size() << " nodos" << std::endl;
    std::cout << "Nodos visitados: " << m_LastSearchResult.visited.size()
              << std::endl;
  } else {
    std::cout << "No se encontró camino entre los nodos seleccionados"
              << std::endl;
  }
}
void Game::RunAstart() {
  std::cout << "Ejecutando BP desde " << m_StartNodeId << " hasta "
            << m_EndNodeId << std::endl;

  m_LastSearchResult = Grafo::FindPathAstart(
      m_GridGenerator->GetNodes(), m_GridGenerator->GetAdjacencyList(),
      m_NodeActive, static_cast<uint32_t>(m_StartNodeId),
      static_cast<uint32_t>(m_EndNodeId));

  m_ShowSearchResult = true;

  if (m_LastSearchResult.found) {
    std::cout << "Camino encontrado! Longitud: "
              << m_LastSearchResult.path.size() << " nodos" << std::endl;
    std::cout << "Nodos visitados: " << m_LastSearchResult.visited.size()
              << std::endl;
  } else {
    std::cout << "No se encontró camino entre los nodos seleccionados"
              << std::endl;
  }
}