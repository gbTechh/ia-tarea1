#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

struct Node {
  glm::vec2 position;
  uint32_t id;

  Node(float x, float y, uint32_t nodeId) : position(x, y), id(nodeId) {}
};

struct Connection {
  uint32_t nodeA;
  uint32_t nodeB;

  Connection(uint32_t a, uint32_t b) : nodeA(a), nodeB(b) {}
};