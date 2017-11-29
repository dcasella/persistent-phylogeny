#include "functions.hpp"


int main(int argc, const char* argv[]) {
  if (argc < 2)
    return 0;
  
  for (int i = 1; i < argc; ++i) {
    RBGraph g;
    read_graph(argv[i], g);
    
    try {
      std::list<SignedCharacter> output = reduce(g);
      
      std::cout << "Ok < ";
      
      for (auto i = output.begin(); i != output.end(); ++i) {
        std::cout << *i << " ";
      }
      
      std::cout << ">" << std::endl;
    }
    catch (const std::runtime_error& e) {
      std::cout << "No" << std::endl;
    }
  }
  
  return 0;
}
