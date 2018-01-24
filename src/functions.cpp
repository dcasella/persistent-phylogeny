#include "functions.hpp"
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/connected_components.hpp>


//=============================================================================
// Auxiliary structs and classes

initial_state_visitor::initial_state_visitor()
    : m_safe_sources(nullptr), m_sources(nullptr), chain(), source_v(0),
      last_v(0) {}

initial_state_visitor::initial_state_visitor(std::list<HDVertex>& safe_sources,
                                             std::list<HDVertex>& sources)
    : m_safe_sources(&safe_sources), m_sources(&sources), chain(), source_v(0),
      last_v(0) {
  m_safe_sources->clear();
  m_sources->clear();
}

void initial_state_visitor::initialize_vertex(const HDVertex v,
                                              const HDGraph& hasse) const {}

void initial_state_visitor::start_vertex(const HDVertex v,
                                         const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "DFS: start_vertex: [ ";

    for (const std::string& kk : hasse[v].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  source_v = v;

  chain.clear();
}

void initial_state_visitor::discover_vertex(const HDVertex v,
                                            const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "DFS: discover_vertex: [ ";

    for (const std::string& kk : hasse[v].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  last_v = v;
}

void initial_state_visitor::examine_edge(const HDEdge e,
                                         const HDGraph& hasse) {
  HDVertex vs, vt;
  std::tie(vs, vt) = incident(e, hasse);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "DFS: examine_edge: [ ";

    for (const std::string& kk : hasse[vs].species) {
      std::cout << kk << " ";
    }

    std::cout << "] -";

    SignedCharacterIter jj = hasse[e].signedcharacters.begin();
    for (; jj != hasse[e].signedcharacters.end(); ++jj) {
      std::cout << *jj;

      if (std::next(jj) != hasse[e].signedcharacters.end())
        std::cout << ",";
    }

    std::cout << "-> [ ";

    for (const std::string& kk : hasse[vt].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  if (in_degree(vs, hasse) == 0)
    // the source vertex of the edge is a source (indegree 0)
    chain.clear();

  // add edge to the chain
  chain.push_back(e);
}

void initial_state_visitor::tree_edge(const HDEdge e,
                                      const HDGraph& hasse) const {
  if (logging::enabled) {
    // verbosity enabled
    HDVertex vs, vt;
    std::tie(vs, vt) = incident(e, hasse);

    std::cout << "DFS: tree_edge: [ ";

    for (const std::string& kk : hasse[vs].species) {
      std::cout << kk << " ";
    }

    std::cout << "] -";

    SignedCharacterIter jj = hasse[e].signedcharacters.begin();
    for (; jj != hasse[e].signedcharacters.end(); ++jj) {
      std::cout << *jj;

      if (std::next(jj) != hasse[e].signedcharacters.end())
        std::cout << ",";
    }

    std::cout << "-> [ ";

    for (const std::string& kk : hasse[vt].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  // ignore
}

void initial_state_visitor::back_edge(const HDEdge e,
                                      const HDGraph& hasse) const {
  if (logging::enabled) {
    // verbosity enabled
    HDVertex vs, vt;
    std::tie(vs, vt) = incident(e, hasse);

    std::cout << "DFS: back_edge: [ ";

    for (const std::string& kk : hasse[vs].species) {
      std::cout << kk << " ";
    }

    std::cout << "] -";

    SignedCharacterIter jj = hasse[e].signedcharacters.begin();
    for (; jj != hasse[e].signedcharacters.end(); ++jj) {
      std::cout << *jj;

      if (std::next(jj) != hasse[e].signedcharacters.end())
        std::cout << ",";
    }

    std::cout << "-> [ ";

    for (const std::string& kk : hasse[vt].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  // ignore
}

void initial_state_visitor::forward_or_cross_edge(const HDEdge e,
                                                  const HDGraph& hasse) {
  HDVertex vs, vt;
  std::tie(vs, vt) = incident(e, hasse);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "DFS: forward_or_cross_edge: [ ";

    for (const std::string& kk : hasse[vs].species) {
      std::cout << kk << " ";
    }

    std::cout << "] -";

    SignedCharacterIter jj = hasse[e].signedcharacters.begin();
    for (; jj != hasse[e].signedcharacters.end(); ++jj) {
      std::cout << *jj;

      if (std::next(jj) != hasse[e].signedcharacters.end())
        std::cout << ",";
    }

    std::cout << "-> [ ";

    for (const std::string& kk : hasse[vt].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  if (out_degree(vt, hasse) == 1) {
    // e is not the last edge in the chain (vt is not a sink in hasse),
    // but it may be needed for the chain to complete
    HDOutEdgeIter oe;
    std::tie(oe, std::ignore) = out_edges(vt, hasse);

    // add outedge to the chain
    chain.push_back(*oe);
  }
  else if (out_degree(vt, hasse) > 1) {
    // e is not the last edge in the chain (vt is not a sink in hasse),
    // ignore it and keep going
    return;
  }

  perform_test(hasse);

  // remove the last added edge from the chain
  chain.pop_back();

  if (out_degree(vt, hasse) == 1)
    // remove the second last added egde from the chain,
    // since two edges were added in this function
    chain.pop_back();
}

void initial_state_visitor::finish_vertex(const HDVertex v,
                                          const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "DFS: finish_vertex: [ ";

    for (const std::string& kk : hasse[v].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  // build list of vertices in chain
  std::list<HDVertex> chain_v;
  for (const HDEdge& e : chain) {
    chain_v.push_back(source(e, hasse));
  }

  bool v_in_chain = (
    std::find(chain_v.cbegin(), chain_v.cend(), v) != chain_v.cend()
  );

  if (out_degree(v, hasse) > 0 || v_in_chain || last_v != v) {
    // v is not the last vertex in the chain
    // (which means the visit is backtracking)

    // remove the last added edge from the chain if the chain is not empty,
    // since this chain has already been tested
    if (!chain.empty())
      chain.pop_back();

    return;
  }

  perform_test(hasse);

  // remove the last added edge from the chain if the chain is not empty,
  // which is sure to be a tree edge
  if (!chain.empty())
    chain.pop_back();
}

void initial_state_visitor::perform_test(const HDGraph& hasse) {
  if (m_safe_sources == nullptr || m_sources == nullptr)
    // uninitialized sources lists
    return;

  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return;

  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  // source_v holds the source vertex of the chain
  // chain holds the list of edges representing the chain

  // check if source_v is already in m_safe_sources or m_sources
  if ((!m_safe_sources->empty() && source_v == m_safe_sources->back()) ||
      (!m_sources->empty() && source_v == m_sources->back()))
    return;

  // test if chain is a safe chain
  if (!safe_chain(hasse))
    // chain is not a safe chain
    return;

  // test is source_v is a safe source (for test 1)
  if (safe_source_test1(hasse))
    // source_v is a safe source, return (don't add it to m_sources)
    return;

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl;
  }

  // test is source_v is a safe source (for test 2)
  if (safe_source_test2(hasse))
    // source_v is a safe source, return (don't add it to m_sources)
    return;

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "Source added to the list of sources"
              << std::endl;

    if (!chain.empty())
      std::cout << std::endl;
  }

  m_sources->push_back(source_v);
}

bool initial_state_visitor::safe_chain(const HDGraph& hasse) {
  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  // chain holds the list of edges representing the chain

  if (chain.empty()) {
    // chain being empty means it's not actually a chain?
    if (logging::enabled) {
      // verbosity enabled
      std::cout << std::endl
                << "Empty chain" << std::endl << std::endl;
    }

    return true;
  }

  std::list<SignedCharacter> lsc;

  for (const std::string& c : hasse[source_v].characters) {
    lsc.push_back({ c, State::gain });
  }

  for (const HDEdge& e : chain) {
    lsc.insert(
      lsc.end(),
      hasse[e].signedcharacters.begin(),
      hasse[e].signedcharacters.end()
    );
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "Test chain: < ";

    for (const SignedCharacter& kk : lsc) {
      std::cout << kk << " ";
    }

    std::cout << "> on a copy of graph G" << std::endl;
  }

  // copy g to g_test
  RBGraph g_test;
  copy_graph(g, g_test);

  // test if lsc is a safe chain
  bool feasible;
  std::tie(std::ignore, feasible) = realize(lsc, g_test);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "G (copy) after the realization of the chain" << std::endl
              << "Adjacency lists:" << std::endl
              << g_test << std::endl << std::endl;
  }

  if (!feasible) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Realization not feasible for G (copy)"
                << std::endl << std::endl;
    }

    return false;
  }

  // if the realization didn't induce a red Σ-graph, chain is a safe chain
  bool output = !is_redsigma(g_test);

  if (logging::enabled) {
    // verbosity enabled
    if (output)
      std::cout << "No red Σ-graph in G (copy)" << std::endl << std::endl;
    else
      std::cout << "Found red Σ-graph in G (copy)" << std::endl << std::endl;
  }

  return output;
}

bool initial_state_visitor::safe_source_test1(const HDGraph& hasse) {
  bool output = false;

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Safe sources - test 1" << std::endl;
  }

  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  // search for a species s+ in GRB|CM∪A that consists of C(s) and is connected
  // to only inactive characters
  for (const std::string& species_name : hasse[source_v].species) {
    RBVertex source_s = get_vertex(species_name, gm);
    // for each source species (s+) in source_v
    bool active = false;

    // check if s+ is connected to active characters
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(source_s, gm);
    for (; e != e_end; ++e) {
      // for each out egde from s+
      if (is_red(*e, gm)) {
        // s+ is connected to active characters; search for another species
        active = true;

        break;
      }
    }

    if (active)
      // s+ is connected to active characters
      continue;

    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Source species: " << species_name
                << std::endl;
    }

    if (!realize_source(source_v, hasse))
      continue;

    m_safe_sources->push_back(source_v);

    output |= true;

    if (exponential::enabled || interactive::enabled) {
      // exponential algorithm or user interaction enabled
      if (logging::enabled) {
        // verbosity enabled
        std::cout << std::endl
                  << "Source added to the list of safe sources"
                  << std::endl << std::endl;
      }

      return output;
    }

    throw InitialState();
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Safe sources - test 1 failed" << std::endl;
  }

  return output;
}

bool initial_state_visitor::safe_source_test2(const HDGraph& hasse) {
  bool output = false;

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Safe sources - test 2" << std::endl;
  }

  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  // list of characters of source_v
  std::list<std::string> source_c = hasse[source_v].characters;

  // list of characters of GRB|CM∪A
  std::list<std::string> gm_c;
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(gm);
  for (; v != v_end; ++v) {
    if (!is_character(*v, g))
      continue;

    gm_c.push_back(gm[*v].name);
  }

  // search for a species s+ in GRB|CM∪A that consists of C(s) and a set of
  // maximal characters, and is connected to only inactive characters
  std::tie(v, v_end) = vertices(gm);
  for (; v != v_end; ++v) {
    if (!is_species(*v, g))
      continue;

    // if s+ is in source_v it means that it was already tested previously
    StringIter search = std::find(
      hasse[source_v].species.cbegin(),
      hasse[source_v].species.cend(),
      gm[*v].name
    );

    if (search != hasse[source_v].species.cend())
      // s+ is in source_v, search for another species
      continue;

    std::list<std::string> maximal_c;
    size_t count_maximal = 0;
    bool active = false;

    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, gm);
    for (; e != e_end; ++e) {
      // for each out egde from s+
      if (is_red(*e, gm)) {
        // s+ is connected to active characters; search for another species
        active = true;

        break;
      }
      else {
        RBVertex vt = target(*e, gm);

        // search if vt is a maximal character in source_c
        StringIter search = std::find(
          source_c.cbegin(), source_c.cend(), gm[vt].name
        );

        if (search == source_c.cend()) {
          // vt is not a maximal character in source_c

          // search if vt is a maximal character in GRB|CM∪A
          search = std::find(gm_c.cbegin(), gm_c.cend(), gm[vt].name);

          if (search != gm_c.cend())
            // add vt to the set of maximal characters
            maximal_c.push_back(gm[vt].name);
        }
        else {
          count_maximal++;
        }
      }
    }

    if (count_maximal < source_c.size() || active)
      // s+ doesn't consist of C(s) or it's connected to active characters
      continue;

    if (maximal_c.size() == 0)
      // s+ doesn't have a set of other maximal characters
      continue;

    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Source species (+ other maximal characters): "
                << gm[*v].name << std::endl;
    }

    if (!realize_source(source_v, hasse))
      continue;

    m_safe_sources->push_back(source_v);

    output |= true;

    if (exponential::enabled || interactive::enabled) {
      // exponential algorithm or user interaction enabled
      if (logging::enabled) {
        // verbosity enabled
        std::cout << std::endl
                  << "Source added to the list of safe sources"
                  << std::endl << std::endl;
      }

      return output;
    }

    throw InitialState();
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Safe sources - test 2 failed" << std::endl;
  }

  return output;
}


