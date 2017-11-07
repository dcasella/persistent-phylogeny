#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "defs.hpp"
#include <boost/graph/random.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/connected_components.hpp>


//=============================================================================
// Auxiliary structs and classes

/**
  @brief Functor used in remove_vertex_if
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
  @brief Functor used in remove_vertex_if
*/
struct if_not_maximal {
  /**
    @param cm Maximal characters
  */
  if_not_maximal(std::list<RBVertex> cm) : cm_(cm) {};
  
  /**
    @param v Vertex
    @param g Red-black graph
    @return  True if \c v is not maximal character of \c g
  */
  inline bool operator()(const RBVertex v, const RBGraph& g) const {
    return (std::find(cm_.begin(), cm_.end(), v) == cm_.end());
  }
  
private:
  
  std::list<RBVertex> cm_;
};

/**
  @brief DFS Visitor used in depth_first_search
*/
class safe_chain_dfs_visitor : public boost::default_dfs_visitor {
  std::list<SignedCharacter>& lsc_;
  const RBGraph g_;
  const RBGraph g_cm_;
  HDVertex source_v;
  HDVertex last_v;
  
public:
  /**
    @param lsc  List of SignedCharacters (which gets modified during the DFS)
    @param g    Red-black graph
    @param g_cm Maximal reducible red-black graph
  */
  safe_chain_dfs_visitor(
      std::list<SignedCharacter>& lsc,
      const RBGraph& g,
      const RBGraph& g_cm);
  
  /**
    @brief Invoked on every vertex of the graph before the start of the graph
           search
    
    @param v     Vertex
    @param hasse Hasse diagram graph
  */
  void initialize_vertex(const HDVertex v, const HDGraph& hasse) const;
  
  /**
    @brief Invoked on the source vertex once before the start of the search
    
    @param v     Vertex
    @param hasse Hasse diagram graph
  */
  void start_vertex(const HDVertex v, const HDGraph& hasse);
  
  /**
    @brief Invoked when a vertex is encountered for the first time
    
    @param v     Vertex
    @param hasse Hasse diagram graph
  */
  void discover_vertex(const HDVertex v, const HDGraph& hasse);
  
  /**
    @brief Invoked on every out-edge of each vertex after it is discovered
    
    @param e     Edge
    @param hasse Hasse diagram graph
  */
  void examine_edge(const HDEdge e, const HDGraph& hasse);
  
  /**
    @brief Invoked on each edge as it becomes a member of the edges that form
           the search tree
    
    @param e     Edge
    @param hasse Hasse diagram graph
  */
  void tree_edge(const HDEdge e, const HDGraph& hasse) const;
  
  /**
    @brief Invoked on the back edges in the graph
    
    @param e     Edge
    @param hasse Hasse diagram graph
  */
  void back_edge(const HDEdge e, const HDGraph& hasse) const;
  
  /**
    @brief Invoked on forward or cross edges in the graph
    
    @param e     Edge
    @param hasse Hasse diagram graph
  */
  void forward_or_cross_edge(const HDEdge e, const HDGraph& hasse) const;
  
  /**
    @brief Invoked on vertex \c v after finish_vertex has been called for all
           the vertices in the DFS-tree rooted at vertex \c v
    
    If vertex \c v is a leaf in the DFS-tree, then the finish_vertex function
    is called on \c v after all the out-edges of \c v have been examined
    
    @param v     Vertex
    @param hasse Hasse diagram graph
  */
  void finish_vertex(const HDVertex v, const HDGraph& hasse);
};


//=============================================================================
// Boost functions (overloading)

// Red-Black Graph

/**
  @brief Remove \c v from \c g
  
  @param v Vertex
  @param g Red-black graph
*/
void remove_vertex(const RBVertex v, RBGraph& g);

/**
  @brief Add vertex with name \c name and type \c type to \c g
  
  @param name Name
  @param type Type
  @param g    Red-black graph
  @return     Vertex descriptor for the new vertex
*/
RBVertex add_vertex(const std::string& name, const Type type, RBGraph& g);

/**
  @brief Add vertex with name \c name to \c g
  
  @param name Name
  @param g    Red-black graph
  @return     Vertex descriptor for the new vertex
*/
inline RBVertex add_vertex(const std::string& name, RBGraph& g) {
  return add_vertex(name, Type::none, g);
}

