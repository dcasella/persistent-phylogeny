#include "functions.hpp"
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/connected_components.hpp>


//=============================================================================
// Auxiliary structs and classes

safe_chain_dfs_visitor::safe_chain_dfs_visitor()
    : m_lsc(nullptr), source_v(0), last_v(0) {}

safe_chain_dfs_visitor::safe_chain_dfs_visitor(std::list<SignedCharacter>& lsc)
    : m_lsc(&lsc), source_v(0), last_v(0) {
  m_lsc->clear();
}

void
safe_chain_dfs_visitor::initialize_vertex(
    const HDVertex v,
    const HDGraph& hasse) const {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "initialize_vertex: [ ";

    StringIter kk = hasse[v].species.begin();
    for (; kk != hasse[v].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  // ignore
}

void
safe_chain_dfs_visitor::start_vertex(const HDVertex v, const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "start_vertex: [ ";

    StringIter kk = hasse[v].species.begin();
    for (; kk != hasse[v].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  source_v = v;
}

void
safe_chain_dfs_visitor::discover_vertex(
    const HDVertex v,
    const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "discover_vertex: [ ";

    StringIter kk = hasse[v].species.begin();
    for (; kk != hasse[v].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  last_v = v;
}

void
safe_chain_dfs_visitor::examine_edge(const HDEdge e, const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    HDVertex vs, vt;
    std::tie(vs, vt) = incident(e, hasse);

    std::cout << "examine_edge: [ ";

    StringIter kk = hasse[vs].species.begin();
    for (; kk != hasse[vs].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "] -";

    SignedCharacterIter jj = hasse[e].signedcharacters.begin();
    for (; jj != hasse[e].signedcharacters.end(); ++jj) {
      std::cout << *jj;

      if (std::next(jj) != hasse[e].signedcharacters.end())
        std::cout << ",";
    }

    std::cout << "-> [ ";

    kk = hasse[vt].species.begin();
    for (; kk != hasse[vt].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  if (m_lsc == nullptr)
    // m_lsc is uninitialized
    return;

  m_lsc->insert(
    m_lsc->end(),
    hasse[e].signedcharacters.begin(),
    hasse[e].signedcharacters.end()
  );
}

void
safe_chain_dfs_visitor::tree_edge(const HDEdge e, const HDGraph& hasse) const {
  if (logging::enabled) {
    // verbosity enabled
    HDVertex vs, vt;
    std::tie(vs, vt) = incident(e, hasse);

    std::cout << "tree_edge: [ ";

    StringIter kk = hasse[vs].species.begin();
    for (; kk != hasse[vs].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "] -";

    SignedCharacterIter jj = hasse[e].signedcharacters.begin();
    for (; jj != hasse[e].signedcharacters.end(); ++jj) {
      std::cout << *jj;

      if (std::next(jj) != hasse[e].signedcharacters.end())
        std::cout << ",";
    }

    std::cout << "-> [ ";

    kk = hasse[vt].species.begin();
    for (; kk != hasse[vt].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  // ignore
}

void
safe_chain_dfs_visitor::back_edge(const HDEdge e, const HDGraph& hasse) const {
  if (logging::enabled) {
    // verbosity enabled
    HDVertex vs, vt;
    std::tie(vs, vt) = incident(e, hasse);

    std::cout << "back_edge: [ ";

    StringIter kk = hasse[vs].species.begin();
    for (; kk != hasse[vs].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "] -";

    SignedCharacterIter jj = hasse[e].signedcharacters.begin();
    for (; jj != hasse[e].signedcharacters.end(); ++jj) {
      std::cout << *jj;

      if (std::next(jj) != hasse[e].signedcharacters.end())
        std::cout << ",";
    }

    std::cout << "-> [ ";

    kk = hasse[vt].species.begin();
    for (; kk != hasse[vt].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  // ignore
}

void
safe_chain_dfs_visitor::forward_or_cross_edge(
    const HDEdge e,
    const HDGraph& hasse) const {
  if (logging::enabled) {
    // verbosity enabled
    HDVertex vs, vt;
    std::tie(vs, vt) = incident(e, hasse);

    std::cout << "forward_or_cross_edge: [ ";

    StringIter kk = hasse[vs].species.begin();
    for (; kk != hasse[vs].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "] -";

    SignedCharacterIter jj = hasse[e].signedcharacters.begin();
    for (; jj != hasse[e].signedcharacters.end(); ++jj) {
      std::cout << *jj;

      if (std::next(jj) != hasse[e].signedcharacters.end())
        std::cout << ",";
    }

    std::cout << "-> [ ";

    kk = hasse[vt].species.begin();
    for (; kk != hasse[vt].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  // ignore
}

void
safe_chain_dfs_visitor::finish_vertex(const HDVertex v, const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "finish_vertex: [ ";

    StringIter kk = hasse[v].species.begin();
    for (; kk != hasse[v].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // orig_g or orig_gm are uninitialized
    return;

  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  if (m_lsc == nullptr)
    // m_lsc is uninitialized
    return;

  if (last_v != v)
    // v is not the last vertex in the chain, which means the visit is
    // backtracking, so ignore it and keep going
    return;

  // source_v holds the source vertex of the chain
  // m_lsc holds the list of SignedCharacters representing the chain

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "S(C): < ";

    SignedCharacterIter kk = m_lsc->begin();
    for (; kk != m_lsc->end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << ">" << std::endl;
  }

  if (m_lsc->empty()) {
    // m_lsc being empty means it's not actually a chain
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Empty chain" << std::endl;
    }

    // TODO: confirm this is correct
    return;
  }

  // copy gm to gm_test
  RBGraph gm_test;
  copy_graph(gm, gm_test);

  // test if m_lsc is a safe chain
  bool feasible;
  std::tie(*m_lsc, feasible) = realize(*m_lsc, gm_test);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << gm_test << std::endl << std::endl;
  }

  if (!feasible) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Realization not feasible for Gm" << std::endl;
    }

    m_lsc->clear();
    return;
  }

  if (is_redsigma(gm_test)) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Found red Σ-graph in Gm" << std::endl;
    }

    m_lsc->clear();
    return;
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "No red Σ-graph in Gm" << std::endl;
  }

  // TODO: is testing the safe source here correct?

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Source: [ ";

    StringIter kk = hasse[source_v].species.begin();
    for (; kk != hasse[source_v].species.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << "( ";

    kk = hasse[source_v].characters.begin();
    for (; kk != hasse[source_v].characters.end(); ++kk) {
      std::cout << *kk << " ";
    }

    std::cout << ") ]" << std::endl << std::endl;
  }

  // test if source_v is a safe source
  if (!safe_source(source_v, hasse)) {
    // source_v is not a safe source
    m_lsc->clear();
    return;
  }

  throw SafeChain();
}


//=============================================================================
// Algorithm functions

bool safe_source(const HDVertex v, const HDGraph& hasse) {
  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return false;

  if (in_degree(v, hasse) > 0 || out_degree(v, hasse) < 1) {
    // v is not a source for hasse
    return false;
  }

  // retrieve graphs and maps from hasse
  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  // copy g to g_test, fill the vertex map v_map (and map_vertex)
  RBGraph g_test;
  copy_graph(g, g_test);

  // initialize the list of characters of v
  std::list<SignedCharacter> source_lsc;
  StringIter ci = hasse[v].characters.begin();
  for (; ci != hasse[v].characters.end(); ++ci) {
    source_lsc.push_back({ *ci, State::gain });
  }

  // test if source_v is a safe source
  bool feasible;
  std::tie(source_lsc, feasible) = realize(source_lsc, g_test);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "G test:" << std::endl
              << g_test << std::endl << std::endl;
  }

  if (!feasible) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Realization not feasible for G" << std::endl;
    }

    return false;
  }

  // if the realization didn't induce a red Σ-graph, v is a safe source
  bool output = !is_redsigma(g_test);

  if (logging::enabled) {
    // verbosity enabled
    if (output)
      std::cout << "No red Σ-graph in G" << std::endl << std::endl;
    else
      std::cout << "Found red Σ-graph in G" << std::endl;
  }

  return output;
}

std::pair<std::list<SignedCharacter>, bool> safe_chain(const HDGraph& hasse) {
  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return std::make_pair(std::list<SignedCharacter>(), false);

  std::list<SignedCharacter> output;
  bool safe = false;

  // try block:
  // the only way to stop depth_first_search from iterating on the Hasse
  // diagram is to throw an exception (this is documented by the BGL FAQ at
  // (1.) http://www.boost.org/doc/libs/1_65_1/libs/graph/doc/faq.html).

  // catch block:
  // SafeChain is a simple exception inheriting from std::exception which is
  // thrown when the visitor of depth_first_search finds a safe chain.

  // about the safe_chain_dfs_visitor:
  // the visitor continuosly modifies the output variable (passed as reference)
  // in search of safe chains. When one is found (and the SafeChain exception
  // thrown), output already holds the chain of the Hasse diagram considered
  // to be safe by the algorithm.
  // This behaviour is considered standard practice within the BGL (as
  // documented in the BGL FAQ at (2.))

  try {
    safe_chain_dfs_visitor vis(output);
    depth_first_search(hasse, boost::visitor(vis));
  }
  catch (const SafeChain& e) {
    // output holds a safe chain
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Safe chain: < ";

      SignedCharacterIter kk = output.begin();
      for (; kk != output.end(); ++kk) {
        std::cout << *kk << " ";
      }

      std::cout << ">" << std::endl << std::endl;
    }

    safe = true;
  }

  // if the visitor didn't raise a SafeChain exception then output is empty and
  // safe is still false (as declared at the start of the function)

  return std::make_pair(output, safe);
}


//=============================================================================
// Algorithm main functions

std::list<SignedCharacter> reduce(RBGraph& g) {
  std::list<SignedCharacter> output;

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "Working on G:" << std::endl
              << g << std::endl << std::endl;
  }

  // cleanup graph from dead vertices
  // TODO: check if this is needed (realize already does this?)
  remove_singletons(g);

  if (is_empty(g)) {
    // if graph is empty
    // return the empty sequence
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "G empty"
                << std::endl << std::endl;
    }

    // return < >
    return output;
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "G not empty" << std::endl;
  }

  // realize free characters in the graph
  // TODO: check if this is needed (realize already does this?)
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_free(*v, g)) {
      // if v is free
      // realize v-
      // return < v-, reduce(g) >
      if (logging::enabled) {
        // verbosity enabled
        std::cout << "G free character " << g[*v].name
                  << std::endl << std::endl;
      }

      std::list<SignedCharacter> lsc;
      std::tie(lsc, std::ignore) = realize({ g[*v].name, State::lose }, g);

      output.splice(output.end(), lsc);
      output.splice(output.end(), reduce(g));

      // return < v-, reduce(g) >
      return output;
    }
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "G no free characters" << std::endl;
  }

  // realize universal characters in the graph
  // TODO: check if this is needed (realize already does this?)
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_universal(*v, g)) {
      // if v is universal
      // realize v+
      // return < v+, reduce(g) >
      if (logging::enabled) {
        // verbosity enabled
        std::cout << "G universal character " << g[*v].name
                  << std::endl << std::endl;
      }

      std::list<SignedCharacter> lsc;
      std::tie(lsc, std::ignore) = realize({ g[*v].name, State::gain }, g);

      output.splice(output.end(), lsc);
      output.splice(output.end(), reduce(g));

      // return < v+, reduce(g) >
      return output;
    }
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "G no universal characters" << std::endl;
  }


  RBGraphVector components;
  size_t num_comps = connected_components(components, g);
  if (num_comps > 1) {
    // if graph is not connected
    // build subgraphs (connected components) g1, g2, etc.
    // return < reduce(g1), reduce(g2), ... >
    for (size_t i = 0; i < num_comps; ++i) {
      output.splice(output.end(), reduce(*components[i].get()));
    }

    // return < reduce(g1), reduce(g2), ... >
    return output;
  }

  // gm = Grb|Cm, maximal reducible graph of g (Grb)
  RBGraph gm = maximal_reducible_graph(g);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Gm:" << std::endl
              << gm << std::endl << std::endl;
  }

  // p = Hasse diagram for gm (Grb|Cm)
  HDGraph p;
  hasse_diagram(p, g, gm);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Hasse:" << std::endl
              << p << std::endl << std::endl;
  }

  if (num_edges(p) == 0)
    // no edges means there are no chains in the Hasse diagram
    throw NoReduction();

  // TODO: fix
  // sc = safe chain for g (Grb)
  std::list<SignedCharacter> sc;
  bool s_safe;
  std::tie(sc, s_safe) = safe_chain(p);

  if (!s_safe)
    // p has no safe [source|chain]
    throw NoReduction();

  // realize the characters of the safe chain
  std::list<SignedCharacter> lsc;
  std::tie(lsc, std::ignore) = realize(sc, g);

  // append the list of realized characters and the recursive call to the
  // output in constant time (std::list::splice simply moves pointers around
  // instead of copying the data)
  output.splice(output.end(), lsc);
  output.splice(output.end(), reduce(g));

  // return < lsc, reduce(g) >
  return output;
}

