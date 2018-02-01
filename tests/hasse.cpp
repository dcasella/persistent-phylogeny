#include "hdgraph.hpp"


int main(int argc, const char* argv[]) {
  HDGraph hasse;
  RBGraph g;

  read_graph("tests/test_5x2.txt", g);
  RBGraph gm = maximal_reducible_graph(g);
  hasse_diagram(hasse, g, gm);

  assert(num_vertices(hasse) == 3);
  assert(num_edges(hasse) == 2);

  std::cout << "hasse: tests passed" << std::endl;

  return 0;
}
