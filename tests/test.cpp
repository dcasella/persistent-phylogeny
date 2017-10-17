#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  
  #ifdef DEBUG
  print_rbgraph(g);
  #endif
  
  return 0;
}

// g++ -O3 -Wfatal-errors -DDEBUG -Isrc -o tests/test tests/test.cpp src/functions.cpp && ./tests/test && rm -f tests/test
