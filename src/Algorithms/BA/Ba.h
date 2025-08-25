#pragma once

#include "../../Enginee/Node.h"
#include <functional>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class BFS {
public:
  struct Result {
    bool found = false;
    std::vector<uint32_t> path;    // Camino desde inicio hasta fin
    std::vector<uint32_t> visited; // Todos los nodos visitados en orden
    std::unordered_map<uint32_t, uint32_t>
        cameFrom; // Para reconstruir el camino
  };

  static Result FindPath(const std::vector<Node> &nodes,
                         const std::vector<Connection> &connections,
                         const std::vector<uint8_t> &nodeActive,
                         uint32_t startId, uint32_t endId);
};