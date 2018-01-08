#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <list>
#include <string>


//=============================================================================
// Logging modifiers

/**
  @brief Global logging namespace
*/
namespace logging {
  extern bool enabled; ///< Logging toggle global variable
};


//=============================================================================
// Runtime modifiers

/**
  @brief Global runtime namespace
*/
namespace runtime {
  extern bool full; ///< Safe source selection toggle global variable
  extern bool interactive; ///< User interaction toggle global variable
};


//=============================================================================
// Typedefs used for readabily

/**
  Iterator (const) of a list of strings
*/
typedef std::list<std::string>::const_iterator StringIter;

#endif // GLOBALS_HPP
