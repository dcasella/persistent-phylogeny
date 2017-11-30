#include "functions.hpp"


int main(int argc, const char* argv[]) {
  if (argc < 2)
    return 0;

  for (int i = 1; i < argc; ++i) {
    RBGraph g;

    try {
      read_graph(argv[i], g);
    }
    catch (const std::runtime_error& e) {
      std::cerr << "No (" << argv[i] << ") " << e.what() << std::endl;

      continue;
    }

    try {
      std::list<SignedCharacter> output = reduce(g);

      std::cout << "Ok (" << argv[i] << ")";

      #ifdef DEBUG
      std::cout << " < ";

      SignedCharacterIter i = output.begin();
      for (; i != output.end(); ++i) {
        std::cout << *i << " ";
      }

      std::cout << ">";
      #endif

      std::cout << std::endl;
    }
    catch (const std::runtime_error& e) {
      std::cerr << "No (" << argv[i] << ")" << std::endl;
    }
  }

  return 0;
}
