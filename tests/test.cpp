#include "functions.hpp"
#include <random>


int main(int argc, const char* argv[]) {
  if (argc != 4)
    return 0;
  
  RBGraph g;
  std::list<SignedCharacter> output;
  std::mt19937 prng { std::random_device {} () };
  
  RBVertexSize num_species = std::stoul(argv[1]);
  RBVertexSize num_characters = std::stoul(argv[2]);
  RBVertexSize num_edges = std::stoul(argv[3]);
  
  generate_random_rbgraph(g, num_species, num_characters, num_edges, prng);
  
  try {
    output = reduce(g);
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  
  #ifdef DEBUG
  std::cout << "C-Reduction: < ";
  
  for (auto i = output.begin(); i != output.end(); ++i) {
    std::cout << *i << " ";
  }
  
  std::cout << ">" << std::endl;
  #endif
  
  return 0;
}

// make CEXTRA="-Wfatal-errors -DDEBUG" tests/test && ./tests/test s c e && rm -f tests/test
