#include "hdgraph.hpp"


//=============================================================================
// Boost functions (overloading)

// Hasse Diagram

HDVertex
add_vertex(
    const std::list<std::string>& species,
    const std::list<std::string>& characters,
    HDGraph& hasse) {
  HDVertex v = boost::add_vertex(hasse);
  hasse[v].species = species;
  hasse[v].characters = characters;

  return v;
}

std::pair<HDEdge, bool>
add_edge(
    const HDVertex u,
    const HDVertex v,
    const std::list<SignedCharacter>& signedcharacters,
    HDGraph& hasse) {
  HDEdge e;
  bool exists;
  std::tie(e, exists) = boost::add_edge(u, v, hasse);
  hasse[e].signedcharacters = signedcharacters;

  return std::make_pair(e, exists);
}


//=============================================================================
// General functions

// Hasse Diagram

std::ostream& operator<<(std::ostream& os, const HDGraph& hasse) {
  HDVertexIter v, v_end;
  std::tie(v, v_end) = vertices(hasse);
  for (; v != v_end; ++v) {
    os << "[ ";

    for (const std::string& i : hasse[*v].species) {
      os << i << " ";
    }

    os << "( ";

    for (const std::string& i : hasse[*v].characters) {
      os << i << " ";
    }

    os << ") ]:";

    HDOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, hasse);
    for (; e != e_end; ++e) {
      HDVertex vt = target(*e, hasse);

      os << " -";

      SignedCharacterIter j = hasse[*e].signedcharacters.begin();
      for (; j != hasse[*e].signedcharacters.end(); ++j) {
        os << *j;

        if (std::next(j) != hasse[*e].signedcharacters.end())
          os << ",";
      }

      os << "-> [ ";

      for (const std::string& i : hasse[vt].species) {
        os << i << " ";
      }

      os << "( ";

      for (const std::string& i : hasse[vt].characters) {
        os << i << " ";
      }

      os << ") ];";
    }

    if (std::next(v) != v_end)
      os << std::endl;
  }

  return os;
}


//=============================================================================
// Algorithm functions

bool
is_included(const std::list<std::string>& a, const std::list<std::string>& b) {
  for (const std::string& i : a) {
    StringIter in;
    in = std::find(b.begin(), b.end(), i);

    if (in == b.end())
      // exit the function at the first string of a not present in b
      return false;
  }

  return true;
}

