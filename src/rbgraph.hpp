#ifndef RBGRAPH_HPP
#define RBGRAPH_HPP

#include "globals.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <iostream>


//=============================================================================
// Forward declaration for typedefs

/**
  Red-black graph traits
*/
typedef boost::adjacency_list_traits<
  boost::setS,       // OutEdgeList
  boost::listS,      // VertexList
  boost::undirectedS // Directed
> RBTraits;

/**
  Bidirectional map of strings and vertices (red-black graph)
*/
typedef boost::bimap<
  boost::bimaps::unordered_set_of<std::string>,
  boost::bimaps::unordered_set_of<RBTraits::vertex_descriptor>
> RBVertexBimap;


//=============================================================================
// Data structures

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
  std::string name; ///< Vertex name
  Type type;        ///< Vertex type (Character or Species)
};

/**
  @brief Struct used to represent the properties of a red-black graph
*/
struct RBGraphProperties {
  size_t num_species = 0;    ///< Number of species in the graph
  size_t num_characters = 0; ///< Number of characters in the graph

  RBVertexBimap bimap; ///< Bidirectional map for vertex names and vertices in
                       ///< the graph
};


//=============================================================================
// Typedefs used for readabily

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

// Maps

/**
  Map of vertex indices (red-black graph)
*/
typedef std::map<RBVertex, RBVertexSize> RBVertexIMap;

/**
  Associative property map of vertex indices (red-black graph)
*/
typedef boost::associative_property_map<RBVertexIMap> RBVertexIAssocMap;

/**
  Map of vertices (red-black graph)
*/
typedef std::map<RBVertex, RBVertex> RBVertexMap;

/**
  Associative property map of vertices (red-black graph)
*/
typedef boost::associative_property_map<RBVertexMap> RBVertexAssocMap;

// Containers

/**
  Vector of unique pointers to red-black graphs
*/
typedef std::vector<std::unique_ptr<RBGraph>> RBGraphVector;


//=============================================================================
// Enum / Struct operator overloads

/**
  @brief Overloading of operator<< for Type

  @param[in] os Output stream
  @param[in] t  Type

  @return Updated output stream
*/
inline std::ostream& operator<<(std::ostream& os, const Type t) {
  return os << (t == Type::species ? "Species" : "Character");
}


//=============================================================================
// Auxiliary structs and classes

/**
  @brief Functor used in remove_vertex_if
*/
struct if_singleton {
  /**
    @brief Overloading of operator() for if_singleton

    @param[in] v Vertex
    @param[in] g Red-black graph

    @return True if \e v is a singleton in \e g
  */
  inline bool operator()(const RBVertex v, const RBGraph& g) const {
    return (out_degree(v, g) == 0);
  }
};

/**
  @brief Functor used in remove_vertex_if
*/
struct if_not_maximal {
  /**
    @brief Functor constructor

    @param[in] cm Maximal characters
  */
  if_not_maximal(const std::list<RBVertex>& cm) : m_cm(&cm) {};

  /**
    @brief Overloading of operator() for if_not_maximal

    @param[in] v Vertex
    @param[in] g Red-black graph

    @return True if \e v is not maximal character of \e g
  */
  inline bool operator()(const RBVertex v, const RBGraph& g) const {
    return (std::find(m_cm->begin(), m_cm->end(), v) == m_cm->end());
  }

private:
  const std::list<RBVertex>* m_cm;
};


//=============================================================================
// Boost functions (overloading)

/**
  @brief Remove \e v from \e g

  @param[in]     v Vertex
  @param[in,out] g Red-black graph
*/
void remove_vertex(const RBVertex v, RBGraph& g);

/**
  @brief Remove \e v from \e g

  @param[in]     v Vertex name
  @param[in,out] g Red-black graph
*/
void remove_vertex(const std::string& v, RBGraph& g);

/**
  @brief Add vertex with \e name and \e type to \e g

  @param[in]     name Name
  @param[in]     type Type
  @param[in,out] g    Red-black graph

  @return Vertex descriptor for the new vertex
*/
RBVertex add_vertex(const std::string& name, const Type type, RBGraph& g);

/**
  @brief Add vertex with \e name to \e g

  @param[in]     name Name
  @param[in,out] g    Red-black graph

  @return Vertex descriptor for the new vertex
*/
inline RBVertex add_vertex(const std::string& name, RBGraph& g) {
  return add_vertex(name, Type::species, g);
}

/**
  @brief Add edge between \e u and \e v with \e color to \e g

  @param[in]     u     Source Vertex
  @param[in]     v     Target Vertex
  @param[in]     color Color
  @param[in,out] g     Red-black graph

  @return Edge descriptor for the new edge.
          If the edge is already in the graph then a duplicate will not be
          added and the bool flag will be false.
          When the flag is false, the returned edge descriptor points to the
          already existing edge
*/
std::pair<RBEdge, bool>
add_edge(const RBVertex u, const RBVertex v, const Color color, RBGraph& g);