std::pair<std::list<SignedCharacter>, bool>
realize(const SignedCharacter& sc, RBGraph& g) {
  std::list<SignedCharacter> output;

  RBVertex cv = 0; // cv = current character vertex
  // get the vertex in g whose name is sc.character
  try {
    cv = get_vertex(sc.character, g);
  }
  catch (const std::out_of_range& e) {
    // g has no vertex named sc.character
    return std::make_pair(output, false);
  }

  RBVertexIMap map_index, map_comp;
  RBVertexIAssocMap i_map(map_index), c_map(map_comp);

  // fill vertex index map
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (size_t index = 0; v != v_end; ++v, ++index) {
    boost::put(i_map, *v, index);
  }

  // build the components map
  boost::connected_components(g, c_map, boost::vertex_index_map(i_map));

  if (sc.state == State::gain && is_inactive(cv, g)) {
    // c+ and c is inactive
    if (logging::enabled) {
      // verbosity enabled
      std::cout << sc << " (+ and inactive): ";
    }

    // realize the character c+:
    // - add a red edge between c and each species in D(c) \ N(c)
    // - delete all black edges incident on c
    std::tie(v, v_end) = vertices(g);
    for (; v != v_end; ++v) {
      if (is_character(*v, g) || map_comp[*v] != map_comp[cv])
        continue;
      // for each species in the same connected component of cv

      if (logging::enabled) {
        // verbosity enabled
        std::cout << g[*v].name << " -> ";
      }

      RBEdge e;
      bool exists;
      std::tie(e, exists) = edge(*v, cv, g);

      if (exists) {
        if (logging::enabled) {
          // verbosity enabled
          std::cout << "del; ";
        }

        // there is an edge (black) between *v and cv
        remove_edge(e, g);
      }
      else {
        // there isn't an edge between *v and cv
        if (logging::enabled) {
          // verbosity enabled
          std::cout << "red; ";
        }

        add_edge(*v, cv, Color::red, g);
      }
    }

    if (logging::enabled) {
      // verbosity enabled
      std::cout << std::endl;
    }
  }
  else if (sc.state == State::lose && is_active(cv, g)) {
    // c- and c is active
    if (logging::enabled) {
      // verbosity enabled
      std::cout << sc << " (- and active)" << std::endl;
    }

    // realize the character c-:
    // - delete all edges incident on c
    clear_vertex(cv, g);
  }
  else {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Could not realize " << sc << std::endl;
    }

    // this should never happen during the algorithm, but it is handled just in
    // case something breaks (or user input happens)
    return std::make_pair(output, false);
  }

  output.push_back(sc);

  // delete all isolated vertices
  remove_singletons(g);

  // realize all universal characters that came up after realizing sc
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_universal(*v, g)) {
      // if v is universal
      // realize v+
      if (logging::enabled) {
        // verbosity enabled
        std::cout << "G universal character " << g[*v].name << std::endl;
      }

      std::list<SignedCharacter> lsc;
      std::tie(lsc, std::ignore) = realize({ g[*v].name, State::gain }, g);

      output.splice(output.end(), lsc);

      return std::make_pair(output, true);
    }
  }

  // realize all free characters that came up after realizing sc
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_free(*v, g)) {
      // if v is free
      // realize v-
      if (logging::enabled) {
        // verbosity enabled
        std::cout << "G free character " << g[*v].name << std::endl;
      }

      std::list<SignedCharacter> lsc;
      std::tie(lsc, std::ignore) = realize({ g[*v].name, State::lose }, g);

      output.splice(output.end(), lsc);

      return std::make_pair(output, true);
    }
  }

  return std::make_pair(output, true);
}

std::pair<std::list<SignedCharacter>, bool>
realize(const RBVertex v, RBGraph& g) {
  std::list<SignedCharacter> lsc;

  if (!is_species(v, g))
    return std::make_pair(lsc, false);

  // build the list of inactive characters adjacent to v (species)
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    RBVertex u = target(*e, g);

    if (is_inactive(u, g))
      lsc.push_back({ g[u].name, State::gain });
  }

  return realize(lsc, g);
}

std::pair<std::list<SignedCharacter>, bool>
realize(const std::list<SignedCharacter>& lsc, RBGraph& g) {
  std::list<SignedCharacter> output;
  bool feasible = true;

  // realize the list of signed characters lsc; the algorithm doesn't stop when
  // a non-feasible realization is encountered, instead it sets the boolean
  // flag to false and keeps going (TODO: maybe change this behaviour)
  SignedCharacterIter i = lsc.begin();
  for (; i != lsc.end(); ++i) {
    std::list<SignedCharacter> signedcharacters;
    bool feasible_realize;
    std::tie(signedcharacters, feasible_realize) = realize(*i, g);

    output.splice(output.end(), signedcharacters);

    if (!feasible_realize)
      feasible = false;
  }

  return std::make_pair(output, feasible);
}
