#ifndef SAFECHAINEXCEPTION_HPP
#define SAFECHAINEXCEPTION_HPP

#include <exception>


/**
  @brief Safe chain DFS Visitor exception
*/
class SafeChain : public std::exception {
public:
  /**
    @brief Returns the reason of the exception

    @return C String
  */
  inline const char* what() {
    return "Found safe chain";
  }
};

#endif
