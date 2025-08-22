#include "../../include/glad/glad.h"
#include "GridRenderer.h"
#include <iostream>

GridRenderer::GridRenderer() {}

GridRenderer::~GridRenderer() { Cleanup(); }

bool GridRenderer::Initialize() {
  std::cout << "Inicializando GridRenderer..." << std::endl;

  if (!CreateNodeShader()) {
    std::cerr << "Error creando shader de nodos" << std::endl;
    return false;
  }

  if (!CreateLineShader()) {
    std::cerr << "Error creando shader de líneas" << std::endl;
    return false;
  }

  if (!CreateBuffers()) {
    std::cerr << "Error creando buffers" << std::endl;
    return false;
  }

  std::cout << "GridRenderer inicializado correctamente" << std::endl;
  return true;
}

bool GridRenderer::CreateNodeShader() {
  const char *vertexShader = R"(
        #version 450 core
        layout (location = 0) in vec2 aPos;
        
        uniform float uPointSize;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            gl_PointSize = uPointSize;
        }
    )";

  const char *fragmentShader = R"(
        #version 450 core
        out vec4 FragColor;
        uniform vec3 uColor;
        
        void main() {
            // Crear puntos circulares
            vec2 coord = gl_PointCoord - vec2(0.5);
            if (length(coord) > 0.5) {
                discard;
            }
            FragColor = vec4(uColor, 1.0);
        }
    )";

  m_NodeShaderProgram = CreateShaderProgram(vertexShader, fragmentShader);
  return m_NodeShaderProgram != 0;
}

bool GridRenderer::CreateLineShader() {
  const char *vertexShader = R"(
        #version 450 core
        layout (location = 0) in vec2 aPos;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

  const char *fragmentShader = R"(
        #version 450 core
        out vec4 FragColor;
        uniform vec3 uColor;
        
        void main() {
            FragColor = vec4(uColor, 1.0);
        }
    )";

  m_LineShaderProgram = CreateShaderProgram(vertexShader, fragmentShader);
  return m_LineShaderProgram != 0;
}

bool GridRenderer::CreateBuffers() {
  // Crear VAO y VBO para nodos
  glGenVertexArrays(1, &m_NodeVAO);
  glGenBuffers(1, &m_NodeVBO);

  glBindVertexArray(m_NodeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_NodeVBO);

  // Configurar atributos de vértices para nodos (posición 2D)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Crear VAO y VBO para líneas
  glGenVertexArrays(1, &m_LineVAO);
  glGenBuffers(1, &m_LineVBO);

  glBindVertexArray(m_LineVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);

  // Configurar atributos de vértices para líneas (posición 2D)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  return true;
}

void GridRenderer::UpdateGrid(const std::vector<Node> &nodes,
                              const std::vector<Connection> &connections) {
  // Actualizar datos de nodos
  m_NodeVertices.clear();
  m_NodeVertices.reserve(nodes.size() * 2);

  for (const auto &node : nodes) {
    m_NodeVertices.push_back(node.position.x);
    m_NodeVertices.push_back(node.position.y);
  }

  m_NodeCount = static_cast<uint32_t>(nodes.size());

  // Actualizar datos de líneas
  m_LineVertices.clear();
  m_LineVertices.reserve(connections.size() *
                         4); // 2 puntos * 2 coordenadas por línea

  for (const auto &connection : connections) {
    const Node &nodeA = nodes[connection.nodeA];
    const Node &nodeB = nodes[connection.nodeB];

    // Punto A
    m_LineVertices.push_back(nodeA.position.x);
    m_LineVertices.push_back(nodeA.position.y);

    // Punto B
    m_LineVertices.push_back(nodeB.position.x);
    m_LineVertices.push_back(nodeB.position.y);
  }

  m_LineCount = static_cast<uint32_t>(connections.size() * 2);

  // Actualizar buffers en GPU
  UpdateNodeBuffer();
  UpdateLineBuffer();
}

void GridRenderer::UpdateNodeBuffer() {
  glBindBuffer(GL_ARRAY_BUFFER, m_NodeVBO);
  glBufferData(GL_ARRAY_BUFFER, m_NodeVertices.size() * sizeof(float),
               m_NodeVertices.data(), GL_STATIC_DRAW);
}

