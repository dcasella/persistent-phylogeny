#include "rbgraph.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g, g1;
  RBVertex v1, v2, v3, v4;

       add_vertex("v0", g);
  v1 = add_vertex("v1", g);
  v2 = add_vertex("v2", g);
  v3 = add_vertex("v3", g);
  v4 = add_vertex("v4", g);

  add_edge(v1, v2, g);
  add_edge(v3, v4, g);

  RBGraphVector components = connected_components(g);
  RBGraphVector components1 = connected_components(g1);

  assert(components.size() == 3);
  assert(num_vertices(*components[0].get()) == 1);
  assert(num_edges(*components[0].get()) == 0);
  assert(num_vertices(*components[1].get()) == 2);
  assert(num_edges(*components[1].get()) == 1);
  assert(num_vertices(*components[2].get()) == 2);
  assert(num_edges(*components[2].get()) == 1);
  assert(components1.size() == 0);

  std::cout << "connected: tests passed" << std::endl;

  return 0;
}
