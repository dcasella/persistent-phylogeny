#ifndef DEFS_HPP
#define DEFS_HPP

#include <boost/graph/adjacency_list.hpp>
#include <iostream>


//=============================================================================
// Data structures

// General

/**
  Strongly typed 1-bit enum class used for character state.
  
  State is paired with a character in the struct SignedCharacter.
*/
enum class State : bool {
  lose, ///< The paired character is lost
  gain  ///< The paired character is gained
};

/**
  @brief Struct used to represent a signed character
  
  Each character c+ and c− is called a signed character.
*/
struct SignedCharacter {
  std::string character = "c"; ///< Character name
  State state = State::gain;   ///< Character state
};

// Red-Black Graph

/**
  Strongly typed 1-bit enum class used for edge color.
  
  Color is used to label edges in a red-black graph.
*/
enum class Color : bool {
  black, ///< The character incident on the labeled edge is inactive
  red    ///< The character incident on the labeled edge is active
};

/**
  Strongly typed 1-bit enum class used for vertex type.
  
  Type is used to label vertices in a red-black graph.
*/
enum class Type : bool {
  species,  ///< The labeled vertex is a species
  character ///< The labeled vertex is a character
};


//=============================================================================
// Bundled properties

// Red-Black Graph

/**
  @brief Struct used to represent the properties of an edge (red-black graph)
  
  Each character c ∈ C is incident only on black edges (in this case c is
  inactive), or it is incident only on red edges (in this case c is active).
*/
struct RBEdgeProperties {
  Color color = Color::black; ///< Edge color (Red or Black)
};

/**
  @brief Struct used to represent the properties of a vertex (red-black graph)
  
  A red-black graph on a set S of species and a set C of characters, is a
  bipartite graph whose vertex set is S ∪ C.
*/
struct RBVertexProperties {
  std::string name = "v";    ///< Vertex name
  Type type = Type::species; ///< Vertex type (Character or Species)
};

/**
  @brief Struct used to represent the properties of a red-black graph
*/
struct RBGraphProperties {
  size_t num_species = 0;    ///< Number of species in the graph
  size_t num_characters = 0; ///< Number of characters in the graph
};

// Hasse Diagram

/**
  @brief Struct used to represent the properties of an edge (Hasse diagram)
  
  The character c is gained in the edge (x, y) such that y is a child of x and
  c has state 0 in x and state 1 in y.
  In this case the edge (x, y) is labeled by c+.
  Conversely, c is lost in the edge (x, y) if y is a child of x and character c
  has state 1 in x and state 0 in y.
  In the latter case the edge (x, y) is labeled by c−.
  For each character c, we allow at most one edge labeled by c−.
*/
struct HDEdgeProperties {
  std::list<SignedCharacter> signedcharacters; ///< List of SignedCharacter
                                               ///< that label the edge
};

/**
  @brief Struct used to represent the properties of a vertex (Hasse diagram)
  
  Given s a species, by C(s) we denote the set of characters of s.
  Let GM be a maximal reducible graph.
  Then the diagram P for GM is the Hasse diagram for the poset (Ps , ≤) of all
  species of GM ordered by the relation ≤, where s1 ≤ s2 if C(s1) ⊆ C(s2).
*/
struct HDVertexProperties {
  std::list<std::string> species;    ///< List of species that label the vertex
  std::list<std::string> characters; ///< List of characters of the species
};

/**
  @brief Struct used to represent the properties of a Hasse diagram
*/
struct HDGraphProperties {
  
};


//=============================================================================
// Typedefs used for readabily

// General

// Iterators

/**
  Iterator (const) of a list of strings
*/
typedef std::list<std::string>::const_iterator StringIter;

/**
  Iterator (const) of a list of signed characters
*/
typedef std::list<SignedCharacter>::const_iterator SignedCharacterIter;

// Red-Black Graph

// Graph

/**
  Red-black graph
*/
typedef boost::adjacency_list<
  boost::setS,        // OutEdgeList
  boost::listS,       // VertexList
  boost::undirectedS, // Directed
  RBVertexProperties, // VertexProperties
  RBEdgeProperties,   // EdgeProperties
  RBGraphProperties   // GraphProperties
