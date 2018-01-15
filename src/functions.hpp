#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "globals.hpp"
#include "rbgraph.hpp"
#include "hdgraph.hpp"


//=============================================================================
// Auxiliary structs and classes

/**
  @brief Safe chain DFS Visitor exception

  Thrown when \c initial_state_visitor finds a safe chain with a safe source
*/
class InitialState : public std::exception {
public:
  /**
    @brief Returns the reason of the exception

    @return C String
  */
  inline const char* what() const throw() {
    return "Found initial state";
  }
};

/**
  @brief Reduce exception

  Thrown when \c reduce can't reduce the graph anymore
*/
class NoReduction : public std::exception {
public:
  /**
    @brief Returns the reason of the exception

    @return C String
  */
  inline const char* what() const throw() {
    return "Could not reduce graph";
  }
};

/**
  @brief DFS Visitor used in depth_first_search
*/
class initial_state_visitor : public boost::default_dfs_visitor {
public:
  /**
    @brief DFS Visitor default constructor
  */
  initial_state_visitor();

  /**
    @brief DFS Visitor constructor

    @param[out] sources Vertices representing the safe sources of the diagram
  */
  initial_state_visitor(std::list<HDVertex>& sources);

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
  void forward_or_cross_edge(const HDEdge e, const HDGraph& hasse);

  /**
    @brief Invoked on vertex \e v after finish_vertex has been called for all
           the vertices in the DFS-tree rooted at vertex \e v

    If vertex \e v is a leaf in the DFS-tree, then the finish_vertex function
    is called on \e v after all the out-edges of \e v have been examined.

    @param[in] v     Vertex
    @param[in] hasse Hasse diagram graph
  */
  void finish_vertex(const HDVertex v, const HDGraph& hasse);

  /**
    @brief Test if \e lsc is a safe chain with \e source_v as active safe
           source in \e hasse

    Call \e safe_chain to check if \e lsc is a safe chain. If it is, call \e
    safe_source to check if \e source_v (the source of the chain \e lsc) is a
    safe source. If it is, add it to the list of safe sources.
    This function throws a \e InitialState exception if \e interactive::enabled
    is disabled and \e source_v is a safe source.

    @param[in] hasse Hasse diagram graph
  */
  void perform_test(const HDGraph& hasse);

  /**
    @brief Test if \e source_v is a safe source in the degenerate diagram \e
           hasse

    Let GRB be a reducible graph such that GM has a degenerate diagram P.
    Then a safe source s of P is good for GRB if
    (1) there exists a species s0 of P whose inactive characters are those of
        a species s of GRB and s does not a character d that is in conflict
        with a minimal a such that a∪{si} for any other source of P is
        included in a species of GRB
    (2) or none of the sources of P induces a species of GRB.

    @param[in] hasse Hasse diagram graph
  */
  void perform_test_degenerate(const HDGraph& hasse);

  /**
    @brief Check if \e lsc is a safe chain in \e hasse

    Let GRB be a red-black graph, let P be the Hasse diagram for GRB|CM and let
    C be a chain of P.
    Then C is safe if the c-reduction S(C) of C is feasible for the graph and
    applying S(C) to GRB results in a graph that has no red Σ-graphs.

    @param[in] hasse Hasse diagram graph

    @return True if \e lsc is a safe chain in \e hasse
  */
  bool safe_chain(const HDGraph& hasse);

  /**
    @brief Check if \e v is a safe source in \e hasse

    Let GRB be a red-black graph and let P be the Hasse diagram for GRB|CM.
    A source s of a safe chain C of P is safe for GRB if there exists a species
    s' in GRB|CM∪A that consists of C(s) and all active characters in GRB and
    the realization of C(s') in GRB does not induce red Σ-graphs in GRB.
    Then s' is called the active safe source for GRB.

    @param[in] v     Vertex
    @param[in] hasse Hasse diagram graph

    @return True if \e v is a safe source in \e hasse
  */
  bool safe_source(const RBVertex v, const HDGraph& hasse);

private:
  std::list<HDVertex>* m_sources;
  std::list<SignedCharacter> lsc;
  HDVertex source_v;
  HDVertex last_v;
  HDVertex last_safesource;
};


//=============================================================================
// Algorithm functions

/**
  @brief Returns a list of safe sources

  Let GM be a maximal reducible red-black graph, let P be the Hasse diagram for
  GM, and let C be a chain of P.
  Then C is safe if the c-reduction S(C) of C is feasible for the graph GM and
  applying S(C) to GM results in a graph that has no red Σ-graph.

  The source s of a safe chain C is the initial state of a tree T solving GRB
  if s is safe.

  @param[in] hasse Hasse diagram graph

  @return List of safe sources
*/
std::list<HDVertex> initial_states(const HDGraph& hasse);


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
