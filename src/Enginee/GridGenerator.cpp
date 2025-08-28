#include "GridGenerator.h"
#include <iostream>

void GridGenerator::GenerateGrid(const GridConfig &config) {
  Clear();

  std::cout << "Generando cuadrícula de " << config.width << "x"
            << config.height << " nodos..." << std::endl;

  // Reservar memoria para mejor performance
  m_Nodes.reserve(config.width * config.height);

  // Estimar conexiones: cada nodo interior tiene 4 conexiones (derecha y abajo)
  // Los nodos del borde tienen menos conexiones
  uint32_t estimatedConnections =
      (config.width - 1) * config.height + config.width * (config.height - 1);
  m_Connections.reserve(estimatedConnections);

  CreateNodes(config);
  CreateConnections(config);
  BuildAdjacencyList();

  std::cout << "Cuadrícula generada: " << m_Nodes.size() << " nodos, "
            << m_Connections.size() << " conexiones" << std::endl;
}

void GridGenerator::BuildAdjacencyList() {
  // Limpiar lista anterior
  m_AdjacencyList.clear();

  // Por cada conexión, agregar vecinos a ambos nodos
  for (const auto &conn : m_Connections) {
    m_AdjacencyList[conn.nodeA].push_back(conn.nodeB);
    m_AdjacencyList[conn.nodeB].push_back(conn.nodeA);
  }

  std::cout << "Lista de adyacencia construida para " << m_AdjacencyList.size()
            << " nodos" << std::endl;
}

void GridGenerator::CreateNodes(const GridConfig &config) {
  for (uint32_t y = 0; y < config.height; ++y) {
    for (uint32_t x = 0; x < config.width; ++x) {
      float posX = config.offset.x + (x * config.spacing);
      float posY = config.offset.y + (y * config.spacing);

      uint32_t nodeId = GetNodeIndex(x, y, config.width);
      m_Nodes.emplace_back(posX, posY, nodeId);
    }
  }
}

void GridGenerator::CreateConnections(const GridConfig &config) {
  for (uint32_t y = 0; y < config.height; ++y) {
    for (uint32_t x = 0; x < config.width; ++x) {
      uint32_t currentNode = GetNodeIndex(x, y, config.width);

      // Conectar con el nodo de la derecha
      if (x < config.width - 1) {
        uint32_t rightNode = GetNodeIndex(x + 1, y, config.width);
        m_Connections.emplace_back(currentNode, rightNode);
      }

      // Conectar con el nodo de abajo
      if (y < config.height - 1) {
        uint32_t bottomNode = GetNodeIndex(x, y + 1, config.width);
        m_Connections.emplace_back(currentNode, bottomNode);
      }

      // Conectar en diagonal abajo-derecha
      if (x < config.width - 1 && y < config.height - 1) {
        uint32_t bottomRightNode = GetNodeIndex(x + 1, y + 1, config.width);
        m_Connections.emplace_back(currentNode, bottomRightNode);
      }

      // Conectar en diagonal abajo-izquierda
      if (x > 0 && y < config.height - 1) {
        uint32_t bottomLeftNode = GetNodeIndex(x - 1, y + 1, config.width);
        m_Connections.emplace_back(currentNode, bottomLeftNode);
      }
    }
  }
}

uint32_t GridGenerator::GetNodeIndex(uint32_t x, uint32_t y,
                                     uint32_t width) const {
  return y * width + x;
}

void GridGenerator::Clear() {
  m_Nodes.clear();
  m_Connections.clear();
}