//=============================================================================
// Algorithm functions

std::list<HDVertex> initial_states(const HDGraph& hasse) {
  std::list<HDVertex> output;

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "DFS visit on the Hasse diagram:"
              << std::endl << std::endl;
  }

  // the visitor continuosly modifies the sources variable (passed as reference)
  // in search of safe chains and sources. At the end of the visit, sources
  // holds the list of sources of the Hasse diagram.
  std::list<HDVertex> sources;
  initial_state_visitor vis(output, sources);

  try {
    depth_first_search(hasse, boost::visitor(vis));
  }
  catch (const InitialState& e) {}

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "DFS visit on the Hasse diagram terminated"
              << std::endl << std::endl;
  }

  if (output.empty() && sources.size() == 1) {
    output.push_back(sources.front());
  }
  else if (output.empty() && sources.size() > 1) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Sources: < ";

      for (const HDVertex& i : sources) {
        std::cout << "[ ";

        for (const std::string& kk : hasse[i].species) {
          std::cout << kk << " ";
        }

        std::cout << "( ";

        for (const std::string& kk : hasse[i].characters) {
          std::cout << kk << " ";
        }

        std::cout << ") ] ";
      }

      std::cout << ">" << std::endl << std::endl;
    }

    output = safe_source_test3(sources, hasse);
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Safe sources: < ";

    for (const HDVertex& i : output) {
      std::cout << "[ ";

      for (const std::string& kk : hasse[i].species) {
        std::cout << kk << " ";
      }

      std::cout << "( ";

      for (const std::string& kk : hasse[i].characters) {
        std::cout << kk << " ";
      }

      std::cout << ") ] ";
    }

    std::cout << ">" << std::endl << std::endl;
  }

  return output;
}

