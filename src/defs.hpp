#ifndef DEFS_HPP
#define DEFS_HPP

#include <boost/graph/adjacency_list.hpp>
#include <iostream>


//=============================================================================
// Constants

// Red-Black Graph

#define BLACK false
#define RED true

#define SPECIES false
#define CHARACTER true


//=============================================================================
// Bundled properties

//Red-Black Graph

/**
  @var color stores the edge color (RED or BLACK)
*/
struct EdgeProp {
  bool color = BLACK;
};

/**
  @var name stores the vertex name
  @var type stores the vertex type (CHARACTERS or SPECIES)
*/
struct VertexProp {
  std::string name = "v";
  bool type = SPECIES;
};

/**
  @var num_species stores the number of species in the graph
  @var num_characters stores the number of characters in the graph
*/
struct GraphProp {
  size_t num_species = 0;
  size_t num_characters = 0;
};


//=============================================================================
// Typedefs used for readabily

// Red-Black Graph

// Graph

typedef boost::adjacency_list<
  boost::listS,       // OutEdgeList
  boost::listS,       // VertexList
  boost::undirectedS, // Directed
  VertexProp,         // VertexProperties
  EdgeProp,           // EdgeProperties
  GraphProp           // GraphProperties
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
