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
    @param[in] v Vertex
    @param[in] g Red-black graph
    
    @return True if \e v is a singleton in \e g
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
    @param[in] cm Maximal characters
  */
  if_not_maximal(std::list<RBVertex> cm) : cm_(cm) {};
  
  /**
    @param[in] v Vertex
    @param[in] g Red-black graph
    
    @return True if \e v is not maximal character of \e g
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
public:
  /**
    @param[out] lsc  List of signed characters (which gets modified during the DFS)
    @param[in]  g    Red-black graph
    @param[in]  g_cm Maximal reducible red-black graph
  */
  safe_chain_dfs_visitor(
      std::list<SignedCharacter>& lsc,
      const RBGraph& g,
      const RBGraph& g_cm);
  
  /**
    @brief Invoked on every vertex of the graph before the start of the graph
           search
    
    @param[in] v     Vertex
    @param[in] hasse Hasse diagram graph
  */
  void initialize_vertex(const HDVertex v, const HDGraph& hasse) const;
  
  /**
    @brief Invoked on the source vertex once before the start of the search
    
    @param[in] v     Vertex
    @param[in] hasse Hasse diagram graph
  */
  void start_vertex(const HDVertex v, const HDGraph& hasse);
  
  /**
    @brief Invoked when a vertex is encountered for the first time
    
    @param[in] v     Vertex
    @param[in] hasse Hasse diagram graph
  */
  void discover_vertex(const HDVertex v, const HDGraph& hasse);
  
  /**
    @brief Invoked on every out-edge of each vertex after it is discovered
    
    @param[in] e     Edge
    @param[in] hasse Hasse diagram graph
  */
  void examine_edge(const HDEdge e, const HDGraph& hasse);
  
  /**
    @brief Invoked on each edge as it becomes a member of the edges that form
           the search tree
    
    @param[in] e     Edge
    @param[in] hasse Hasse diagram graph
  */
  void tree_edge(const HDEdge e, const HDGraph& hasse) const;
  
  /**
    @brief Invoked on the back edges in the graph
    
    @param[in] e     Edge
    @param[in] hasse Hasse diagram graph
  */
  void back_edge(const HDEdge e, const HDGraph& hasse) const;
  
  /**
    @brief Invoked on forward or cross edges in the graph
    
    @param[in] e     Edge
    @param[in] hasse Hasse diagram graph
  */
  void forward_or_cross_edge(const HDEdge e, const HDGraph& hasse) const;
  
  /**
    @brief Invoked on vertex \e v after finish_vertex has been called for all
           the vertices in the DFS-tree rooted at vertex \e v
    
    If vertex \e v is a leaf in the DFS-tree, then the finish_vertex function
    is called on \e v after all the out-edges of \e v have been examined.
    
    @param[in] v     Vertex
    @param[in] hasse Hasse diagram graph
  */
  void finish_vertex(const HDVertex v, const HDGraph& hasse);
  
private:
  std::list<SignedCharacter>& lsc_;
  const RBGraph g_;
  const RBGraph g_cm_;
  HDVertex source_v;
  HDVertex last_v;
};


//=============================================================================
// Boost functions (overloading)

// Red-Black Graph

/**
  @brief Remove \e v from \e g
  
  @param[in]     v Vertex
  @param[in,out] g Red-black graph
*/
void remove_vertex(const RBVertex v, RBGraph& g);

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

// Hasse Diagram

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

// Red-Black Graph

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
  @brief Overloading of operator<< for RBGraph
  
  @param[in] os Output stream
  @param[in] g  Red-black graph
  
  @return Updated output stream
*/
std::ostream& operator<<(std::ostream& os, const RBGraph& g);

/**
  @brief Generate a random red-black graph
  
  @param[out] g   Red-black graph
  @param[in]  S   Number of species
  @param[in]  C   Number of characters
  @param[in]  E   Number of edges
  @param[in]  gen Random number generator
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
  @brief Read from \e filename into \e g
  
  @param[in]  filename Filename
  @param[out] g        Red-black graph
*/
void read_graph(const std::string& filename, RBGraph& g);

// Hasse Diagram

/**
  @brief Overloading of operator<< for HDGraph
  
  @param[in] os    Output stream
  @param[in] hasse Hasse diagram graph
  
  @return Updated output stream
*/
std::ostream& operator<<(std::ostream& os, const HDGraph& hasse);


//=============================================================================
// Algorithm functions

// Red-Black Graph

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
  
  @param[out] components Vector of unique pointers to each subgraph
  @param[in]  g          Red-black graph
  
  @return Number of connected components
*/
size_t connected_components(RBGraphVector& components, const RBGraph& g);

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
  @brief Build the maximal reducible red-black graph of \e g given the maximal
         characters \e cm, removing non-maximal characters directly from \e g
  
  Let GRB be a red-black graph and CM the set of maximal characters of GRB.
  A maximal reducible graph consists of a reducible red-black graph whose
  characters are all maximal and inactive in the graph.
  Then GRB|CM is the maximal reducible graph induced by CM in graph GRB.
  
  @param[in]     cm List of maximal characters of \e g
  @param[in,out] g  Red-black graph (out: Maximal reducible red-black graph)