/**
  @brief Add edge between \c u and \c v with color \c color to \c g
  
  @param u     Source Vertex
  @param v     Target Vertex
  @param color Color
  @param g     Red-black graph
  @return      Edge descriptor for the new edge.
               If the edge is already in the graph then a duplicate will not be
               added and the bool flag will be false. When the flag is false,
               the returned edge descriptor points to the already existing edge
*/
std::pair<RBEdge, bool>
add_edge(const RBVertex u, const RBVertex v, const Color color, RBGraph& g);

// Hasse Diagram

/**
  @brief Add vertex with species \c species and characters \c characters to
         \c hasse
  
  @param species    List of species names
  @param characters List of character names
  @param hasse      Hasse diagram graph
  @return           Vertex descriptor for the new vertex
*/
HDVertex
add_vertex(
    const std::list<std::string>& species,
    const std::list<std::string>& characters,
    HDGraph& hasse);

/**
  @brief Add vertex with species \c species and characters \c characters to
         \c hasse
  
  @param species    Species name
  @param characters List of character names
  @param hasse      Hasse diagram graph
  @return           Vertex descriptor for the new vertex
*/
inline HDVertex
add_vertex(
    const std::string& species,
    const std::list<std::string>& characters,
    HDGraph& hasse) {
  return add_vertex(std::list<std::string>({ species }), characters, hasse);
}

/**
  @brief Add edge between \c u and \c v with list of SignedCharacters
         \c signedcharacters to \c hasse
  
  @param u                Source Vertex
  @param v                Target Vertex
  @param signedcharacters List of SignedCharacters
  @param hasse            Hasse diagram graph
  @return                 Edge descriptor for the new edge.
                          If the edge is already in the graph then a duplicate
                          will not be added and the bool flag will be false.
                          When the flag is false, the returned edge descriptor
                          points to the already existing edge
*/
std::pair<HDEdge, bool>
add_edge(
    const HDVertex u,
    const HDVertex v,
    const std::list<SignedCharacter>& signedcharacters,
    HDGraph& hasse);


//=============================================================================
// General functions

// Red-Black Graph

/**
  @brief Return the number of species in \c g
  
  @param g Red-black graph
  @return  Reference to the number of species in \c g
*/
inline size_t& num_species(RBGraph& g) {
  return g[boost::graph_bundle].num_species;
}

/**
  @brief Return the number of species (const) in \c g
  
  @param g Red-black graph
  @return  Constant number of species in \c g
*/
inline const size_t num_species(const RBGraph& g) {
  return g[boost::graph_bundle].num_species;
}

/**
  @brief Return the number of characters in \c g
  
  @param g Red-black graph
  @return  Reference to the number of characters in \c g
*/
inline size_t& num_characters(RBGraph& g) {
  return g[boost::graph_bundle].num_characters;
}

/**
  @brief Return the number of characters (const) in \c g
  
  @param g Red-black graph
  @return  Constant number of characters in \c g
*/
inline const size_t num_characters(const RBGraph& g) {
  return g[boost::graph_bundle].num_characters;
}

/**
  @brief Check if \c v is a species in \c g
  
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is a species in \c g
*/
inline bool is_species(const RBVertex v, const RBGraph& g) {
  return (g[v].type == Type::species);
}

/**
  @brief Check if \c v is a character in \c g
  
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is a character in \c g
*/
inline bool is_character(const RBVertex v, const RBGraph& g) {
  return (g[v].type == Type::character);
}

/**
  @brief Check if \c e is a black edge in \c g
  
  @param e Edge
  @param g Red-black graph
  @return  True if \c e is a black edge in \c g
*/
inline bool is_black(const RBEdge e, const RBGraph& g) {
  return (g[e].color == Color::black);
}

/**
  @brief Check if \c e is a red edge in \c g
  
  @param e Edge
  @param g Red-black graph
  @return  True if \c e is a red edge in \c g
*/
inline bool is_red(const RBEdge e, const RBGraph& g) {
  return (g[e].color == Color::red);
}

