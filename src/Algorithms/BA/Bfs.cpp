#include <iostream>
#include <queue>
#include <set>
#include <vector>

template <typename T> struct Node {
  T x, y;
  int id;
  bool isVisited;
  bool operator==(const Node<T> &other) const {
    return x == other.x && y == other.y;
  }
  bool operator<(const Node<T> &other) const {
    if (x != other.x) {
      return x < other.x;
    }
    return y < other.y;
  }
  Node(int _id, T _x, T _y) {
    x = _x;
    y = _y;
    id = _id;
  }
};

template <typename T> class BFS {
private:
  int cont = 0;
  Node<T> *root = nullptr;
  Node<T> *end = nullptr;
  bool isMatch = false;
  std::queue<Node<T>> queue;
  std::set<Node<T>> v_visited;

public:
  BFS(Node<T> *_end) { end = _end; }
  BFS(T _x, T _y, int _id, bool _visited) {}

  bool isVisited(Node<T> *node) {
    return v_visited.find(node) != v_visited.end();
  }

  void addNode(Node<T> node) {
    if (root == nullptr) {
      root = node;
      v_visited.insert(node);
      return;
    }

    if (!isMatch) {
      if (end->x == node.x && end->y == node.y) {
        isMatch = true;
        return;
      }

      queue.push(node);
    }
  }

  std::vector<Node<T>> getNeightbors(Node<T> node) {}
};
