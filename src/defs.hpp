#ifndef DEFS_HPP
#define DEFS_HPP

#include <boost/graph/adjacency_list.hpp>
#include <iostream>


//=============================================================================
// Data structuress

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

typedef boost::graph_traits<RBGraph>::edge_descriptor Edge;
typedef boost::graph_traits<RBGraph>::vertex_descriptor Vertex;

// Iterators

typedef boost::graph_traits<RBGraph>::edge_iterator EdgeIter;
typedef boost::graph_traits<RBGraph>::vertex_iterator VertexIter;

typedef boost::graph_traits<RBGraph>::out_edge_iterator OutEdgeIter;

typedef boost::graph_traits<RBGraph>::adjacency_iterator AdjIter;

// Size types

typedef boost::graph_traits<RBGraph>::vertices_size_type VertexSize;

// Index Maps

typedef std::map<Vertex, VertexSize> IndexMap;
typedef boost::associative_property_map<IndexMap> AssocMap;

// Containers

typedef std::vector< std::unique_ptr<RBGraph> > RBGraphVector;

#endif // DEFS_HPP
