#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  
  read_graph("tests/no_6x3.txt", g);
  
  #ifdef DEBUG
  print_graph(g);
  #endif
  
  return 0;
}