*/
void maximal_reducible_graph(const std::list<RBVertex>& cm, RBGraph& g);

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
  @brief Returns the vertex iterator of \e g if its name is \e name.
         Returns the end of the iterator if a vertex could not be found
  
  @param[in] v     Vertex iterator
  @param[in] v_end Vertex end iterator
  @param[in] name  Vertex name
  @param[in] g     Red-black graph
  
  @return Vertex iterator of \e g if its name is \e name, or \e v_end
*/
RBVertexIter
find_vertex(RBVertexIter v, RBVertexIter v_end,
            const std::string& name, const RBGraph& g);

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

/**
  @brief Check if \e g is a red Σ-graph
  
  A red-black graph containing a red Σ-graph cannot be reduce to an empty graph
  by a c-reduction.
  
  @param[in] g Red-black graph
  
  @return True if \e g is a red Σ-graph
*/
bool is_redsigma(const RBGraph& g);

/**
  @brief Check if \e v is a safe source in \e hasse
  
  Let GRB be a red-black graph, let P be the Hasse diagram for GRB|CM.
  A source s of a chain of P is safe for GRB if the realization of s in GRB
  does not induce red Σ-graphs in GRB.
  
  @param[in] v     Vertex
  @param[in] g     Red-black graph
  @param[in] hasse Hasse diagram graph
  
  @return True if \e v is a safe source in \e hasse
*/
bool safe_source(const HDVertex v, const RBGraph& g, const HDGraph& hasse);

/**
  @brief Returns a safe chain if \e hasse has a one
  
  Returns a safe chain and bool = True if \e hasse has a safe chain.
  Returns an empty chain and bool = False otherwise.
  
  Let GM be a maximal reducible red-black graph, let P be the Hasse diagram for
  GM, and let C be a chain of P.
  Then C is safe if the c-reduction S(C) of C is feasible for the graph GM and
  applying S(C) to GM results in a graph that has no red Σ-graph.
  
  @param[in] g     Red-black graph
  @param[in] g_cm  Maximal reducible red-black graph
  @param[in] hasse Hasse diagram graph
  
  @return Safe chain (list of signed characters).
          If \e hasse has a safe chain then the bool flag will be true.
          When the flag is false, the returned chain is empty
*/
std::pair<std::list<SignedCharacter>, bool>
safe_chain(const RBGraph& g, const RBGraph& g_cm, const HDGraph& hasse);


//=============================================================================
// Algorithm main functions

/**
  @brief Computes an extended c-reduction that is a successful reduction of a
         reducible graph
  
  A tree traversal of the positive characters of a tree T solving a graph GRB
  is a c-reduction R that is feasible for GRB and its application to GRB
  results in an empty graph.
  Then R is called a successful reduction for GRB.
  The extended c-reduction of R is the sequence of positive and negative
  characters obtained by the application of R to GRB.
  
  @param[in,out] g Red-black graph
  
  @return Realized characters (list of signed characters), that is a
          c-reduction of \e g
*/
std::list<SignedCharacter> reduce(RBGraph& g);

/**
  @brief Realize the character \e c (+ or -) in \e g
  
  Returns the list of realized characters and bool = True if the realization of
  \e sc is feasible for \e g.
  Returns an empty list and bool = False otherwise.
  
  Let GRB be a red-black graph, and let c be a character of GRB.
  Let D(c) be the set of species in the connected component of GRB that
  contains c.
  The result of the realization of c+ on GRB, which is defined only if c is
  inactive, is a red-black graph obtained from GRB by adding a red edge between
  c and each species in D(c) \ N(c), deleting all black edges incident on c,
  and finally deleting all isolated vertices.
  The realization of c− on GRB is defined only if c is active and there is no
  species in D(c) \ N(c): in this case the results of the realization is
  obtained from GRB by deleting all edges incident on c, and then deleting all
  isolated vertices.
  
  @param[in]     sc SignedCharacter of \e g
  @param[in,out] g  Red-black graph
  
  @return Realized characters (list of signed characters).
          If the realization was successful then the bool flag will be true.
          When the flag is false, the returned list is empty
*/
std::pair<std::list<SignedCharacter>, bool>
realize(const SignedCharacter& sc, RBGraph& g);

/**
  @brief Realize the inactive characters of the species \e v in \e g
  
  Returns the list of realized characters and bool = True if the realization of
  \e v is feasible for \e g.
  Returns an empty list and bool = False otherwise.
  
  The realization of a species s is the realization of its set C(s) of
  characters in any order. An active character c that is connected to all
  species of a graph GRB by red edges is called free in GRB and it is then
  deleted from GRB.
  
  @param[in]     v Vertex
  @param[in,out] g Red-black graph
  
  @return Realized characters (list of signed characters).
          If the realization was successful then the bool flag will be true.
          When the flag is false, the returned list is empty
*/
std::pair<std::list<SignedCharacter>, bool>
realize(const RBVertex v, RBGraph& g);

/**
  @brief Realize the list of characters \e lsc (+ or - each) in \e g
  
  Returns the list of realized characters and bool = True if the realizations
  of \e lsc are feasible for \e g.
  Returns an empty list and bool = False otherwise.
  
  @param[in]     lsc List of signed characters of \e g
  @param[in,out] g   Red-black graph
  
  @return Realized characters (list of signed characters).
          If the realizations were successful then the bool flag will be true.
          When the flag is false, the returned list is empty
*/
std::pair<std::list<SignedCharacter>, bool>
realize(const std::list<SignedCharacter>& lsc, RBGraph& g);

#endif
