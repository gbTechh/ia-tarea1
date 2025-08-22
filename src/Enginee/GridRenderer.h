#pragma once

#include "../Enginee/Render.h"
#include "Node.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

// Estructura de configuración fuera de la clase
struct GridRenderConfig {
  glm::vec3 nodeColor = glm::vec3(1.0f, 1.0f, 1.0f); // Blanco
  glm::vec3 lineColor = glm::vec3(0.5f, 0.5f, 1.0f); // Azul claro
  float nodeSize = 2.0f;  // Tamaño del punto en píxeles
  float lineWidth = 1.0f; // Grosor de línea
};

class GridRenderer {
public:
  GridRenderer();
  ~GridRenderer();

  bool Initialize();
  void UpdateGrid(const std::vector<Node> &nodes,
                  const std::vector<Connection> &connections);
  void Render(const GridRenderConfig &config = {});
  void Cleanup();
  void HighlightNodes(const std::vector<Node> &nodes,
                      const std::vector<uint32_t> &nodeIds,
                      const glm::vec3 &color, float size);

private:
  // Shaders
  uint32_t m_NodeShaderProgram = 0;
  uint32_t m_LineShaderProgram = 0;

  // VAOs y VBOs para nodos
  uint32_t m_NodeVAO = 0;
  uint32_t m_NodeVBO = 0;

  // VAOs y VBOs para líneas
  uint32_t m_LineVAO = 0;
  uint32_t m_LineVBO = 0;

  // Datos
  std::vector<float> m_NodeVertices;
  std::vector<float> m_LineVertices;
  uint32_t m_NodeCount = 0;
  uint32_t m_LineCount = 0;

  // Métodos privados
  bool CreateNodeShader();
  bool CreateLineShader();
  bool CreateBuffers();
  void UpdateNodeBuffer();
  void UpdateLineBuffer();
  uint32_t CompileShader(uint32_t type, const char *source);
  uint32_t CreateShaderProgram(const char *vertexSource,
                               const char *fragmentSource);
  void CheckShaderCompilation(uint32_t shader, const char *type);
  void CheckProgramLinking(uint32_t program);
};