std::list<HDVertex>
safe_source_test3(const std::list<HDVertex>& sources, const HDGraph& hasse) {
  std::list<HDVertex> output;

  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return output;

  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Safe sources - test 3" << std::endl;
  }

  HDVertexIMap source_map;

  // make sure every source is connected to active characters
  for (const HDVertex& source : sources) {
    bool source_active = true;

    // make sure every species s+ is connected to active characters
    for (const std::string& species_name : hasse[source].species) {
      RBVertex source_s = get_vertex(species_name, gm);
      // for each source species (s+) in source
      size_t active_count = 0;

      // check if s+ is connected to active characters
      RBOutEdgeIter e, e_end;
      std::tie(e, e_end) = out_edges(source_s, gm);
      for (; e != e_end; ++e) {
        // for each out egde from s+
        if (!is_red(*e, gm))
          continue;

        active_count++;
      }

      if (active_count == 0) {
        // s+ is not connected to active characters
        source_active = false;

        break;
      }

      if (source_map[source] > 0 && active_count >= source_map[source])
        continue;

      source_map[source] = active_count;
    }

    if (!source_active)
      // if s+ could not be found, the test fails
      return output;
  }

  size_t min_active_count = 0;
  std::list<HDVertex> maybe_output;

  for (const std::pair<HDVertex, size_t>& pair : source_map) {
    size_t active_count = pair.second;

    if (min_active_count > 0 && active_count >= min_active_count)
      continue;

    min_active_count = active_count;
  }

  for (const std::pair<HDVertex, size_t>& pair : source_map) {
    HDVertex source = pair.first;
    size_t active_count = pair.second;

    if (active_count > min_active_count)
      continue;

    maybe_output.push_back(source);
  }

  for (const HDVertex& source : maybe_output) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Source (+ active characters): [ ";

      for (const std::string& kk : hasse[source].species) {
        std::cout << kk << " ";
      }

      std::cout << "( ";

      for (const std::string& kk : hasse[source].characters) {
        std::cout << kk << " ";
      }

      std::cout << ") ]" << std::endl;
    }

    bool is_safe = realize_source(source, hasse);

    if (logging::enabled) {
      // verbosity enabled
      std::cout << std::endl;
    }

    if (!is_safe)
      continue;

    output.push_back(source);

    return output;
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Safe sources - test 3 failed" << std::endl;
  }

  return output;
}

