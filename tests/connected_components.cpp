#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  RBVertex v0, v1, v2, v3, v4;
  
  v0 = add_vertex(g); g[v0].name = "v0";
  v1 = add_vertex(g); g[v1].name = "v1";
  v2 = add_vertex(g); g[v2].name = "v2";
  v3 = add_vertex(g); g[v3].name = "v3";
  v4 = add_vertex(g); g[v4].name = "v4";
  
  add_edge(v1, v2, g);
  add_edge(v3, v4, g);
  
  RBGraphVector components;
  
  assert(connected_components(g, components) == 3);
  
  std::cout << "connected_components: tests passed" << std::endl;
  
  return 0;
}