> RBGraph;

// Descriptors

/**
  Edge of a red-black graph
*/
typedef boost::graph_traits<RBGraph>::edge_descriptor RBEdge;

/**
  Vertex of a red-black graph
*/
typedef boost::graph_traits<RBGraph>::vertex_descriptor RBVertex;

// Iterators

/**
  Iterator of edges (red-black graph)
*/
typedef boost::graph_traits<RBGraph>::edge_iterator RBEdgeIter;

/**
  Iterator of vertices (red-black graph)
*/
typedef boost::graph_traits<RBGraph>::vertex_iterator RBVertexIter;

/**
  Iterator of outgoing edges (red-black graph)
*/
typedef boost::graph_traits<RBGraph>::out_edge_iterator RBOutEdgeIter;

// Size types

/**
  Size type of vertices (red-black graph)
*/
typedef boost::graph_traits<RBGraph>::vertices_size_type RBVertexSize;

// Index Maps

/**
  Map of vertex indices (red-black graph)
*/
typedef std::map<RBVertex, RBVertexSize> RBIndexMap;

/**
  Associative property map of vertex indices (red-black graph)
*/
typedef boost::associative_property_map<RBIndexMap> RBAssocMap;

// Containers

/**
  Vector of unique pointers to red-black graphs
*/
typedef std::vector<std::unique_ptr<RBGraph>> RBGraphVector;

// Hasse Diagram

// Graph

/**
  Hasse diagram
*/
typedef boost::adjacency_list<
  boost::setS,           // OutEdgeList
  boost::vecS,           // VertexList
  boost::bidirectionalS, // Directed
  HDVertexProperties,    // VertexProperties
  HDEdgeProperties,      // EdgeProperties
  HDGraphProperties      // GraphProperties
> HDGraph;

// Descriptors

/**
  Edge of a Hasse diagram
*/
typedef boost::graph_traits<HDGraph>::edge_descriptor HDEdge;

/**
  Vertex of a Hasse diagram
*/
typedef boost::graph_traits<HDGraph>::vertex_descriptor HDVertex;

// Iterators

/**
  Iterator of edges (Hasse diagram)
*/
typedef boost::graph_traits<HDGraph>::edge_iterator HDEdgeIter;

/**
  Iterator of vertices (Hasse diagram)
*/
typedef boost::graph_traits<HDGraph>::vertex_iterator HDVertexIter;

/**
  Iterator of incoming edges (Hasse diagram)
*/
typedef boost::graph_traits<HDGraph>::in_edge_iterator HDInEdgeIter;

/**
  Iterator of outgoing edges (Hasse diagram)
*/
typedef boost::graph_traits<HDGraph>::out_edge_iterator HDOutEdgeIter;

//=============================================================================
// Enum / Struct operator overloads

// General

/**
  @brief Overloading of operator<< for Type
  
  @param[in] os Output stream
  @param[in] t  Type
  
  @return Updated output stream
*/
inline std::ostream& operator<<(std::ostream& os, const Type& t) {
  if (t == Type::species)
    os << "Species";
  else
    os << "Character";
  
  return os;
}

/**
  @brief Overloading of operator<< for State
  
  @param[in] os Output stream
  @param[in] s  State
  
  @return Updated output stream
*/
inline std::ostream& operator<<(std::ostream& os, const State& s) {
  if (s == State::lose)
    os << "-";
  else
    os << "+";
  
  return os;
}

/**
  @brief Overloading of operator<< for SignedCharacter
  
  @param[in] os Output stream
  @param[in] sc SignedCharacter
  
  @return Updated output stream
*/
inline std::ostream& operator<<(std::ostream& os, const SignedCharacter& sc) {
  return os << sc.character << sc.state;
}

/**
  @brief Overloading of operator== for a pair of signed characters
  
  @param[in] a SignedCharacter
  @param[in] b SignedCharacter
  
  @return True if a is equal to b
*/
inline bool operator==(const SignedCharacter& a, const SignedCharacter& b) {
  return (a.character == b.character && a.state == b.state);
}

#endif // DEFS_HPP
