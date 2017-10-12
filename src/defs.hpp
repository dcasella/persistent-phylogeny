#ifndef DEFS_HPP
#define DEFS_HPP

#include <boost/graph/adjacency_list.hpp>
#include <iostream>


//=============================================================================
// Data structuress

// General

/**
  @brief Strongly typed boolean enum class used for character state (- or +)
*/
enum class State : unsigned char {
  none, lose, gain
};

// Red-Black Graph

/**
  @brief Strongly typed boolean enum class used for edge color
*/
enum class Color : bool {
  black, red
};

/**
  @brief Strongly typed boolean enum class used for vertex type
*/
enum class Type : bool {
  species, character
};


//=============================================================================
// Bundled properties

// Red-Black Graph

/**
  @var color stores the edge color (Red or Black)
*/
struct RBEdgeProperties {
  Color color = Color::black;
};

/**
  @var name stores the vertex name
  @var type stores the vertex type (Character or Species)
*/
struct RBVertexProperties {
  std::string name = "v";
  Type type = Type::species;
};

/**
  @var num_species stores the number of species in the graph
  @var num_characters stores the number of characters in the graph
*/
struct RBGraphProperties {
  size_t num_species = 0;
  size_t num_characters = 0;
};

// Hasse Diagram

/**
  @var character stores the character name which lables the edge
  @var state stores the character state
*/
struct HDEdgeProperties {
  std::string character = "c";
  State state = State::none;
};

/**
  @var vertices stores the list of vertices which labels the vertex
*/
struct HDVertexProperties {
  std::list<std::string> vertices;
  std::list<std::string> characters;
};

struct HDGraphProperties {
  
};


//=============================================================================
// Typedefs used for readabily

// Red-Black Graph

// Graph

typedef boost::adjacency_list<
  boost::setS,        // OutEdgeList
  boost::listS,       // VertexList
  boost::undirectedS, // Directed
  RBVertexProperties, // VertexProperties
  RBEdgeProperties,   // EdgeProperties
  RBGraphProperties   // GraphProperties
> RBGraph;

// Descriptors

typedef boost::graph_traits<RBGraph>::edge_descriptor RBEdge;
typedef boost::graph_traits<RBGraph>::vertex_descriptor RBVertex;

// Iterators

typedef boost::graph_traits<RBGraph>::edge_iterator RBEdgeIter;
typedef boost::graph_traits<RBGraph>::vertex_iterator RBVertexIter;

typedef boost::graph_traits<RBGraph>::out_edge_iterator RBOutEdgeIter;

typedef boost::graph_traits<RBGraph>::adjacency_iterator RBAdjIter;

// Size types

typedef boost::graph_traits<RBGraph>::vertices_size_type RBVertexSize;

// Index Maps

typedef std::map<RBVertex, RBVertexSize> IndexMap;
typedef boost::associative_property_map<IndexMap> AssocMap;

// Containers

typedef std::vector< std::unique_ptr<RBGraph> > RBGraphVector;

// Hasse Diagram

// Graph

typedef boost::adjacency_list<
  boost::setS,        // OutEdgeList
  boost::listS,       // VertexList
  boost::directedS,   // Directed
  HDVertexProperties, // VertexProperties
  HDEdgeProperties,   // EdgeProperties
  HDGraphProperties   // GraphProperties
> HDGraph;

// Descriptors

typedef boost::graph_traits<HDGraph>::edge_descriptor HDEdge;
typedef boost::graph_traits<HDGraph>::vertex_descriptor HDVertex;

// Iterators

typedef boost::graph_traits<HDGraph>::edge_iterator HDEdgeIter;
typedef boost::graph_traits<HDGraph>::vertex_iterator HDVertexIter;

typedef boost::graph_traits<HDGraph>::out_edge_iterator HDOutEdgeIter;

typedef boost::graph_traits<HDGraph>::adjacency_iterator HDAdjIter;

// Size types

typedef boost::graph_traits<HDGraph>::vertices_size_type HDVertexSize;

#endif // DEFS_HPP