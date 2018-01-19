#include "functions.hpp"
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/connected_components.hpp>


//=============================================================================
// Auxiliary structs and classes

initial_state_visitor::initial_state_visitor()
    : m_sources(nullptr), chain(), source_v(0), last_v(0) {}

initial_state_visitor::initial_state_visitor(std::list<HDVertex>& sources)
    : m_sources(&sources), chain(), source_v(0), last_v(0) {
  m_sources->clear();
}

void initial_state_visitor::initialize_vertex(const HDVertex v,
                                              const HDGraph& hasse) const {}

void initial_state_visitor::start_vertex(const HDVertex v,
                                         const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "start_vertex: [ ";

    for (const std::string& kk : hasse[v].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  source_v = v;

  chain.clear();

  if (num_edges(hasse) != 0)
    // the Hasse diagram is non-degenerate
    return;

  // no edges means there are no chains in the Hasse diagram:
  // the Hasse diagram is degenerate

  perform_test_degenerate(hasse);
}

void initial_state_visitor::discover_vertex(const HDVertex v,
                                            const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "discover_vertex: [ ";

    for (const std::string& kk : hasse[v].species) {
      std::cout << kk << " ";
    }

    std::cout << "]" << std::endl;
  }

  last_v = v;
}

void initial_state_visitor::examine_edge(const HDEdge e,
                                         const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    HDVertex vs, vt;
    std::tie(vs, vt) = incident(e, hasse);

    std::cout << "examine_edge: [ ";

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

  chain.push_back(e);
}

void initial_state_visitor::tree_edge(const HDEdge e,
                                      const HDGraph& hasse) const {
  if (logging::enabled) {
    // verbosity enabled
    HDVertex vs, vt;
    std::tie(vs, vt) = incident(e, hasse);

    std::cout << "tree_edge: [ ";

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

    std::cout << "back_edge: [ ";

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
    std::cout << "forward_or_cross_edge: [ ";

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

  if (out_degree(vt, hasse) > 0)
    // e is not the last edge in the chain (vt is not a sink in hasse), so
    // ignore it and keep going
    return;

  perform_test(hasse);
}

void initial_state_visitor::finish_vertex(const HDVertex v,
                                          const HDGraph& hasse) {
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "finish_vertex: [ ";

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

  if (v_in_chain || last_v != v) {
    // v is not the last vertex in the chain
    // (which means the visit is backtracking)

    // remove the last added edge from the chain if the chain is not empty
    if (!chain.empty())
      chain.pop_back();

    return;
  }

  perform_test(hasse);

  // remove the last added edge from the chain
  if (!chain.empty())
    chain.pop_back();
}

void initial_state_visitor::perform_test(const HDGraph& hasse) {
  if (m_sources == nullptr)
    // m_sources is uninitialized
    return;

  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return;

  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  // source_v holds the source vertex of the chain
  // chain holds the list of edges representing the chain

  // test if chain is a safe chain
  if (!safe_chain(hasse))
    // chain is not a safe chain
    return;

  // check if source_v is already a safe source
  if (m_sources->size() > 0 && source_v == m_sources->back()) {
    // source_v already is a safe source
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Source: [ ";

      for (const std::string& kk : hasse[source_v].species) {
        std::cout << kk << " ";
      }

      std::cout << "( ";

      for (const std::string& kk : hasse[source_v].characters) {
        std::cout << kk << " ";
      }

      std::cout << ") ] is safe" << std::endl << std::endl;
    }

    return;
  }

  std::list<std::string> source_c = hasse[source_v].characters;

  for (const std::string& species_name : hasse[source_v].species) {
    RBVertex source_s = get_vertex(species_name, gm);

    std::list<std::string> minimal_c;
    size_t count_inactives = 0;
    bool active = false;

    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(source_s, gm);
    for (; e != e_end; ++e) {
      if (is_red(*e, gm)) {
        active = true;
        break;
      }
      else {
        RBVertex vt = target(*e, gm);

        StringIter search = std::find(
          source_c.cbegin(), source_c.cend(), g[vt].name
        );

        if (search == source_c.cend())
          minimal_c.push_back(gm[vt].name);

        count_inactives++;
      }
    }

    // if there exists a species s+ in GRB|CM∪A that consists of C(s) and all
    // active characters in GRB

    // s+ doesn't consist of C(s) or it's active
    if (count_inactives < source_c.size() || active)
      continue;

    // if there exists a species s+ in GRB|CM∪A that consists of C(s) and all
    // active characters in GRB and a set I of characters
    if (count_inactives > source_c.size() || minimal_c.size() > 0) {
      // TODO (do nothing for now)
      continue;
    }

    if (!safe_source(hasse))
      // source_v is not a safe source
      continue;

    m_sources->push_back(source_v);

    if (exponential::enabled || interactive::enabled) {
      // exponential algorithm or user interaction enabled
      return;
    }

    // stop at the first safe source
    throw InitialState();
  }

  // if there does not exist a species s+ in GRB|CM∪A that consists of C(s) and
  // all active characters in GRB
  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Source: [ ";

    for (const std::string& kk : hasse[source_v].species) {
      std::cout << kk << " ";
    }

    std::cout << "( ";

    for (const std::string& kk : hasse[source_v].characters) {
      std::cout << kk << " ";
    }

    std::cout << ") ] is not safe" << std::endl << std::endl;
  }
}

void initial_state_visitor::perform_test_degenerate(const HDGraph& hasse) {
  if (m_sources == nullptr)
    // m_sources is uninitialized
    return;

  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return;

  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  // source_v holds the source vertex

  // check if source_v is already a safe source
  if (m_sources->size() > 0 && source_v == m_sources->back()) {
    // source_v already is a safe source
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Source: [ ";

      for (const std::string& kk : hasse[source_v].species) {
        std::cout << kk << " ";
      }

      std::cout << "( ";

      for (const std::string& kk : hasse[source_v].characters) {
        std::cout << kk << " ";
      }

      std::cout << ") ] is safe" << std::endl << std::endl;
    }

    return;
  }

  // TODO (assume source is safe for now)

  m_sources->push_back(source_v);

  if (exponential::enabled || interactive::enabled) {
    // exponential algorithm or user interaction enabled
    return;
  }

  // stop at the first safe source
  throw InitialState();
}

bool initial_state_visitor::safe_chain(const HDGraph& hasse) {
  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  // chain holds the list of edges representing the chain

  // TODO: change this check?
  if (chain.empty()) {
    // chain being empty means it's not actually a chain?
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Empty chain" << std::endl << std::endl;
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
              << "S(C): < ";

    for (const SignedCharacter& kk : lsc) {
      std::cout << kk << " ";
    }

    std::cout << ">" << std::endl;
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
              << "G test:" << std::endl
              << g_test << std::endl << std::endl;
  }

  if (!feasible) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Realization not feasible for G" << std::endl << std::endl;
    }

    return false;
  }

  // if the realization didn't induce a red Σ-graph, chain is a safe chain
  bool output = !is_redsigma(g_test);

  if (logging::enabled) {
    // verbosity enabled
    if (output)
      std::cout << "No red Σ-graph in G" << std::endl << std::endl;
    else
      std::cout << "Found red Σ-graph in G" << std::endl << std::endl;
  }

  return output;
}

