#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  RBVertex s3, s4, s5,
           c1, c2, c3, c4, c5, c6, c7, c8;
  RBEdge a, b, c;
  
  s3 = add_vertex(g); g[s3].name = "s3"; g[s3].type = Type::species;
  s4 = add_vertex(g); g[s4].name = "s4"; g[s4].type = Type::species;
  s5 = add_vertex(g); g[s5].name = "s5"; g[s5].type = Type::species;
  c1 = add_vertex(g); g[c1].name = "c1"; g[c1].type = Type::character;
  c2 = add_vertex(g); g[c2].name = "c2"; g[c2].type = Type::character;
  c3 = add_vertex(g); g[c3].name = "c3"; g[c3].type = Type::character;
  c4 = add_vertex(g); g[c4].name = "c4"; g[c4].type = Type::character;
  c5 = add_vertex(g); g[c5].name = "c5"; g[c5].type = Type::character;
  c6 = add_vertex(g); g[c6].name = "c6"; g[c6].type = Type::character;
  c7 = add_vertex(g); g[c7].name = "c7"; g[c7].type = Type::character;
  c8 = add_vertex(g); g[c8].name = "c8"; g[c8].type = Type::character;
  
  g[boost::graph_bundle].num_species = 3;
  g[boost::graph_bundle].num_characters = 8;
  
  add_edge(s3, c2, g);
  add_edge(s3, c3, g);
  std::tie(a, std::ignore) = add_edge(s3, c4, g); g[a].color = Color::red;
  add_edge(s4, c1, g);
  add_edge(s4, c2, g);
  std::tie(b, std::ignore) = add_edge(s4, c4, g); g[b].color = Color::red;
  add_edge(s5, c1, g);
  add_edge(s5, c2, g);
  add_edge(s5, c3, g);
  std::tie(c, std::ignore) = add_edge(s5, c4, g); g[c].color = Color::red;
  add_edge(s5, c5, g);
  add_edge(s5, c7, g);
  
  assert(is_free(s3, g) == false);
  assert(is_free(c5, g) == false);
  assert(is_free(c4, g) == true);
  
  std::cout << "free: tests passed" << std::endl;
  
  return 0;
}
