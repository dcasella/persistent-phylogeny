#include "functions.hpp"


int main(int argc, const char* argv[]) {
  HDGraph hasse;
  // HDVertex v1, v2, v3;
  // HDEdge a, b;
  
  // v1 = add_vertex(hasse);
  // v2 = add_vertex(hasse);
  // v3 = add_vertex(hasse);
  // hasse[v1].vertices = std::list<std::string>({"s2"});
  // hasse[v2].vertices = std::list<std::string>({"s4"});
  // hasse[v3].vertices = std::list<std::string>({"s3", "s5", "s6"});
  // hasse[v1].characters.push_back("c2");
  // hasse[v2].characters.push_back("c3");
  // hasse[v3].characters.push_back("c2");
  // hasse[v3].characters.push_back("c3");
  
  // boost::tie(a, std::ignore) = add_edge(v1, v3, hasse);
  // boost::tie(b, std::ignore) = add_edge(v2, v3, hasse);
  // hasse[a].character = "c3";
  // hasse[a].state = State::gain;
  // hasse[b].character = "c2";
  // hasse[b].state = State::gain;
  
  RBGraph g;
  read_graph("tests/no_5x2.txt", g);
  hasse_diagram(g, hasse);
  
  #ifdef DEBUG
  print_hdgraph(hasse);
  #endif
  
  return 0;
}

// rm -f tests/test && g++ -O3 -Wfatal-errors -DDEBUG -Isrc -o tests/test tests/test.cpp src/functions.cpp && ./tests/test