bool realize_source(const HDVertex source, const HDGraph& hasse) {
  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return false;

  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Test source realization: [ ";

    for (const std::string& kk : hasse[source].species) {
      std::cout << kk << " ";
    }

    std::cout << "( ";

    for (const std::string& kk : hasse[source].characters) {
      std::cout << kk << " ";
    }

    std::cout << ") ] on a copy of graph G" << std::endl;
  }

  // copy g to g_test
  RBGraph g_test;
  copy_graph(g, g_test);

  // initialize the list of characters of source
  std::list<SignedCharacter> source_lsc;
  for (const std::string& ci : hasse[source].characters) {
    source_lsc.push_back({ ci, State::gain });
  }

  bool feasible;
  std::tie(std::ignore, feasible) = realize(source_lsc, g_test);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "G (copy) after the realization of the source" << std::endl
              << "Adjacency lists:" << std::endl
              << g_test << std::endl << std::endl;
  }

  if (!feasible) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Realization not feasible for G (copy)" << std::endl;
    }

    return false;
  }

  // if the realization didn't induce a red Σ-graph, source is a safe source
  bool output = !is_redsigma(g_test);

  if (logging::enabled) {
    // verbosity enabled
    if (output)
      std::cout << "No red Σ-graph in G (copy)" << std::endl;
    else
      std::cout << "Found red Σ-graph in G (copy)" << std::endl;
  }

  return output;
}