void GridRenderer::UpdateLineBuffer() {
  glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
  glBufferData(GL_ARRAY_BUFFER, m_LineVertices.size() * sizeof(float),
               m_LineVertices.data(), GL_STATIC_DRAW);
}

void GridRenderer::Render(const GridRenderConfig &config) {
  // Habilitar blending para mejor apariencia
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Renderizar líneas primero (para que aparezcan detrás de los nodos)
  if (m_LineCount > 0) {
    glUseProgram(m_LineShaderProgram);
    glUniform3f(glGetUniformLocation(m_LineShaderProgram, "uColor"),
                config.lineColor.r, config.lineColor.g, config.lineColor.b);

    glLineWidth(config.lineWidth);
    glBindVertexArray(m_LineVAO);
    glDrawArrays(GL_LINES, 0, m_LineCount);
  }

  // Renderizar nodos
  if (m_NodeCount > 0) {
    glEnable(GL_PROGRAM_POINT_SIZE);

    glUseProgram(m_NodeShaderProgram);
    glUniform3f(glGetUniformLocation(m_NodeShaderProgram, "uColor"),
                config.nodeColor.r, config.nodeColor.g, config.nodeColor.b);
    glUniform1f(glGetUniformLocation(m_NodeShaderProgram, "uPointSize"),
                config.nodeSize);

    glBindVertexArray(m_NodeVAO);
    glDrawArrays(GL_POINTS, 0, m_NodeCount);
  }

  glBindVertexArray(0);
  glUseProgram(0);
  glDisable(GL_BLEND);
}

uint32_t GridRenderer::CreateShaderProgram(const char *vertexSource,
                                           const char *fragmentSource) {
  uint32_t vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
  uint32_t fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

  if (vertexShader == 0 || fragmentShader == 0) {
    return 0;
  }

  uint32_t program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  CheckProgramLinking(program);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}

uint32_t GridRenderer::CompileShader(uint32_t type, const char *source) {
  uint32_t shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  CheckShaderCompilation(shader,
                         type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

  return shader;
}

void GridRenderer::CheckShaderCompilation(uint32_t shader, const char *type) {
  int success;
  char infoLog[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
    std::cerr << "Error compilando shader " << type << ": " << infoLog
              << std::endl;
  }
}

void GridRenderer::CheckProgramLinking(uint32_t program) {
  int success;
  char infoLog[1024];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 1024, nullptr, infoLog);
    std::cerr << "Error enlazando programa shader: " << infoLog << std::endl;
  }
}

void GridRenderer::Cleanup() {
  if (m_NodeVBO)
    glDeleteBuffers(1, &m_NodeVBO);
  if (m_NodeVAO)
    glDeleteVertexArrays(1, &m_NodeVAO);
  if (m_LineVBO)
    glDeleteBuffers(1, &m_LineVBO);
  if (m_LineVAO)
    glDeleteVertexArrays(1, &m_LineVAO);
  if (m_NodeShaderProgram)
    glDeleteProgram(m_NodeShaderProgram);
  if (m_LineShaderProgram)
    glDeleteProgram(m_LineShaderProgram);
}

void GridRenderer::HighlightNodes(const std::vector<Node> &nodes,
                                  const std::vector<uint32_t> &nodeIds,
                                  const glm::vec3 &color, float size) {
  if (nodeIds.empty())
    return;

  std::vector<float> highlightVertices;
  highlightVertices.reserve(nodeIds.size() * 2);

  for (auto id : nodeIds) {
    if (id < nodes.size()) {
      highlightVertices.push_back(nodes[id].position.x);
      highlightVertices.push_back(nodes[id].position.y);
    }
  }

  // Crear un buffer temporal para estos nodos
  uint32_t vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, highlightVertices.size() * sizeof(float),
               highlightVertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Dibujar
  glEnable(GL_PROGRAM_POINT_SIZE);
  glUseProgram(m_NodeShaderProgram);

  glUniform3f(glGetUniformLocation(m_NodeShaderProgram, "uColor"), color.r,
              color.g, color.b);
  glUniform1f(glGetUniformLocation(m_NodeShaderProgram, "uPointSize"), size);

  glBindVertexArray(vao);
  glDrawArrays(GL_POINTS, 0, (GLsizei)nodeIds.size());

  // Cleanup
  glBindVertexArray(0);
  glUseProgram(0);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}