/**
  @brief Add edge between \e u and \e v to \e g

  @param[in]     u     Source Vertex
  @param[in]     v     Target Vertex
  @param[in,out] g     Red-black graph

  @return Edge descriptor for the new edge.
          If the edge is already in the graph then a duplicate will not be
          added and the bool flag will be false.
          When the flag is false, the returned edge descriptor points to the
          already existing edge
*/
inline std::pair<RBEdge, bool>
add_edge(const RBVertex u, const RBVertex v, RBGraph& g) {
  return add_edge(u, v, Color::black, g);
}


//=============================================================================
// General functions

/**
  @brief Return the number of species in \e g

  @param[in] g Red-black graph

  @return Reference to the number of species in \e g
*/
inline size_t& num_species(RBGraph& g) {
  return g[boost::graph_bundle].num_species;
}

/**
  @brief Return the number of species (const) in \e g

  @param[in] g Red-black graph

  @return Constant number of species in \e g
*/
inline const size_t num_species(const RBGraph& g) {
  return g[boost::graph_bundle].num_species;
}

/**
  @brief Return the number of characters in \e g

  @param[in] g Red-black graph

  @return Reference to the number of characters in \e g
*/
inline size_t& num_characters(RBGraph& g) {
  return g[boost::graph_bundle].num_characters;
}

/**
  @brief Return the number of characters (const) in \e g

  @param[in] g Red-black graph

  @return Constant number of characters in \e g
*/
inline const size_t num_characters(const RBGraph& g) {
  return g[boost::graph_bundle].num_characters;
}

/**
  @brief Return the bidirectional map in \e g

  @param[in] g Red-black graph

  @return Reference to the bidirectional map in \e g
*/
inline RBVertexBimap& bimap(RBGraph& g) {
  return g[boost::graph_bundle].bimap;
}

/**
  @brief Return the bidirectional map (const) in \e g

  @param[in] g Red-black graph

  @return Constant bidirectional map in \e g
*/
inline const RBVertexBimap bimap(const RBGraph& g) {
  return g[boost::graph_bundle].bimap;
}

/**
  @brief Check if \e v is a species in \e g

  @param[in] v Vertex
  @param[in] g Red-black graph

  @return True if \e v is a species in \e g
*/
inline bool is_species(const RBVertex v, const RBGraph& g) {
  return (g[v].type == Type::species);
}

/**
  @brief Check if \e v is a character in \e g

  @param[in] v Vertex
  @param[in] g Red-black graph

  @return True if \e v is a character in \e g
*/
inline bool is_character(const RBVertex v, const RBGraph& g) {
  return (g[v].type == Type::character);
}

/**
  @brief Check if \e e is a black edge in \e g

  @param[in] e Edge
  @param[in] g Red-black graph

  @return True if \e e is a black edge in \e g
*/
inline bool is_black(const RBEdge e, const RBGraph& g) {
  return (g[e].color == Color::black);
}

/**
  @brief Check if \e e is a red edge in \e g

  @param[in] e Edge
  @param[in] g Red-black graph

  @return True if \e e is a red edge in \e g
*/
inline bool is_red(const RBEdge e, const RBGraph& g) {
  return (g[e].color == Color::red);
}

/**
  @brief Remove \e v from \e g if it satisfies \c predicate

  @param[in]     v         Vertex
  @param[in,out] g         Red-black graph
  @param[in]     predicate Predicate
*/
template <typename Predicate>
void remove_vertex_if(const RBVertex v, Predicate predicate, RBGraph& g) {
  if (predicate(v, g)) {
    // vertex satisfies the predicate
    clear_vertex(v, g);
    remove_vertex(v, g);
  }
}

/**
  @brief Build the bimap in \c g

  @param[in] g Red-black graph
*/
void build_bimap(RBGraph& g);

/**
  @brief Return the vertex descriptor of the vertex named \c v in \c g

  @param[in] v Vertex name
  @param[in] g Red-black graph

  @return Vertex
*/
inline RBVertex get_vertex(const std::string& v, const RBGraph& g) {
  return bimap(g).left.at(v);
}

/**
  @brief Copy graph \c g to graph \c g_copy

  @param[in]     g      Red-black graph
  @param[in,out] g_copy Red-black graph
*/
void copy_graph(const RBGraph& g, RBGraph& g_copy);

/**
  @brief Copy graph \c g to graph \c g_copy and fill its vertex map

  @param[in]     g      Red-black graph
  @param[in,out] g_copy Red-black graph
  @param[in,out] v_map  Vertex map, mapping vertices from g to g_copy
*/
void copy_graph(const RBGraph& g, RBGraph& g_copy, RBVertexMap& v_map);

/**
  @brief Overloading of operator<< for RBGraph

  @param[in] os Output stream
  @param[in] g  Red-black graph

  @return Updated output stream
*/
std::ostream& operator<<(std::ostream& os, const RBGraph& g);

// File I/O

/**
  @brief Read from \e filename into \e g

  @param[in]  filename Filename
  @param[out] g        Red-black graph
*/
void read_graph(const std::string& filename, RBGraph& g);


