#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  RBVertex s1, s2, s3, s4, s5, s6,
           c1, c2, c3, c4, c5, c6, c7, c8;
  RBEdge a, b, c;
  
  s1 = add_vertex(g); g[s1].name = "s1";
  s2 = add_vertex(g); g[s2].name = "s2";
  s3 = add_vertex(g); g[s3].name = "s3";
  s4 = add_vertex(g); g[s4].name = "s4";
  s5 = add_vertex(g); g[s5].name = "s5";
  s6 = add_vertex(g); g[s6].name = "s6";
  c1 = add_vertex(g); g[c1].name = "c1"; g[c1].type = Type::character;
  c2 = add_vertex(g); g[c2].name = "c2"; g[c2].type = Type::character;
  c3 = add_vertex(g); g[c3].name = "c3"; g[c3].type = Type::character;
  c4 = add_vertex(g); g[c4].name = "c4"; g[c4].type = Type::character;
  c5 = add_vertex(g); g[c5].name = "c5"; g[c5].type = Type::character;
  c6 = add_vertex(g); g[c6].name = "c6"; g[c6].type = Type::character;
  c7 = add_vertex(g); g[c7].name = "c7"; g[c7].type = Type::character;
  c8 = add_vertex(g); g[c8].name = "c8"; g[c8].type = Type::character;
  
  g[boost::graph_bundle].num_species = 6;
  g[boost::graph_bundle].num_characters = 8;
  
  add_edge(s1, c8, g);
  add_edge(s2, c3, g);
  add_edge(s2, c5, g);
  add_edge(s2, c6, g);
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
  add_edge(s6, c2, g);
  add_edge(s6, c3, g);
  add_edge(s6, c5, g);
  
  assert(is_inactive(c2, g));
  assert(!is_inactive(s5, g));
  assert(!is_inactive(c4, g));
  
  std::cout << "inactive: tests passed" << std::endl;
  
  return 0;
}