bool initial_state_visitor::safe_source(const HDGraph& hasse) {
  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Source: [ ";

    for (const std::string& kk : hasse[source_v].species) {
      std::cout << kk << " ";
    }

    std::cout << "( ";

    for (const std::string& kk : hasse[source_v].characters) {
      std::cout << kk << " ";
    }

    std::cout << ") ]" << std::endl;
  }

  // copy g to g_test
  RBGraph g_test;
  copy_graph(g, g_test);

  // initialize the list of characters of source_v
  std::list<SignedCharacter> source_lsc;
  for (const std::string& ci : hasse[source_v].characters) {
    source_lsc.push_back({ ci, State::gain });
  }

  // test if source_v is a safe source
  bool feasible;
  std::tie(std::ignore, feasible) = realize(source_lsc, g_test);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl
              << "G test:" << std::endl
              << g_test << std::endl << std::endl;
  }

  if (!feasible) {
    if (logging::enabled) {
      // verbosity enabled
      std::cout << "Realization not feasible for G" << std::endl << std::endl;
    }

    return false;
  }

  // if the realization didn't induce a red Σ-graph, source_v is a safe source
  bool output = !is_redsigma(g_test);

  if (logging::enabled) {
    // verbosity enabled
    if (output)
      std::cout << "No red Σ-graph in G" << std::endl << std::endl;
    else
      std::cout << "Found red Σ-graph in G" << std::endl << std::endl;
  }

  return output;
}


//=============================================================================
// Algorithm functions

std::list<HDVertex> initial_states(const HDGraph& hasse) {
  std::list<HDVertex> output;

  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return output;

  RBGraph g = *orig_g(hasse);

  // try block:
  // the only way to stop depth_first_search from iterating on the Hasse
  // diagram is to throw an exception (this is documented by the BGL FAQ at
  // (1.) http://www.boost.org/doc/libs/1_65_1/libs/graph/doc/faq.html).

  // catch block:
  // InitialState is a simple exception inheriting from std::exception which is
  // thrown when the visitor of depth_first_search finds a safe chain and its
  // safe source.

  // about the initial_state_visitor:
  // the visitor continuosly modifies the output variable (passed as reference)
  // in search of safe chains and sources. When one is found (and the
  // InitialState exception thrown), output already holds the list of sources
  // of the Hasse diagram considered to be safe by the algorithm.
  // This behaviour is considered standard practice within the BGL (as
  // documented in the BGL FAQ at (2.))

  try {
    initial_state_visitor vis(output);
    depth_first_search(hasse, boost::visitor(vis));
  }
  catch (const InitialState& e) {
    // output holds a safe source
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

  // if exponential::enabled or interactive::enabled, no exception is thrown,
  // but output won't be empty if there is at least one safe source in the
  // Hasse diagram

  return output;
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

  // gm = Grb|Cm∪A, maximal reducible graph of g (Grb)
  RBGraph gm = maximal_reducible_graph(g, true);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Gm:" << std::endl
              << gm << std::endl << std::endl;
  }

  // p = Hasse diagram for gm (Grb|Cm∪A)
  HDGraph p;
  hasse_diagram(p, g, gm);

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Hasse:" << std::endl
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
        std::cout << std::endl
                  << "Current safe source: [ ";

        for (const std::string& kk : p[source].species) {
          std::cout << kk << " ";
        }

        std::cout << "( ";

        for (const std::string& kk : p[source].characters) {
          std::cout << kk << " ";
        }

        std::cout << ") ]" << std::endl;
      }

      // realize the characters of the safe source
      sc.clear();

      for (const std::string& ci : p[source].characters) {
        sc.push_back({ ci, State::gain });
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

          std::cout << ") ]" << std::endl;
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

          std::cout << ") ]" << std::endl;
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
      std::cout << sc << " (+ and inactive): ";
    }

    // realize the character c+:
    // - add a red edge between c and each species in D(c) \ N(c)
    // - delete all black edges incident on c
    std::tie(v, v_end) = vertices(g);
    for (; v != v_end; ++v) {
      if (!is_species(*v, g) || map_comp[*v] != map_comp[cv])
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
