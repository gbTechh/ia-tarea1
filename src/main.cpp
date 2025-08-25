#include "Client/Game.h"

int main() {
  Game game;

  // 👇 NUEVO: porcentaje para SPACE (30%)
  game.SetRemovalFraction(0.30f);
  // Inicializar grilla con esos parámetros
  game.InitializeGrid();

  // Resaltar nodos antes de correr
  // (por ejemplo (0,0) y (3,2))
  game.HighlightNodes({0, 2 * game.m_GridConfig.width + 3},
                      glm::vec3(0.0f, 1.0f, 0.0f), 8.0f);

  game.Run();

  return 0;
}