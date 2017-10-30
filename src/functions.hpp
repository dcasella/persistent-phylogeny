#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "defs.hpp"
#include <boost/graph/connected_components.hpp>


//=============================================================================
// General functions

// Red-Black Graph

/**
  @brief Remove \c v from \c g if it satisfies \c pred
  @param v    Vertex
  @param g    Red-black graph
  @param pred Predicate
*/
template <typename Predicate>
void remove_vertex_if(const RBVertex& v, Predicate pred, RBGraph& g) {
  if (pred(v, g)) {
    // vertex satisfies pred
    if (g[v].type == Type::species)
      g[boost::graph_bundle].num_species--;
    else if (g[v].type == Type::character)
      g[boost::graph_bundle].num_characters--;
    
    clear_vertex(v, g);
    remove_vertex(v, g);
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
void read_graph(const std::string& filename, RBGraph& g);

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
  @brief Check if \c v is active
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is active
*/
bool is_active(const RBVertex v, const RBGraph& g);

/**
  @brief Check if \c v is inactive
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is inactive
*/
bool is_inactive(const RBVertex v, const RBGraph& g);

/**
  @brief Functor used in remove_vertex_if
         ...
*/
struct if_singleton {
  /**
    @param v Vertex
    @param g Red-black graph
    @return  True if \c v is a singleton in \c g
  */
  inline bool operator()(const RBVertex v, RBGraph& g) const {
    return (out_degree(v, g) == 0);
  }
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
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is free
*/
bool is_free(const RBVertex v, const RBGraph& g);

/**
  @brief Check if \c v is universal
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is universal
*/
bool is_universal(const RBVertex v, const RBGraph& g);

/**
  @brief Build the Red-black subgraphs of \c g
         Each subgraph is a copy of the respective connected component
  @param components Vector of unique pointers to each subgraph
  @param g          Red-black graph
  @return           Number of connected components
*/
size_t connected_components(RBGraphVector& components, const RBGraph& g);

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
    @param v Vertex
    @param g Red-black graph
    @return  True if \c v is not maximal character of \c g
  */
  inline bool operator()(const RBVertex v, const RBGraph& g) const {
    return (std::find(cm.begin(), cm.end(), v) == cm.end());
  }
  
private:
  
  std::list<RBVertex> cm;
};

/**
  @brief Build the maximal reducible graph of \c g given the maximal characters
         \c cm, removing non-maximal characters directly from \c g
  @param cm List of maximal characters of \c g
  @param g  Red-black graph
*/
void maximal_reducible_graph(const std::list<RBVertex>& cm, RBGraph& g);

/**
  @brief Predicate used to sort a vector of lists of vertices
  @param a List of vertices
  @param b List of vertices
  @return  True if \c a has less elements than \c b
*/
inline bool ascending_size(const std::list<RBVertex>& a,
                           const std::list<RBVertex>& b) {
   return a.size() < b.size();
}

/**
  @brief Returns True if \c a is included in \c b
  @param a List of character names (strings)
  @param b List of character names (strings)
  @return  True if \c a is included in \c b, False otherwise
*/
bool is_included(const std::list<std::string>& a,
                 const std::list<std::string>& b);

/**
  @brief Build the Hasse diagram of \c g
  @param hasse Hasse diagram graph
  @param g     Red-black graph
*/
void hasse_diagram(HDGraph& hasse, const RBGraph& g);

/**
  @brief Returns the vertex iterator of \c g if its name is \c name
         Returns the end of the iterator if a vertex could not be found
  @param v     Vertex iterator
  @param v_end Vertex end iterator
  @param name  Vertex name
  @param g     Red-black graph
  @return      Vertex iterator of \c g if its name is \c name, or \c v_end
*/
RBVertexIter find_vertex(RBVertexIter v, RBVertexIter v_end,
                         const std::string& name, const RBGraph& g);

/**
  @brief Returns the vertex iterator of \c hasse if it's a source
         Returns the end of the iterator if a source could not be found
  @param v     Vertex iterator
  @param v_end Vertex end iterator
  @param hasse Hasse diagram graph
  @return      Vertex iterator of \c hasse if it's a source, or \c v_end
*/
HDVertexIter find_source(HDVertexIter v, HDVertexIter v_end,
                         const HDGraph& hasse);

/**
  @brief Check if \c g is a red Σ-graph
  @param g Red-black graph
  @return  True if \c g is a red Σ-graph
*/
bool is_redsigma(const RBGraph& g);

/**
  @brief Returns bool = True and a safe chain if \c hasse has a safe source
         Returns bool = False if \c hasse has no safe source
  @param g     Red-black graph
  @param g_cm  Red-black maximal reducible graph
  @param hasse Hasse diagram graph
  @return      Pair consisting in a safe chain and the outcome of the function
               A safe chain is represented as a list of CharacterStates
*/
std::pair<std::list<CharacterState>, bool> safe_chain(const RBGraph& g,
                                                      const RBGraph& g_cm,
                                                      const HDGraph& hasse);


//=============================================================================
// Algorithm main functions

/**
  @brief ...
  @param g Red-black graph
  @return  c-reduction of \c g
*/
std::list<CharacterState> reduce(RBGraph& g);

/**
  @brief Realize the character \c c (+ or -) in \c g
  @param c Character name of \c g and state
  @param g Red-black graph
  @return  True if the realization of \c c is feasible for g
*/
bool realize(const CharacterState& c, RBGraph& g);

/**
  @brief Realize the inactive characters of the species \c v in \c g
  @param v Vertex
  @param g Red-black graph
  @return  True if the realization of \c v is feasible for g
*/
bool realize(const RBVertex& v, RBGraph& g);

/**
  @brief Realize the list of characters \c lc (+ or - each) in \c g
  @param lc List of character names of \c g and states
  @param g  Red-black graph
  @return   True if the realizations of \c lc are feasible for g
*/
bool realize(const std::list<CharacterState>& lc, RBGraph& g);

#endif
