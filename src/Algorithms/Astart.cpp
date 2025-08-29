
#include "Grafo.h"
#include <algorithm>
#include <cstdint>
#include <unordered_set>
#include <utility>

Grafo::Result Grafo::FindPathAstart(
    const std::vector<Node> &nodes,
    const std::unordered_map<uint32_t, std::vector<uint32_t>> &adjacencyList,
    const std::vector<uint8_t> &nodeActive, uint32_t startId, uint32_t endId) {

  Result result;

  // Verificar que los nodos existen y están activos
  if (startId >= nodes.size() || endId >= nodes.size() ||
      nodeActive[startId] == 0 || nodeActive[endId] == 0) {
    return result;
  }

  std::unordered_set<uint32_t> visited;
  std::unordered_map<uint32_t, uint32_t> cameFrom; // 👈 Para reconstruir camino

  uint32_t currentNode = startId;
  result.visited.push_back(startId);

  while (currentNode != endId) {
    visited.insert(currentNode);

    auto it = adjacencyList.find(currentNode);
    if (it == adjacencyList.end()) {
      result.found = false;
      return result;
    }

    std::vector<std::pair<uint32_t, float>> v_disVecino;
    std::vector<std::pair<uint32_t, float>> v_disEnd;
    std::vector<std::pair<uint32_t, float>> v_dis;

    for (uint32_t neighborId : it->second) {
      result.visited.push_back(neighborId);
      // Saltar nodos bloqueados o ya visitados
      if (neighborId >= nodes.size() || nodeActive[neighborId] == 0 ||
          visited.find(neighborId) != visited.end()) {
        continue;
      }

      const Node &currentNodeObj = nodes[currentNode];
      const Node &neighborNodeObj = nodes[neighborId];
      const Node &endNodeObj = nodes[endId];

      // Fórmula CORREGIDA:
      float dsNeighbor = Grafo::distance(currentNodeObj, neighborNodeObj);
      float dsEnd = distance(neighborNodeObj, endNodeObj);
      float totalDs = dsNeighbor + dsEnd;

      v_dis.push_back({neighborId, totalDs});

      // 👇 ¡ACTUALIZAR cameFrom! (importante)
      cameFrom[neighborId] = currentNode; // Guardamos de dónde venimos
    }

    if (v_dis.empty()) {
      result.found = false;
      return result;
    }

    // Encontrar el vecino con menor f(n)
    auto minIt = std::min_element(
        v_dis.begin(), v_dis.end(),
        [](const auto &a, const auto &b) { return a.second < b.second; });

    currentNode = minIt->first;
  }

  // RECONSTRUIR CAMINO usando cameFrom
  if (currentNode == endId) {
    result.found = true;

    // Reconstruir camino desde el final al inicio
    std::vector<uint32_t> path;
    uint32_t node = endId;

    while (node != startId) {
      path.push_back(node);
      node = cameFrom[node]; //
    }
    path.push_back(startId);
    std::reverse(path.begin(), path.end());

    result.path = path;
    result.cameFrom = cameFrom; //
  }

  return result;
}