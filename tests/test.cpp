#include "functions.hpp"


int main(int argc, const char* argv[]) {
  HDGraph hasse;
  
  RBGraph g1;
  read_graph("tests/no_5x2.txt", g1);
  hasse_diagram(g1, hasse);
  
  RBGraph g;
  RBVertex s2, s3, s4, s5, s6,
           c1, c2, c3, c4, c5, c6, c7;
  RBEdge a, b, c;
  
  s2 = add_vertex(g); g[s2].name = "s2"; g[s2].type = Type::species;
  s3 = add_vertex(g); g[s3].name = "s3"; g[s3].type = Type::species;
  s4 = add_vertex(g); g[s4].name = "s4"; g[s4].type = Type::species;
  s5 = add_vertex(g); g[s5].name = "s5"; g[s5].type = Type::species;
  s6 = add_vertex(g); g[s6].name = "s6"; g[s6].type = Type::species;
  c1 = add_vertex(g); g[c1].name = "c1"; g[c1].type = Type::character;
  c2 = add_vertex(g); g[c2].name = "c2"; g[c2].type = Type::character;
  c3 = add_vertex(g); g[c3].name = "c3"; g[c3].type = Type::character;
  c4 = add_vertex(g); g[c4].name = "c4"; g[c4].type = Type::character;
  c5 = add_vertex(g); g[c5].name = "c5"; g[c5].type = Type::character;
  c6 = add_vertex(g); g[c6].name = "c6"; g[c6].type = Type::character;
  c7 = add_vertex(g); g[c7].name = "c7"; g[c7].type = Type::character;
  
  g[boost::graph_bundle].num_species = 5;
  g[boost::graph_bundle].num_characters = 7;
  
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
  
  HDVertex s;
  bool safe;
  std::tie(s, safe) = safe_source(g, hasse);
  
  return 0;
}

// g++ -O3 -Wfatal-errors -DDEBUG -Isrc -o tests/test tests/test.cpp src/functions.cpp && ./tests/test && rm -f tests/test
