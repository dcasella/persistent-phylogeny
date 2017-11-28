#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  RBVertex v0, v1, v2, v3, v4;
  
  v0 = add_vertex("v0", g);
  v1 = add_vertex("v1", g);
  v2 = add_vertex("v2", g);
  v3 = add_vertex("v3", g);
  v4 = add_vertex("v4", g);
  
  RBVertexBimap& bm = bimap(g);
  
  assert(num_vertices(g) == 5);
  assert(bm.left.at("v0") == v0 && bm.right.at(v0) == "v0");
  assert(bm.left.at("v1") == v1 && bm.right.at(v1) == "v1");
  assert(bm.left.at("v2") == v2 && bm.right.at(v2) == "v2");
  assert(bm.left.at("v3") == v3 && bm.right.at(v3) == "v3");
  assert(bm.left.at("v4") == v4 && bm.right.at(v4) == "v4");
  
  remove_vertex(v4, g);
  
  try {
    bm.left.at("v4"); // bm.right.at(v4);
  }
  catch (const std::out_of_range& e) {
    assert(num_vertices(g) == 4);
  }
  
  remove_vertex("v3", g);
  
  try {
    bm.left.at("v3"); // bm.right.at(v3);
  }
  catch (const std::out_of_range& e) {
    assert(num_vertices(g) == 3);
  }
  
  v3 = add_vertex("v3", g);
  v4 = add_vertex("v3", g);
  
  assert(bm.left.at("v3") == v3 && bm.right.at(v3) == "v3");
  assert(v3 == v4);
  
  std::cout << "bimap: tests passed" << std::endl;
  
  return 0;
}