/**
  @brief Remove \c v from \c g if it satisfies \c pred
  
  @param v    Vertex
  @param g    Red-black graph
  @param pred Predicate
*/
template <typename Predicate>
void remove_vertex_if(const RBVertex v, Predicate pred, RBGraph& g) {
  if (pred(v, g)) {
    // vertex satisfies pred
    clear_vertex(v, g);
    remove_vertex(v, g);
  }
}

/**
  @brief Overloading of operator<< for RBGraph
  
  @param os Output stream
  @param g  Red-black graph
  @return   Updated output stream
*/
std::ostream& operator<<(std::ostream& os, const RBGraph& g);

/**
  @brief Generate a random Red-black graph
  
  @param g   Red-black graph
  @param S   Number of species
  @param C   Number of characters
  @param E   Number of edges
  @param gen Random number generator
*/
template <class RandNumGen>
void generate_random_rbgraph(
    RBGraph& g,
    const RBVertexSize S,
    const RBVertexSize C,
    const RBVertexSize E,
    RandNumGen& gen) {
  assert(E <= (S * C));
  
  generate_random_graph(g, (S + C), 0, gen, false, false);
  
  num_species(g) = S;
  num_characters(g) = C;
  
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  
  // name species in the graph
  for (RBVertexSize i = 0; i < S; ++i, ++v) {
    g[*v].name = "s" + std::to_string(i + 1);
    g[*v].type = Type::species;
  }
  
  // name characters in the graph
  for (RBVertexSize i = 0; i < C; ++i, ++v) {
    g[*v].name = "c" + std::to_string(i + 1);
    g[*v].type = Type::character;
  }
  
  // add edges between random species and characters
  for (RBVertexSize i = 0; i < E; ++i) {
    bool new_edge;
    RBVertex vs, vt;
    
    do {
      // search for a random vertex until vs holds a species
      while ((vs = random_vertex(g, gen)) && g[vs].type != Type::species);
      
      // search for a random vertex until vt holds a character
      while ((vt = random_vertex(g, gen)) && g[vt].type != Type::character);
      
      // add edge (if it doesn't exist)
      std::tie(std::ignore, new_edge) = add_edge(vs, vt, g);
    }
    while (!new_edge);
  }
  
  #ifdef DEBUG
  std::cout << "G generated" << std::endl << std::endl;
  #endif
}

// File I/O

/**
  @brief Read from \c filename into \c g
  
  @param filename Filename
  @param g        Red-black graph
*/
void read_graph(const std::string& filename, RBGraph& g);

// Hasse Diagram

/**
  @brief Overloading of operator<< for HDGraph
  
  @param os    Output stream
  @param hasse Hasse diagram graph
  @return      Updated output stream
*/
std::ostream& operator<<(std::ostream& os, const HDGraph& hasse);


//=============================================================================
// Algorithm functions

// Red-Black Graph

/**
  @brief Check if \c v is active in \c g
  
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is active in \c g
*/
bool is_active(const RBVertex v, const RBGraph& g);

/**
  @brief Check if \c v is inactive in \c g
  
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is inactive in \c g
*/
bool is_inactive(const RBVertex v, const RBGraph& g);

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
  @brief Check if \c v is free in \c g
  
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is free in \c g
*/
bool is_free(const RBVertex v, const RBGraph& g);

/**
  @brief Check if \c v is universal in \c g
  
  @param v Vertex
  @param g Red-black graph
  @return  True if \c v is universal in \c g
*/
bool is_universal(const RBVertex v, const RBGraph& g);

/**
  @brief Build the Red-black subgraphs of \c g.
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
inline bool
descending_size(const std::list<RBVertex>& a, const std::list<RBVertex>& b) {
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
  @brief Build the maximal reducible red-black graph of \c g given the maximal
         characters \c cm, removing non-maximal characters directly from \c g
  
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
inline bool
ascending_size(const std::list<RBVertex>& a, const std::list<RBVertex>& b) {
  return a.size() < b.size();
}

/**
  @brief Returns True if \c a is included in \c b
  
  @param a List of character names (strings)
  @param b List of character names (strings)
  @return  True if \c a is included in \c b, False otherwise
*/
bool
is_included(const std::list<std::string>& a, const std::list<std::string>& b);

