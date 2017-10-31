#include "functions.hpp"


int main(int argc, const char* argv[]) {
  HDGraph hasse;
  RBGraph g, g_cm;
  std::list<CharacterState> lc;
  bool safe;
  
  std::tie(lc, safe) = safe_chain(g, g_cm, hasse);
  
  assert(safe == false);
  assert(lc.empty());
  
  read_graph("tests/no_5x2.txt", g);
  hasse_diagram(hasse, g);
  g_cm = g;
  maximal_reducible_graph(maximal_characters2(g_cm), g_cm);
  
  std::tie(lc, safe) = safe_chain(g, g_cm, hasse);
  
  assert(safe == true);
  assert(lc == std::list<CharacterState>({ { "c1", State::gain },
                                           { "c2", State::gain },
                                           { "c1", State::lose } }));
  
  std::cout << "safechain: tests passed" << std::endl;
  
  return 0;
}
