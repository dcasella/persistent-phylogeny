#include "functions.hpp"


int main(int argc, const char* argv[]) {
  HDGraph hasse;
  RBGraph g;
  std::list<CharacterState> lc;
  bool safe;
  
  std::tie(lc, safe) = safe_chain(g, hasse);
  
  assert(safe == false);
  assert(lc.empty());
  
  read_graph("tests/no_5x2.txt", g);
  hasse_diagram(g, hasse);
  
  std::tie(lc, safe) = safe_chain(g, hasse);
  
  assert(safe == true);
  assert(lc == std::list<CharacterState>({ { "c1", State::gain } }));
  
  std::cout << "safechain: tests passed" << std::endl;
  
  return 0;
}
