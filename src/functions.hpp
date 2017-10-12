#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "defs.hpp"
#include <boost/graph/connected_components.hpp>


//=============================================================================
// General functions

// Red-Black Graph

/**
  @brief Remove \c v from \c g if it satisfies \c pred
  @param v    RBVertex
  @param g    Red-black graph
  @param pred Predicate
*/
template <typename Predicate>
void remove_vertex_if(const RBVertex v, Predicate pred, RBGraph& g) {
  if (pred(v, g)) {
    // vertex satisfies pred
    clear_vertex(v, g);
    remove_vertex(v, g);
    
    if (g[v].type == Type::species)
      g[boost::graph_bundle].num_species--;
    else
      g[boost::graph_bundle].num_characters--;
  }
}

/**
  @brief Print \c g
  @param g Red-black graph
*/
void print_rbgraph(const RBGraph& g);

// File I/O

/**
  @brief Read from \c filename into \c g
  @param filename Filename
  @param g        Red-black graph
*/
void read_graph(const std::string filename, RBGraph& g);

// std::tuple<size_t, size_t>
// read_matrix(const std::string filename, std::vector<bool>& m);

// Hasse Diagram

/**
  @brief Print \c g
  @param g Hasse diagram graph
*/
void print_hdgraph(const HDGraph& g);


//=============================================================================
// Algorithm functions

// Red-Black Graph

/**
  @brief Functor used in remove_vertex_if
         ...
*/
struct if_singleton {
  /**
    @param v RBVertex
    @param g Red-black graph
    @return  True if \c v is a singleton in \c g
  */
  bool operator()(const RBVertex v, RBGraph& g) const;
};

/**
  @brief Remove singleton vertices from \c g
  @param g Red-black graph
*/
void remove_singletons(RBGraph& g);

/**
  @brief Check if \c g is empty
  @param g Red-black graph
  @return  True if \c g is empty
*/
inline bool is_empty(const RBGraph& g) {
  return (num_vertices(g) == 0);
}

/**
  @brief Check if \c v is free
  @param v RBVertex
  @param g Red-black graph
  @return  True if \c v is free
*/
bool is_free(const RBVertex v, const RBGraph& g);

/**
  @brief Check if \c v is universal
  @param v RBVertex
  @param g Red-black graph
  @return  True if \c v is universal
*/
bool is_universal(const RBVertex v, const RBGraph& g);

/**
  @brief Build the Red-black subgraphs of \c g
         Each subgraph is a copy of the respective connected component
  @param g          Red-black graph
  @param components Vector of unique pointers to each subgraph
  @return           Number of connected components
*/
size_t connected_components(const RBGraph& g, RBGraphVector& components);

/**
  @brief Build the list of maximal characters of \c g
  ...
  A character is maximal in \c g if ...
  @param g Red-black graph
  @return  Maximal characters (vertices) of \c g
*/
std::list<RBVertex> maximal_characters(const RBGraph& g);

/**
  @brief Predicate used to sort a vector of lists
  @param a List of vertices
  @param b List of vertices
  @return  True if \c a has more elements than \c b
*/
inline bool descending_size(const std::list<RBVertex>& a,
                            const std::list<RBVertex>& b) {
   return a.size() > b.size();
}

/**
  @brief Build the list of maximal characters of \c g
  ...
  A character is maximal in \c g if ...
  @param g Red-black graph
  @return  Maximal characters (vertices) of \c g
*/
std::list<RBVertex> maximal_characters2(const RBGraph& g);

/**
  @brief Functor used in remove_vertex_if
         ...
*/
struct if_not_maximal {
  if_not_maximal(std::list<RBVertex> cm_) : cm(cm_) {};
  
  /**
    @param v RBVertex
    @param g Red-black graph
    @return  True if \c v is not maximal character of \c g
  */
  bool operator()(const RBVertex v, const RBGraph& g) const;
  
private:
  
  std::list<RBVertex> cm;
};

/**
  @brief Build the maximal reducible graph of \c g given the maximal characters
         \c cm, removing non-maximal characters directly from \c g
  @param g  Red-black graph
  @param cm List of maximal characters of \c g
*/
void maximal_reducible_graph(RBGraph& g, const std::list<RBVertex>& cm);

/**
  @brief Predicate used to sort a vector of lists
  @param a List of vertices
  @param b List of vertices
  @return  True if \c a has more elements than \c b
*/
inline bool ascending_size(const std::list<RBVertex>& a,
                           const std::list<RBVertex>& b) {
   return a.size() < b.size();
}

/**
  @brief Build the Hasse diagram of \c g
  @param g     Red-black graph
  @param hasse Hasse diagram graph
*/
void hasse_diagram(const RBGraph& g, HDGraph& hasse);


//=============================================================================
// Algorithm main functions

/**
  @brief ...
  @param g Red-black graph
  @return  c-reduction of \c g
*/
std::list<std::string> reduce(RBGraph& g);

#endif