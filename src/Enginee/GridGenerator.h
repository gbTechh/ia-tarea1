#pragma once

#include "Node.h"
#include <unordered_map>
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
  const std::unordered_map<uint32_t, std::vector<uint32_t>> &
  GetAdjacencyList() const {
    return m_AdjacencyList;
  }
  void Clear();

private:
  std::vector<Node> m_Nodes;
  std::vector<Connection> m_Connections;
  std::unordered_map<uint32_t, std::vector<uint32_t>> m_AdjacencyList;

  void BuildAdjacencyList();
  void CreateNodes(const GridConfig &config);
  void CreateConnections(const GridConfig &config);
  uint32_t GetNodeIndex(uint32_t x, uint32_t y, uint32_t width) const;
};