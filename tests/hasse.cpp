#include "functions.hpp"


int main(int argc, const char* argv[]) {
  HDGraph hasse;
  
  RBGraph g;
  read_graph("tests/no_5x2.txt", g);
  hasse_diagram(g, hasse);
  
  assert(num_vertices(hasse) == 3);
  assert(num_edges(hasse) == 2);
  
  std::cout << "hasse: tests passed" << std::endl;
  
  return 0;
}
