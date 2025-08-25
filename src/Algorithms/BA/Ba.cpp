#include "Ba.h"
#include <algorithm>
#include <queue>
#include <unordered_set>

BFS::Result BFS::FindPath(const std::vector<Node> &nodes,
                          const std::vector<Connection> &connections,
                          const std::vector<uint8_t> &nodeActive,
                          uint32_t startId, uint32_t endId) {
  Result result;

  // Verificar que los nodos existen y están activos
  if (startId >= nodes.size() || endId >= nodes.size() ||
      nodeActive[startId] == 0 || nodeActive[endId] == 0) {
    return result;
  }

  std::queue<uint32_t> queue;
  std::unordered_set<uint32_t> visited;
  std::unordered_map<uint32_t, uint32_t> cameFrom;

  queue.push(startId);
  visited.insert(startId);
  result.visited.push_back(startId);

  while (!queue.empty()) {
    uint32_t currentId = queue.front();
    queue.pop();

    // Si encontramos el nodo objetivo
    if (currentId == endId) {
      result.found = true;

      // Reconstruir el camino
      uint32_t node = endId;
      while (node != startId) {
        result.path.push_back(node);
        node = cameFrom[node];
      }
      result.path.push_back(startId);
      std::reverse(result.path.begin(), result.path.end());

      result.cameFrom = cameFrom;
      return result;
    }

    // Encontrar todos los vecinos del nodo actual
    for (const auto &conn : connections) {
      uint32_t neighborId = UINT32_MAX;

      // Verificar si la conexión involucra al nodo actual
      if (conn.nodeA == currentId) {
        neighborId = conn.nodeB;
      } else if (conn.nodeB == currentId) {
        neighborId = conn.nodeA;
      }

      // Si encontramos un vecino válido
      if (neighborId != UINT32_MAX && neighborId < nodes.size() &&
          nodeActive[neighborId] == 1 &&
          visited.find(neighborId) == visited.end()) {

        queue.push(neighborId);
        visited.insert(neighborId);
        result.visited.push_back(neighborId);
        cameFrom[neighborId] = currentId;
      }
    }
  }

  return result;
}