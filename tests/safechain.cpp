#include "functions.hpp"


int main(int argc, const char* argv[]) {
  HDGraph hasse;
  RBGraph g, gm;
  std::list<SignedCharacter> lsc;
  bool safe;
  
  std::tie(lsc, safe) = safe_chain(g, gm, hasse);
  
  assert(safe == false);
  assert(lsc.empty());
  
  read_graph("tests/no_5x2.txt", g);
  hasse_diagram(hasse, g);
  gm = g;
  maximal_reducible_graph(maximal_characters2(gm), gm);
  
  std::tie(lsc, safe) = safe_chain(g, gm, hasse);
  
  assert(safe == true);
  assert(lsc == std::list<SignedCharacter>({ { "c1", State::gain },
                                             { "c2", State::gain },
                                             { "c1", State::lose } }));
  
  std::cout << "safechain: tests passed" << std::endl;
  
  return 0;
}