void hasse_diagram(HDGraph& hasse, const RBGraph& g, const RBGraph& gm) {
  std::vector<std::list<RBVertex>> sets(num_species(gm));
  std::map<RBVertex, std::list<RBVertex>> v_map;

  // how sets is going to be structured:
  // sets[index] => < S, List of characters adjacent to S >

  // how v_map is going to be structured:
  // v_map[S] => < List of characters adjacent to S >

  // sets is used to sort the lists by number of elements, this is why we store
  // the list of adjacent characters to S. While we store S to be able to
  // access v_map[S] in costant time, instead of iterating on sets to find the
  // correct list

  // initialize sets and v_map for each species in the graph
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(gm);
  for (size_t index = 0; v != v_end; ++v) {
    if (!is_species(*v, gm))
      continue;
    // for each species vertex

    if (logging::enabled) {
      // verbosity enabled
      std::cout << "C(" << gm[*v].name << ") = { ";
    }

    // sets[index]'s first element is the species vertex
    sets[index].push_back(*v);

    // build v's set of adjacent characters
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, gm);
    for (; e != e_end; ++e) {
      // vt = one of the characters adjacent to *v
      RBVertex vt = target(*e, gm);

      if (logging::enabled) {
        // verbosity enabled
        std::cout << gm[vt].name << " ";
      }

      // sets[index]'s other elements are the characters adjacent to S
      sets[index].push_back(vt);
      // v_map[S]'s elements are the characters adjacent to S
      v_map[*v].push_back(vt);
    }

    if (logging::enabled) {
      // verbosity enabled
      std::cout << "}" << std::endl;
    }

    index++;
  }

  if (logging::enabled) {
    // verbosity enabled
    std::cout << std::endl;
  }

  // sort sets by size in ascending order
  std::sort(sets.begin(), sets.end(), ascending_size);

  bool first_iteration = true;
  for (size_t i = 0; i < sets.size(); ++i) {
    // for each set of characters
    if (sets[i].empty())
      continue;

    // v = species of gm
    RBVertex v = sets[i].front();

    // fill the list of characters names of v
    std::list<std::string> lcv;
    for (const RBVertex& cv : v_map[v]) {
      lcv.push_back(gm[cv].name);
    }

    if (first_iteration) {
      // first iteration of the loop:
      // add v to the Hasse diagram, and being the first vertex of the graph
      // there's no need to do any work.
      if (logging::enabled) {
        // verbosity enabled
        std::cout << "Hasse.addV " << gm[v].name << std::endl << std::endl;
      }

      add_vertex(gm[v].name, lcv, hasse);

      first_iteration = false;

      continue;
    }

    if (logging::enabled) {
      // verbosity enabled
      std::cout << "C(" << gm[v].name << ") = { ";

      for (const std::string& kk : lcv) {
        std::cout << kk << " ";
      }

      std::cout << "}:" << std::endl;
    }

    // new_edges will contain the list of edges that may be added to the Hasse
    // diagram: HDVertex is the source, std::string is the edge label
    std::list<std::pair<HDVertex, std::string>> new_edges;

    // check if there is a vertex with the same characters as v or if v needs
    // to be added to the Hasse diagram
    HDVertexIter hdv, hdv_end;
    std::tie(hdv, hdv_end) = vertices(hasse);
    for (; hdv != hdv_end; ++hdv) {
      // for each vertex in hasse
      if (logging::enabled) {
        // verbosity enabled
        std::cout << "hdv: ";

        for (const std::string& kk : hasse[*hdv].species) {
          std::cout << kk << " ";
        }

        std::cout << "= { ";

        for (const std::string& kk : hasse[*hdv].characters) {
          std::cout << kk << " ";
        }

        std::cout << "} -> ";
      }

      if (lcv == hasse[*hdv].characters) {
        // v and hdv have the same characters
        if (logging::enabled) {
          // verbosity enabled
          std::cout << "mod" << std::endl;
        }

        // add v to the list of species in hdv
        hasse[*hdv].species.push_back(gm[v].name);

        break;
      }

      std::list<std::string> lhdv = hasse[*hdv].characters;

      // TODO: check if edge building is done right

      // TODO: is_included might be superfluous
      // initialize new_edges if lhdv is a subset of lcv, with the structure:
      // *hdv -*ci-> v
      if (is_included(lhdv, lcv)) {
        // hdv is included in v
        for (const std::string& ci : lcv) {
          // for each character in hdv
          StringIter in;
          in = std::find(lhdv.begin(), lhdv.end(), ci);

          if (in == lhdv.end()) {
            // character is not present in lhdv
            new_edges.push_back(std::make_pair(*hdv, ci));
          }
        }
      }

      // on the last iteration of the cycle, after having collected all the
      // pairs in new_edges, add vertex and edges to the Hasse diagram
      if (std::next(hdv) == hdv_end) {
        // last iteration on the characters in the list has been performed
        if (logging::enabled) {
          // verbosity enabled
          std::cout << "add" << std::endl;
        }

        // build a vertex for v and add it to the Hasse diagram
        HDVertex u = add_vertex(gm[v].name, lcv, hasse);

        // build in_edges for the vertex and add them to the Hasse diagram
        for (const std::pair<HDVertex, std::string>& ei : new_edges) {
          // for each new edge to add to the Hasse diagram
          HDEdge edge;
          std::tie(edge, std::ignore) = add_edge(ei.first, u, hasse);
          hasse[edge].signedcharacters.push_back({ ei.second, State::gain });

          if (logging::enabled) {
            // verbosity enabled
            std::cout << "Hasse.addE ";

            for (const std::string& kk : hasse[ei.first].species) {
              std::cout << kk << " ";
            }

            std::cout << "-";

            SignedCharacterIter jj = hasse[edge].signedcharacters.begin();
            for (; jj != hasse[edge].signedcharacters.end(); ++jj) {
              std::cout << *jj;

              if (std::next(jj) != hasse[edge].signedcharacters.end())
                std::cout << ",";
            }

            std::cout << "-> ";

            for (const std::string& kk : hasse[u].species) {
              std::cout << kk << " ";
            }

            std::cout << std::endl;
          }
        }

        break;
      }
      else if (logging::enabled) {
        // verbosity enabled
        std::cout << "ignore";
      }

      if (logging::enabled) {
        // verbosity enabled
        std::cout << std::endl;
      }
    }

    if (logging::enabled) {
      // verbosity enabled
      std::cout << std::endl;
    }
  }

  // Store the graph pointer into the Hasse diagram's graph properties
  hasse[boost::graph_bundle].g = &g;

  // Store the maximal reducible graph pointer into the Hasse diagram's graph
  // properties
  hasse[boost::graph_bundle].gm = &gm;

  if (logging::enabled) {
    // verbosity enabled
    std::cout << "Before transitive reduction:" << std::endl
              << hasse << std::endl << std::endl;
  }

  // TODO: is this transitive reduction too simple?

  // transitive reduction of the Hasse diagram
  HDVertexIter u, u_end;
  std::tie(u, u_end) = vertices(hasse);
  for (; u != u_end; ++u) {
    if (in_degree(*u, hasse) == 0 || out_degree(*u, hasse) == 0)
      continue;
    // for each internal vertex in hasse

    HDInEdgeIter ie, ie_end;
    std::tie(ie, ie_end) = in_edges(*u, hasse);
    for (; ie != ie_end; ++ie) {
      // for each in-edge
      HDOutEdgeIter oe, oe_end;
      std::tie(oe, oe_end) = out_edges(*u, hasse);
      for (; oe != oe_end; ++oe) {
        // for each out-edge
        // source -> u -> target
        HDEdge e;
        bool exists;
        std::tie(e, exists) = edge(
          source(*ie, hasse), target(*oe, hasse), hasse
        );

        if (!exists)
          // no transitivity between source and target
          continue;

        // remove source -> target, which breaks the no-transitivty rule in
        // the Hasse diagram we need, because a path between source and target
        // already exists in the form: source -> u -> target
        remove_edge(e, hasse);
      }
    }
  }
}
