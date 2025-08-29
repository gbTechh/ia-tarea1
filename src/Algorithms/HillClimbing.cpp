#include "Grafo.h"
#include <algorithm>
#include <cstdint>
#include <queue>
#include <unordered_set>
#include <utility>

Grafo::Result Grafo::FindPathHillC(
    const std::vector<Node> &nodes,
    const std::unordered_map<uint32_t, std::vector<uint32_t>> &adjacencyList,
    const std::vector<uint8_t> &nodeActive, uint32_t startId, uint32_t endId) {
  Result result;

  // Verificar que los nodos existen y están activos
  if (startId >= nodes.size() || endId >= nodes.size() ||
      nodeActive[startId] == 0 || nodeActive[endId] == 0) {
    return result;
  }

  std::priority_queue<int> p_queue;
  std::unordered_set<uint32_t> visited;
  std::unordered_map<uint32_t, uint32_t> cameFrom;
  std::vector<std::tuple<int, uint32_t, bool>> v_grafo;
  std::vector<uint32_t> path;

  int priority = 1;
  p_queue.push(priority);
  v_grafo.push_back({priority, startId, true});
  visited.insert(startId);
  result.visited.push_back(startId);
  uint32_t currentId = startId;
  path.push_back(startId); // 👈 AGREGAR startId al path inicial

  // bool isNodohoja = false; // 👈 ELIMINAR - no se usa correctamente

  while (currentId != endId) {
    int lastPriority = p_queue.top();

    // 👇 ELIMINAR esta condición (ya está en el while)
    // if (currentId == endId) { ... }

    auto it = adjacencyList.find(currentId);
    if (it != adjacencyList.end()) {
      std::vector<std::pair<uint32_t, float>> v_dis;
      ++lastPriority;

      for (uint32_t neighborId : it->second) {
        if (neighborId < nodes.size() && nodeActive[neighborId] == 1 &&
            visited.find(neighborId) ==
                visited.end()) { // 👈 CORREGIR condición

          const Node &currentNodeObj = nodes[currentId];
          const Node &neighborNodeObj = nodes[neighborId];
          const Node &endNodeObj =
              nodes[endId]; // 👈 AGREGAR para calcular heurística

          float dsEnd = distance(neighborNodeObj, endNodeObj); // Heurística
          v_dis.push_back({neighborId, dsEnd});
          v_grafo.push_back({lastPriority, neighborId, false});
          result.visited.push_back(neighborId);

          cameFrom[neighborId] = currentId;
        }
      }
      if (v_dis.empty()) {

        bool foundNext = false;
        for (auto &elem : v_grafo) {
          if (!std::get<2>(elem)) { // Si no está expandido
            currentId = std::get<1>(elem);
            std::get<2>(elem) = true;
            foundNext = true;

            // RECONSTRUIR path desde start hasta currentId
            path.clear();
            uint32_t node = currentId;
            std::vector<uint32_t> tempPath;
            while (node != startId) {
              tempPath.push_back(node);
              node = cameFrom[node];
            }
            tempPath.push_back(startId);
            std::reverse(tempPath.begin(), tempPath.end());
            path = tempPath;

            break;
          }
        }
        if (!foundNext) {
          break; // No hay más nodos por explorar
        }
      } else {
        // Encontrar el vecino con menor distancia AL OBJETIVO
        auto minIt = std::min_element(
            v_dis.begin(), v_dis.end(),
            [](const auto &a, const auto &b) { return a.second < b.second; });

        currentId = minIt->first;
        for (auto &elem : v_grafo) {
          if (std::get<0>(elem) == lastPriority &&
              std::get<1>(elem) == currentId) {
            std::get<2>(elem) = true;
            break;
          }
        }
        path.push_back(currentId);
        visited.insert(currentId);

        p_queue.push(lastPriority);
      }
    } else {
      break; // Nodo sin vecinos
    }
  }

  if (currentId == endId) {
    result.found = true;
    result.path = path;
    result.cameFrom = cameFrom;
  }

  return result;
}
