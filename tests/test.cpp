#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  std::list<CharacterState> output;
  
  read_graph("tests/no_6x3.txt", g);
  
  try {
    output = reduce(g);
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  
  #ifdef DEBUG
  std::cout << "C-Reduction: < ";
  for (auto i = output.begin(); i != output.end(); ++i)
    std::cout << *i << " ";
  std::cout << ">" << std::endl;
  #endif
  
  return 0;
}

// g++ -O3 -Wfatal-errors -DDEBUG -Isrc -o tests/test tests/test.cpp src/functions.cpp && ./tests/test && rm -f tests/test
