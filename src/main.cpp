#include "functions.hpp"


int main(int argc, const char* argv[]) {
  if (argc < 2)
    return 0;
  
  for (int i = 1; i < argc; ++i) {
    RBGraph g;
    read_graph(argv[i], g);
    std::list<SignedCharacter> output = reduce(g);
    
    std::cout << "C-Reduction: < ";
    
    for (auto i = output.begin(); i != output.end(); ++i) {
      std::cout << *i << " ";
    }
    
    std::cout << ">" << std::endl;
  }
  
  return 0;
}