//=============================================================================
// Algorithm main functions

std::list<SignedCharacter> reduce(RBGraph& g) {
  std::list<SignedCharacter> output;

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "Working on the red-black graph G" << std::endl
              << "Adjacency lists:" << std::endl
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
        std::cout << "G free character " << g[*v].name << std::endl;
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
        std::cout << "G universal character " << g[*v].name << std::endl;
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

  RBGraphVector components = connected_components(g);

  if (components.size() > 1) {
    // if graph is not connected
    // build subgraphs (connected components) g1, g2, etc.
    // return < reduce(g1), reduce(g2), ... >
    for (size_t i = 0; i < components.size(); ++i) {
      output.splice(output.end(), reduce(*components[i].get()));
    }

    // return < reduce(g1), reduce(g2), ... >
    return output;
  }

  components.clear();

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl;
  }

  // gm = Grb|Cm∪A, maximal reducible graph of g (Grb)
  RBGraph gm = maximal_reducible_graph(g, true);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "Subgraph Gm of G induced by the maximal characters Cm"
              << std::endl
              << "Adjacency lists:" << std::endl
              << gm << std::endl << std::endl;
  }

  // p = Hasse diagram for gm (Grb|Cm∪A)
  HDGraph p;
  hasse_diagram(p, g, gm);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Hasse diagram for the subgraph Gm" << std::endl
              << "Adjacency lists:" << std::endl
              << p << std::endl << std::endl;
  }

  // s = initial states
  std::list<HDVertex> s = initial_states(p);

  if (s.empty())
    // p has no safe source
    throw NoReduction();

  std::list<SignedCharacter> sc;

  // exponential safe source selection
  if (s.size() > 1 && exponential::enabled) {
    // exponential algorithm enabled
    std::list<std::list<SignedCharacter>> sources_output;

    for (const HDVertex& source : s) {
      // for each safe source in s
      RBGraph g_test;
      copy_graph(g, g_test);

      if (logging::enabled) {
        // verbosity enabled
        std::cout << "Current safe source: [ ";

        for (const std::string& kk : p[source].species) {
          std::cout << kk << " ";
        }

        std::cout << "( ";

        for (const std::string& kk : p[source].characters) {
          std::cout << kk << " ";
        }

        std::cout << ") ]" << std::endl << std::endl;
      }

      // realize the characters of the safe source
      sc.clear();

      for (const std::string& ci : p[source].characters) {
        sc.push_back({ ci, State::gain });
      }

      if (logging::enabled) {
        // verbosity enabled
        std::cout << "Realize the characters < ";

        for (const SignedCharacter& kk : sc) {
          std::cout << kk << " ";
        }

        std::cout << "> in G" << std::endl;
      }

      std::tie(sc, std::ignore) = realize(sc, g_test);

      try {
        std::list<SignedCharacter> rest = reduce(g_test);

        if (logging::enabled) {
          // verbosity enabled
          std::cout << "Ok for safe source [ ";

          for (const std::string& kk : p[source].species) {
            std::cout << kk << " ";
          }

          std::cout << "( ";

          for (const std::string& kk : p[source].characters) {
            std::cout << kk << " ";
          }

          std::cout << ") ]" << std::endl << std::endl;
        }

        // append the recursive call to the current source's output
        sc.splice(sc.end(), rest);
        sources_output.push_back(sc);
      }
      catch (const NoReduction& e) {
        if (logging::enabled) {
          // verbosity enabled
          std::cout << "No successful reduction for safe source [ ";

          for (const std::string& kk : p[source].species) {
            std::cout << kk << " ";
          }

          std::cout << "( ";

          for (const std::string& kk : p[source].characters) {
            std::cout << kk << " ";
          }

          std::cout << ") ]" << std::endl << std::endl;
        }
      }
    }

    if (sources_output.empty())
      // no realization induces a successful reduction
      throw NoReduction();

    return sources_output.front();
  }
  // user-input-driven safe source selection
  else if (s.size() > 1 && interactive::enabled) {
    // user interaction enabled
    HDVertex source = 0;
    size_t choice = 0;

    if (!logging::enabled) {
      std::cout << std::endl << std::endl;
    }

    std::cout << "========================================"
              << "========================================"
              << std::endl << std::endl
              << "List of available source indices to choose from:"
              << std::endl;

    size_t index = 0;
    for (const HDVertex& source : s) {
      std::cout << "  - " << index << ": [ ";

      for (const std::string& kk : p[source].species) {
        std::cout << kk << " ";
      }

      std::cout << "( ";

      for (const std::string& kk : p[source].characters) {
        std::cout << kk << " ";
      }

      std::cout << ") ]" << std::endl;

      index++;
    }

    std::cout << std::endl;

    // get input
    std::string input;
    std::cout << "Choose a source: ";

    while (std::getline(std::cin, input)) {
      // if (input == "help" || input == "h") {
      //   // print help message
      // }

      // parse input as a number
      std::stringstream istream(input);
      if (istream >> choice) {
        // choice is a valid number
        if (choice < s.size()) {
          // choice is a valid safe source index
          // set the source
          source = *std::next(s.begin(), choice);

          std::cout << "Source [ ";

          for (const std::string& kk : p[source].species) {
            std::cout << kk << " ";
          }

          std::cout << "( ";

          for (const std::string& kk : p[source].characters) {
            std::cout << kk << " ";
          }

          std::cout << ") ] chosen" << std::endl << std::endl;
          // exit the loop
          break;
        }
      }

      std::cout << "Error: invalid input."
                // << std::endl
                // << "Try 'help' or 'h' for more information."
                << std::endl << std::endl
                << "Choose a source: ";
    }

    sc.clear();

    for (const std::string& ci : p[source].characters) {
      sc.push_back({ ci, State::gain });
    }

    if (logging::enabled) {
      // verbosity enabled
      std::cout << "========================================"
                << "========================================"
                << std::endl << std::endl;
    }
  }
  // standard safe source selection (the first one found)
  else {
    sc.clear();

    for (const std::string& ci : p[s.front()].characters) {
      sc.push_back({ ci, State::gain });
    }
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Realize the characters < ";

    for (const SignedCharacter& kk : sc) {
      std::cout << kk << " ";
    }

    std::cout << "> in G" << std::endl;
  }

  // realize the characters of the safe source
  std::tie(sc, std::ignore) = realize(sc, g);

  // append the list of realized characters and the recursive call to the
  // output in constant time (std::list::splice simply moves pointers around
  // instead of copying the data)
  output.splice(output.end(), sc);
  output.splice(output.end(), reduce(g));

  // return < sc, reduce(g) >
  return output;
}

