#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  Vertex s1, s2, s3, s4, s5, s6,
         c1, c2, c3, c4, c5, c6, c7, c8;
  
  s1 = add_vertex(g); g[s1].name = "s1";
  s2 = add_vertex(g); g[s2].name = "s2";
  s3 = add_vertex(g); g[s3].name = "s3";
  s4 = add_vertex(g); g[s4].name = "s4";
  s5 = add_vertex(g); g[s5].name = "s5";
  s6 = add_vertex(g); g[s6].name = "s6";
  c1 = add_vertex(g); g[c1].name = "c1"; g[c1].type = CHARACTER;
  c2 = add_vertex(g); g[c2].name = "c2"; g[c2].type = CHARACTER;
  c3 = add_vertex(g); g[c3].name = "c3"; g[c3].type = CHARACTER;
  c4 = add_vertex(g); g[c4].name = "c4"; g[c4].type = CHARACTER;
  c5 = add_vertex(g); g[c5].name = "c5"; g[c5].type = CHARACTER;
  c6 = add_vertex(g); g[c6].name = "c6"; g[c6].type = CHARACTER;
  c7 = add_vertex(g); g[c7].name = "c7"; g[c7].type = CHARACTER;
  c8 = add_vertex(g); g[c8].name = "c8"; g[c8].type = CHARACTER;
  
  g[boost::graph_bundle].num_species = 6;
  g[boost::graph_bundle].num_characters = 8;
  
  add_edge(s1, c8, g);
  add_edge(s2, c3, g);
  add_edge(s2, c5, g);
  add_edge(s2, c6, g);
  add_edge(s3, c2, g);
  add_edge(s3, c3, g);
  add_edge(s4, c1, g);
  add_edge(s4, c2, g);
  add_edge(s5, c1, g);
  add_edge(s5, c2, g);
  add_edge(s5, c3, g);
  add_edge(s5, c5, g);
  add_edge(s5, c7, g);
  add_edge(s6, c2, g);
  add_edge(s6, c3, g);
  add_edge(s6, c5, g);
  
  #ifdef DEBUG
  print_graph(g);
  #endif
  
  assert(boost::num_vertices(g) == 14);
  
  remove_singletons(g);
  
  #ifdef DEBUG
  print_graph(g);
  #endif
  
  assert(boost::num_vertices(g) == 13);
  
  std::cout << "singletons: tests passed" << std::endl;
  
  return 0;
}
