#include "functions.hpp"


int main(int argc, const char* argv[]) {
  HDGraph hasse;
  std::list<SignedCharacter> lsc;
  bool safe;
  
  std::tie(lsc, safe) = safe_chain(hasse);
  
  assert(safe == false);
  assert(lsc.empty());
  
  RBGraph g, gm;
  read_graph("tests/no_5x2.txt", g);
  gm = maximal_reducible_graph(g);
  hasse_diagram(hasse, g, gm);
  
  std::tie(lsc, safe) = safe_chain(hasse);
  
  assert(safe == true);
  assert(lsc == std::list<SignedCharacter>({ { "c1", State::gain },
                                             { "c2", State::gain },
                                             { "c1", State::lose } }));
  
  std::cout << "safechain: tests passed" << std::endl;
  
  return 0;
}