std::pair<std::list<SignedCharacter>, bool>
realize(const SignedCharacter& sc, RBGraph& g) {
  std::list<SignedCharacter> output;

  // current character vertex
  RBVertex cv = 0;

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
      std::cout << "Realizing " << sc;
    }

    // realize the character c+:
    // - add a red edge between c and each species in D(c) \ N(c)
    // - delete all black edges incident on c
    std::tie(v, v_end) = vertices(g);
    for (; v != v_end; ++v) {
      if (!is_species(*v, g) || map_comp[*v] != map_comp[cv])
        continue;
      // for each species in the same connected component of cv

      RBEdge e;
      bool exists;
      std::tie(e, exists) = edge(*v, cv, g);

      if (exists)
        // there is an edge (black) between *v and cv
        remove_edge(e, g);
      else
        // there isn't an edge between *v and cv
        add_edge(*v, cv, Color::red, g);
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
      std::cout << "Realizing " << sc << std::endl;
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

  // realize the list of signed characters lsc; the algorithm stops when a
  // non-feasible realization is encountered, setting the boolean flag to false
  // TODO: maybe change this behaviour
  for (const SignedCharacter& i : lsc) {
    if (std::find(output.cbegin(), output.cend(), i) != output.cend())
      // the signed character i has already been realized in a previous sc
      continue;

    std::list<SignedCharacter> sc;
    bool feasible;
    std::tie(sc, feasible) = realize(i, g);

    if (!feasible)
      return std::make_pair(sc, false);

    output.splice(output.end(), sc);
  }

  return std::make_pair(output, true);
}
