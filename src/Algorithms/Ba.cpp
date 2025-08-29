#include "Grafo.h"
#include <algorithm>
#include <cstdint>
#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>

Grafo::Result Grafo::FindPathBFS(
    const std::vector<Node> &nodes,
    const std::unordered_map<uint32_t, std::vector<uint32_t>> &adjacencyList,
    const std::vector<uint8_t> &nodeActive, uint32_t startId, uint32_t endId) {

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
    auto it = adjacencyList.find(currentId);
    if (it != adjacencyList.end()) {
      for (uint32_t neighborId : it->second) {
        if (neighborId < nodes.size() && nodeActive[neighborId] == 1 &&
            visited.find(neighborId) == visited.end()) {

          queue.push(neighborId);
          visited.insert(neighborId);
          result.visited.push_back(neighborId);
          cameFrom[neighborId] = currentId;
        }
      }
    }
  }

  return result;
}
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
float distance(const Node &a, const Node &b) {
  return glm::distance(a.position, b.position);
}

void imprimirMatrizAdyacencia(
    const std::unordered_map<uint32_t, std::vector<uint32_t>> &adjacencyList,
    const std::vector<uint8_t> &nodeActive) {

  std::cout << "\n=== MATRIZ DE ADYACENCIA ===\n";

  for (const auto &par : adjacencyList) {
    uint32_t nodoPadre = par.first;
    const std::vector<uint32_t> &hijos = par.second;

    // Verificar si el nodo está activo
    std::string estado =
        (nodoPadre < nodeActive.size() && nodeActive[nodoPadre] == 1)
            ? "ACTIVO"
            : "BLOQUEADO";

    std::cout << "Nodo " << nodoPadre << " (" << estado << ") -> Hijos: [";

    for (size_t i = 0; i < hijos.size(); ++i) {
      // Verificar estado del hijo
      std::string estadoHijo =
          (hijos[i] < nodeActive.size() && nodeActive[hijos[i]] == 1)
              ? ""
              : " (BLOQUEADO)";

      std::cout << hijos[i] << estadoHijo;
      if (i < hijos.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]" << std::endl;
  }

  std::cout << "=============================\n";
}

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

    std::vector<std::pair<uint32_t, float>>
        v_dis; // vector para acumular las sumatorias y calcular la menor

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
      float dsNeighbor = distance(currentNodeObj, neighborNodeObj);
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