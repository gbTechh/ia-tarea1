
#include "Grafo.h"
#include <algorithm>
#include <cstdint>
#include <unordered_set>
#include <utility>

Grafo::Result Grafo::FindPathAstart2(

    const std::vector<Node> &nodes,
    const std::unordered_map<uint32_t, std::vector<uint32_t>> &adjacencyList,
    const std::vector<uint8_t> &nodeActive, uint32_t startId, uint32_t endId) {

  Result result;

  if (startId >= nodes.size() || endId >= nodes.size() ||
      nodeActive[startId] == 0 || nodeActive[endId] == 0) {
    return result;
  }

  // Priority queue: pair<fScore, nodeId>
  using NodePair = std::pair<float, uint32_t>;
  std::priority_queue<NodePair, std::vector<NodePair>, std::greater<NodePair>>
      openSet;

  std::unordered_map<uint32_t, float> gScore; // Costo desde inicio
  std::unordered_set<uint32_t> closedSet;
  std::unordered_map<uint32_t, uint32_t> cameFrom;

  // Inicializar gScore con infinito
  for (uint32_t i = 0; i < nodes.size(); ++i) {
    gScore[i] = std::numeric_limits<float>::max();
  }
  gScore[startId] = 0.0f;

  // Función heurística
  auto heuristic = [&](uint32_t nodeId) {
    return distance(nodes[nodeId], nodes[endId]);
  };

  // Función distancia entre nodos
  auto distBetween = [&](uint32_t a, uint32_t b) {
    return distance(nodes[a], nodes[b]);
  };

  openSet.push({heuristic(startId), startId});
  result.visited.push_back(startId);

  while (!openSet.empty()) {
    uint32_t currentId = openSet.top().second;
    openSet.pop();

    if (currentId == endId) {
      result.found = true;

      // Reconstruir camino
      std::vector<uint32_t> path;
      uint32_t node = endId;
      while (node != startId) {
        path.push_back(node);
        node = cameFrom[node];
      }
      path.push_back(startId);
      std::reverse(path.begin(), path.end());

      result.path = path;
      result.cameFrom = cameFrom;
      return result;
    }

    if (closedSet.find(currentId) != closedSet.end()) {
      continue;
    }
    closedSet.insert(currentId);

    auto it = adjacencyList.find(currentId);
    if (it != adjacencyList.end()) {
      for (uint32_t neighborId : it->second) {
        if (neighborId >= nodes.size() || nodeActive[neighborId] == 0 ||
            closedSet.find(neighborId) != closedSet.end()) {
          continue;
        }

        // Calcular costo tentativo
        float tentative_gScore =
            gScore[currentId] + distBetween(currentId, neighborId);

        if (tentative_gScore < gScore[neighborId]) {
          cameFrom[neighborId] = currentId;
          gScore[neighborId] = tentative_gScore;
          float fScore = tentative_gScore + heuristic(neighborId);
          openSet.push({fScore, neighborId});

          result.visited.push_back(neighborId);
        }
      }
    }
  }

  return result;
}