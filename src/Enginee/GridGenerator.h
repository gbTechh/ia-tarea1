#pragma once

#include "Node.h"
#include <vector>

class GridGenerator {
public:
  struct GridConfig {
    uint32_t width = 100;
    uint32_t height = 100;
    float spacing =
        0.02f; // Espaciado entre nodos (en coordenadas normalizadas)
    glm::vec2 offset =
        glm::vec2(-1.0f, -1.0f); // Offset para centrar la cuadrícula
  };

public:
  GridGenerator() = default;
  ~GridGenerator() = default;

  void GenerateGrid(const GridConfig &config);

  const std::vector<Node> &GetNodes() const { return m_Nodes; }
  const std::vector<Connection> &GetConnections() const {
    return m_Connections;
  }

  void Clear();

private:
  std::vector<Node> m_Nodes;
  std::vector<Connection> m_Connections;

  void CreateNodes(const GridConfig &config);
  void CreateConnections(const GridConfig &config);
  uint32_t GetNodeIndex(uint32_t x, uint32_t y, uint32_t width) const;
};