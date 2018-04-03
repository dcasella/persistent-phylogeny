#include "rbgraph.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  RBVertex v0, v1, v2, v3, v4;

  v0 = add_vertex("v0", g);
  v1 = add_vertex("v1", g);
  v2 = add_vertex("v2", g);
  v3 = add_vertex("v3", g);
  v4 = add_vertex("v4", g);

  RBVertexNameMap& v_map = vertex_map(g);

  assert(num_vertices(g) == 5);
  assert(v_map.at("v0") == v0 && g[v0].name == "v0");
  assert(v_map.at("v1") == v1 && g[v1].name == "v1");
  assert(v_map.at("v2") == v2 && g[v2].name == "v2");
  assert(v_map.at("v3") == v3 && g[v3].name == "v3");
  assert(v_map.at("v4") == v4 && g[v4].name == "v4");

  remove_vertex(v4, g);

  try {
    v_map.at("v4");
  }
  catch (const std::out_of_range& e) {
    assert(num_vertices(g) == 4);
  }

  remove_vertex("v3", g);

  try {
    v_map.at("v3");
  }
  catch (const std::out_of_range& e) {
    assert(num_vertices(g) == 3);
  }

  v3 = add_vertex("v3", g);
  v4 = add_vertex("v3", g);

  assert(v_map.at("v3") == v3 && g[v3].name == "v3");
  assert(v3 == v4);

  std::cout << "map: tests passed" << std::endl;

  return 0;
}