//=============================================================================
// Algorithm functions

/**
  @brief Check if \e v is active in \e g

  A vertex is active in a red-black graph if it's a character that is incident
  only on red edges.

  @param[in] v Vertex
  @param[in] g Red-black graph

  @return True if \e v is active in \e g
*/
bool is_active(const RBVertex v, const RBGraph& g);

/**
  @brief Check if \e v is inactive in \e g

  A vertex is inactive in a red-black graph if it's a character that is
  incident only on black edges.

  @param[in] v Vertex
  @param[in] g Red-black graph

  @return True if \e v is inactive in \e g
*/
bool is_inactive(const RBVertex v, const RBGraph& g);

/**
  @brief Remove singleton vertices from \e g

  A vertex is a singleton in a red-black graph if it's incident on no edges.

  @param[in,out] g Red-black graph
*/
void remove_singletons(RBGraph& g);

/**
  @brief Check if \e g is empty

  A red-black graph is empty if it has no vertices.

  @param[in] g Red-black graph

  @return True if \e g is empty
*/
inline bool is_empty(const RBGraph& g) {
  return (num_vertices(g) == 0);
}

/**
  @brief Check if \e v is free in \e g

  A vertex is free in a red-black graph if it's an active character that is
  connected to all species of the graph by red dges.

  @param[in] v Vertex
  @param[in] g Red-black graph

  @return True if \e v is free in \e g
*/
bool is_free(const RBVertex v, const RBGraph& g);

/**
  @brief Check if \e v is universal in \e g

  A vertex is free in a red-black graph if it's an inactive character that is
  connected to all species of the graph by black dges.

  @param[in] v Vertex
  @param[in] g Red-black graph

  @return True if \e v is universal in \e g
*/
bool is_universal(const RBVertex v, const RBGraph& g);

/**
  @brief Build the red-black subgraphs of \e g.
         Each subgraph is a copy of the respective connected component

  If the graph \c g is connected, RBGraphVector will be of size 1, but the
  unique_ptr will be empty. This is because the purpose of the functions is to
  build the subgraphs, not copy the whole graph when it isn't needed.

  @param[in]  g          Red-black graph

  @return components Vector of unique pointers to each subgraph
*/
RBGraphVector connected_components(const RBGraph& g);

/**
  @brief Build the list of maximal characters of \e g

  Let c be an unsigned character.
  Then S(c) is the set of species that have the character c.
  Given two characters c1 and c2, we will say that c1 includes c2 if
  S(c1) ⊇ S(c2).
  Then a character c is maximal in a red-black graph if S(c) ⊄ S(c') for any
  character c' of the graph.
  Moreover two characters c, c' overlap if they share a common species
  but neither is included in the other.

  @param[in] g Red-black graph

  @return Maximal characters (vertices) of \e g
*/
std::list<RBVertex> maximal_characters(const RBGraph& g);

/**
  @brief Predicate used to sort a vector of lists

  @param[in] a List of vertices
  @param[in] b List of vertices

  @return True if \e a has more elements than \e b
*/
inline bool
descending_size(const std::list<RBVertex>& a, const std::list<RBVertex>& b) {
  return a.size() > b.size();
}

/**
  @brief Build the list of maximal characters of \e g

  Let c be an unsigned character.
  Then S(c) is the set of species that have the character c.
  Given two characters c1 and c2, we will say that c1 includes c2 if
  S(c1) ⊇ S(c2).
  Then a character c is maximal in a red-black graph if S(c) ⊄ S(c') for any
  character c' of the graph.
  Moreover two characters c, c' overlap if they share a common species
  but neither is included in the other.

  @param[in] g Red-black graph

  @return Maximal characters (vertices) of \e g
*/
std::list<RBVertex> maximal_characters2(const RBGraph& g);

/**
  @brief Build the maximal reducible red-black graph of \e gm

  Let GRB be a red-black graph and CM the set of maximal characters of GRB.
  A maximal reducible graph consists of a reducible red-black graph whose
  characters are all maximal and inactive in the graph.
  Then GRB|CM is the maximal reducible graph induced by CM in graph GRB.

  @param[in] gm Red-black graph

  @return Maximal reducible graph
*/
RBGraph maximal_reducible_graph(const RBGraph& gm);

/**
  @brief Predicate used to sort a vector of lists of vertices

  @param[in] a List of vertices
  @param[in] b List of vertices

  @return True if \e a has less elements than \e b
*/
inline bool
ascending_size(const std::list<RBVertex>& a, const std::list<RBVertex>& b) {
  return a.size() < b.size();
}

/**
  @brief Check if \e g is a red Σ-graph

  A red-black graph containing a red Σ-graph cannot be reduce to an empty graph
  by a c-reduction.

  @param[in] g Red-black graph

  @return True if \e g is a red Σ-graph
*/
bool is_redsigma(const RBGraph& g);

#endif // RBGRAPH_HPP
