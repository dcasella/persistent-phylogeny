#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  
  // TODO: fill g
  
  std::list<SignedCharacter> output = reduce(g);
  
  std::cout << "C-Reduction: < ";
  for (auto i = output.begin(); i != output.end(); ++i)
    std::cout << *i << " ";
  std::cout << ">" << std::endl;
  
  return 0;
}
