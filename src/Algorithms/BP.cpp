#include "Grafo.h"
#include <algorithm>
#include <cstdint>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>

bool dfsIterative(
    std::stack<uint32_t> &stack,
    std::set<std::pair<uint32_t, uint32_t>> &visitedEdges,
    const std::unordered_map<uint32_t, std::vector<uint32_t>> &adjacencyList,
    const std::vector<Node> &nodes, const std::vector<uint8_t> &nodeActive,
    uint32_t endId, Grafo::Result &result) {
  while (!stack.empty()) {
    uint32_t currentId = stack.top();
    result.visited.push_back(currentId);

    // ¿Encontramos el objetivo?
    if (currentId == endId) {
      result.found = true;

      // Reconstruir camino desde el stack
      std::vector<uint32_t> tempPath;
      while (!stack.empty()) {
        tempPath.push_back(stack.top());
        stack.pop();
      }
      std::reverse(tempPath.begin(), tempPath.end());
      result.path = tempPath;
      return true;
    }

    // Buscar próximo vecino no visitado
    auto it = adjacencyList.find(currentId);
    if (it != adjacencyList.end()) {
      bool foundUnvisited = false;

      for (uint32_t neighborId : it->second) {
        // Verificar si la ARISTA no fue visitada y el nodo es válido
        if (neighborId < nodes.size() && nodeActive[neighborId] == 1 &&
            visitedEdges.find({currentId, neighborId}) == visitedEdges.end()) {

          // Marcar arista como visitada y agregar al stack
          visitedEdges.insert({currentId, neighborId});
          stack.push(neighborId);
          foundUnvisited = true;
          break; // ¡IMPORTANTE! Solo un vecino a la vez
        }
      }

      if (!foundUnvisited) {
        stack.pop(); // Backtrack: no hay más vecinos sin visitar
      }
    } else {
      stack.pop(); // Nodo sin vecinos
    }
  }

  return false;
}

Grafo::Result Grafo::FindPathBP(
    const std::vector<Node> &nodes,
    const std::unordered_map<uint32_t, std::vector<uint32_t>> &adjacencyList,
    const std::vector<uint8_t> &nodeActive, uint32_t startId, uint32_t endId) {

  Result result;

  if (startId >= nodes.size() || endId >= nodes.size() ||
      nodeActive[startId] == 0 || nodeActive[endId] == 0) {
    return result;
  }

  std::stack<uint32_t> stack;
  std::unordered_set<uint32_t> visited;
  std::unordered_map<uint32_t, uint32_t> cameFrom;

  stack.push(startId);
  visited.insert(startId);
  result.visited.push_back(startId);

  while (!stack.empty()) {
    uint32_t currentId = stack.top();
    stack.pop();

    // ¿Encontramos el objetivo?
    if (currentId == endId) {
      result.found = true;

      // Reconstruir camino (IGUAL que BFS)
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

    // Explorar vecinos (en orden inverso para mantener DFS)
    auto it = adjacencyList.find(currentId);
    if (it != adjacencyList.end()) {
      // Recorrer en orden inverso para procesar correctamente en stack
      for (auto rit = it->second.rbegin(); rit != it->second.rend(); ++rit) {
        uint32_t neighborId = *rit;

        if (neighborId < nodes.size() && nodeActive[neighborId] == 1 &&
            visited.find(neighborId) == visited.end()) {

          stack.push(neighborId);
          visited.insert(neighborId);
          result.visited.push_back(neighborId);
          cameFrom[neighborId] = currentId;
        }
      }
    }
  }

  return result;
}
