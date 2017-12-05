#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <list>
#include <string>


//=============================================================================
// Logging modifiers

namespace logging {
  extern bool enabled;
};


//=============================================================================
// Runtime modifiers

namespace runtime {
  extern bool full;
};


//=============================================================================
// Typedefs used for readabily

/**
  Iterator (const) of a list of strings
*/
typedef std::list<std::string>::const_iterator StringIter;

#endif // GLOBALS_HPP
