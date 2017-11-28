#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  
  add_vertex(g);
  
  assert(!is_empty(g));
  
  remove_vertex(vertex(0, g), g);
  
  assert(is_empty(g));
  
  std::cout << "empty: tests passed" << std::endl;
  
  return 0;
}
