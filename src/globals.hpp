#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <list>
#include <string>


//=============================================================================
// Output modifiers

/**
  @brief Global logging namespace
*/
namespace logging {
  extern bool enabled; ///< Logging toggle
};


//=============================================================================
// Algorithm modifiers

/**
  @brief Global exponential algorithm namespace
*/
namespace exponential {
  extern bool enabled; ///< Exponential algorithm toggle
};

/**
  @brief Global user interaction namespace
*/
namespace interactive {
  extern bool enabled; ///< User interaction toggle
};

/**
  @brief Global safe source selection namespace
*/
namespace nthsource {
  extern size_t index; ///< Safe source index selection
};


//=============================================================================
// Typedefs used for readabily

/**
  Iterator (const) of a list of strings
*/
typedef std::list<std::string>::const_iterator StringIter;

#endif // GLOBALS_HPP
