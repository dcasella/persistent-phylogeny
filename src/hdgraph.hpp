#ifndef HDGRAPH_HPP
#define HDGRAPH_HPP

#include "rbgraph.hpp"
#include <boost/graph/graph_utility.hpp>


//=============================================================================
// Data structures

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


//=============================================================================
// Bundled properties

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

/**
  Iterator (const) of a list of signed characters
*/
typedef std::list<SignedCharacter>::const_iterator SignedCharacterIter;


//=============================================================================
// Enum / Struct operator overloads

/**
  @brief Overloading of operator<< for State
  
  @param[in] os Output stream
  @param[in] s  State
  
  @return Updated output stream
*/
inline std::ostream& operator<<(std::ostream& os, const State s) {
  return os << (s == State::lose ? "-" : "+");
}

/**
  @brief Overloading of operator<< for SignedCharacter
  
  @param[in] os Output stream
  @param[in] sc SignedCharacter
  
  @return Updated output stream
*/
inline std::ostream& operator<<(std::ostream& os, const SignedCharacter sc) {
  return os << sc.character << sc.state;
}

/**
  @brief Overloading of operator== for a pair of signed characters
  
  @param[in] a SignedCharacter
  @param[in] b SignedCharacter
  
  @return True if a is equal to b
*/
inline bool operator==(const SignedCharacter& a, const SignedCharacter b) {
  return (a.character == b.character && a.state == b.state);
}


//=============================================================================
// Boost functions (overloading)

/**
  @brief Add vertex with \e species and \e characters to \e hasse
  
  @param[in]     species    List of species names
  @param[in]     characters List of character names
  @param[in,out] hasse      Hasse diagram graph
  
  @return Vertex descriptor for the new vertex
*/
HDVertex
add_vertex(
    const std::list<std::string>& species,
    const std::list<std::string>& characters,
    HDGraph& hasse);

/**
  @brief Add vertex with \e species and \e characters to \e hasse
  
  @param[in]     species    Species name
  @param[in]     characters List of character names
  @param[in,out] hasse      Hasse diagram graph
  
  @return Vertex descriptor for the new vertex
*/
inline HDVertex
add_vertex(
    const std::string& species,
    const std::list<std::string>& characters,
    HDGraph& hasse) {
  return add_vertex(std::list<std::string>({ species }), characters, hasse);
}

/**
  @brief Add edge between \e u and \e v with a list of signed characters to
         \e hasse
  
  @param[in]     u                Source Vertex
  @param[in]     v                Target Vertex
  @param[in]     signedcharacters List of signed characters
  @param[in,out] hasse            Hasse diagram graph
  
  @return Edge descriptor for the new edge.
          If the edge is already in the graph then a duplicate will not be
          added and the bool flag will be false.
          When the flag is false, the returned edge descriptor points to the
          already existing edge
*/
std::pair<HDEdge, bool>
add_edge(
    const HDVertex u,
    const HDVertex v,
    const std::list<SignedCharacter>& signedcharacters,
    HDGraph& hasse);


//=============================================================================
// General functions

/**
  @brief Overloading of operator<< for HDGraph
  
  @param[in] os    Output stream
  @param[in] hasse Hasse diagram graph
  
  @return Updated output stream
*/
std::ostream& operator<<(std::ostream& os, const HDGraph& hasse);


//=============================================================================
// Algorithm functions

/**
  @brief Returns True if \e a is included in \e b
  
  @param[in] a List of character names (strings)
  @param[in] b List of character names (strings)
  
  @return True if \e a is included in \e b, False otherwise
*/
bool
is_included(const std::list<std::string>& a, const std::list<std::string>& b);

/**
  @brief Build the Hasse diagram of \e g
  
  Let GM be a maximal reducible graph.
  Then the diagram P for GM is the Hasse diagram for the poset (Ps, ≤) of all
  species of GM ordered by the relation ≤, where s1 ≤ s2 if C(s1) ⊆ C(s2).
  Given (Ps, ≤) the poset of all inactive species of a red-black graph, we
  consider the representation of the poset (Ps, ≤) by its Hasse diagram,
  represented by a directed acyclic graph P.
  More precisely, two species s1 and s2 are connected by the arc (s1, s2) if
  s1 < s2 and there does not exist a species s3 such that s1 < s3 < s2.
  
  @param[out] hasse Hasse diagram graph
  @param[in]  g     Maximal reducible red-black graph
*/
void hasse_diagram(HDGraph& hasse, const RBGraph& g);

/**
  @brief Returns the vertex iterator of \e hasse if it's a source.
         Returns the end of the iterator if a source could not be found
  
  A source is a node of indegree 0.
  
  @param[in] v     Vertex iterator
  @param[in] v_end Vertex end iterator
  @param[in] hasse Hasse diagram graph
  
  @return Vertex iterator of \e hasse if it's a source, or \e v_end
*/
HDVertexIter
find_source(HDVertexIter v, HDVertexIter v_end, const HDGraph& hasse);

#endif // HDGRAPH_HPP