/**
  @brief Build the Hasse diagram of \c g
  
  @param hasse Hasse diagram graph
  @param g     Red-black graph
*/
void hasse_diagram(HDGraph& hasse, const RBGraph& g);

/**
  @brief Returns the vertex iterator of \c g if its name is \c name.
         Returns the end of the iterator if a vertex could not be found
  
  @param v     Vertex iterator
  @param v_end Vertex end iterator
  @param name  Vertex name
  @param g     Red-black graph
  @return      Vertex iterator of \c g if its name is \c name, or \c v_end
*/
RBVertexIter
find_vertex(RBVertexIter v, RBVertexIter v_end,
            const std::string& name, const RBGraph& g);

/**
  @brief Returns the vertex iterator of \c hasse if it's a source.
         Returns the end of the iterator if a source could not be found
  
  @param v     Vertex iterator
  @param v_end Vertex end iterator
  @param hasse Hasse diagram graph
  @return      Vertex iterator of \c hasse if it's a source, or \c v_end
*/
HDVertexIter
find_source(HDVertexIter v, HDVertexIter v_end, const HDGraph& hasse);

/**
  @brief Check if \c g is a red Σ-graph
  
  @param g Red-black graph
  @return  True if \c g is a red Σ-graph
*/
bool is_redsigma(const RBGraph& g);

/**
  @brief Check if \c v is a safe source in \c hasse
  
  @param v     Vertex
  @param g     Red-black graph
  @param hasse Hasse diagram graph
  @return      True if \c v is a safe source in \c hasse
*/
bool safe_source(const HDVertex v, const RBGraph& g, const HDGraph& hasse);

/**
  @brief Returns a safe chain if \c hasse has a one
  
  ...
  Returns a safe chain and bool = True if \c hasse has a safe chain.
  Returns an empty chain and bool = False otherwise
  
  @param g     Red-black graph
  @param g_cm  Maximal reducible red-black graph
  @param hasse Hasse diagram graph
  @return      Safe chain (list of SignedCharacters).
               If \c hasse has a safe chain then the bool flag will be true.
               When the flag is false, the returned chain is empty
*/
std::pair<std::list<SignedCharacter>, bool>
safe_chain(const RBGraph& g, const RBGraph& g_cm, const HDGraph& hasse);


//=============================================================================
// Algorithm main functions

/**
  @brief ...
  
  @param g Red-black graph
  @return  c-reduction of \c g
*/
std::list<SignedCharacter> reduce(RBGraph& g);

/**
  @brief Realize the character \c c (+ or -) in \c g
  
  ...
  Returns the list of realized characters and bool = True if the realization of
  \c sc is feasible for \c g.
  Returns an empty list and bool = False otherwise
  
  @param sc SignedCharacter of \c g
  @param g  Red-black graph
  @return   Realized characters (list of SignedCharacters).
            If the realization was successful then the bool flag will be true.
            When the flag is false, the returned list is empty
*/
std::pair<std::list<SignedCharacter>, bool>
realize(const SignedCharacter& sc, RBGraph& g);

/**
  @brief Realize the inactive characters of the species \c v in \c g
  
  ...
  Returns the list of realized characters and bool = True if the realization of
  \c v is feasible for \c g.
  Returns an empty list and bool = False otherwise
  
  @param v Vertex
  @param g Red-black graph
  @return  Realized characters (list of SignedCharacters).
           If the realization was successful then the bool flag will be true.
           When the flag is false, the returned list is empty
*/
std::pair<std::list<SignedCharacter>, bool>
realize(const RBVertex v, RBGraph& g);

/**
  @brief Realize the list of characters \c lsc (+ or - each) in \c g
  
  ...
  Returns the list of realized characters and bool = True if the realizations
  of \c lsc are feasible for \c g.
  Returns an empty list and bool = False otherwise
  
  @param lsc List of SignedCharacters of \c g
  @param g   Red-black graph
  @return    Realized characters (list of SignedCharacters).
             If the realizations were successful then the bool flag will be
             true. When the flag is false, the returned list is empty
*/
std::pair<std::list<SignedCharacter>, bool>
realize(const std::list<SignedCharacter>& lsc, RBGraph& g);

#endif
