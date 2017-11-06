#ifndef SAFECHAINEXCEPTION_HPP
#define SAFECHAINEXCEPTION_HPP


/**
  @brief Safe chain DFS Visitor exception
*/
class SafeChain : public std::exception {
public:
  inline const char* what() {
    return "Found safe chain";
  }
};

#endif
