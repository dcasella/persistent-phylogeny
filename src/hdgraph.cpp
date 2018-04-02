#include "hdgraph.hpp"
#include <boost/graph/graph_utility.hpp>


//=============================================================================
// Boost functions (overloading)

// Hasse Diagram

HDVertex
add_vertex(
    const std::list<int>& species,
    const std::list<int>& characters,
    HDGraph& hasse) {
  const HDVertex v = boost::add_vertex(hasse);
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

    for (const int& i : hasse[*v].species) {
      os << "s" << i << " ";
    }

    os << "( ";

    for (const int& i : hasse[*v].characters) {
      os << "c" << i << " ";
    }

    os << ") ]:";

    HDOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, hasse);
    for (; e != e_end; ++e) {
      const HDVertex vt = target(*e, hasse);

      os << " -";

      SignedCharacterIter j = hasse[*e].signedcharacters.cbegin();
      for (; j != hasse[*e].signedcharacters.cend(); ++j) {
        os << *j;

        if (std::next(j) != hasse[*e].signedcharacters.cend())
          os << ",";
      }

      os << "-> [ ";

      for (const int& i : hasse[vt].species) {
        os << "s" << i << " ";
      }

      os << "( ";

      for (const int& i : hasse[vt].characters) {
        os << "c" << i << " ";
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
is_included(const std::list<int>& a, const std::list<int>& b) {
  for (const int& a_element : a) {
    if (std::find(b.cbegin(), b.cend(), a_element) == b.cend())
      // exit the function at the first element of a not present in b
      return false;
  }

  return true;
}

void hasse_diagram(HDGraph& hasse, const RBGraph& g, const RBGraph& gm) {
  std::vector<std::list<RBVertex>> vec_adj_char(num_species(gm));
  std::map<RBVertex, std::list<RBVertex>> adj_char;

  // how vec_adj_char is going to be structured:
  // vec_adj_char[index] => < S, List of characters adjacent to S >

  // how adj_char is going to be structured:
  // adj_char[S] => < List of characters adjacent to S >

  // vec_adj_char is used to sort the lists by number of elements, this is why we store
  // the list of adjacent characters to S. While we store S to be able to
  // access adj_char[S] in costant time, instead of iterating on vec_adj_char to find the
  // correct list

  // initialize vec_adj_char and adj_char for each species in the graph
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(gm);
  for (size_t index = 0; v != v_end; ++v) {
    if (!is_species(*v, gm))
      continue;
    // for each species vertex

    // vec_adj_char[index]'s first element is the species vertex
    vec_adj_char[index].push_back(*v);

    // build v's set of adjacent characters
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, gm);
    for (; e != e_end; ++e) {
      // ignore active characters
      if (is_red(*e, gm))
        continue;

      // vt = one of the characters adjacent to *v
      const RBVertex vt = target(*e, gm);

      // vec_adj_char[index]'s other elements are the characters adjacent to S
      vec_adj_char[index].push_back(vt);
      // adj_char[S]'s elements are the characters adjacent to S
      adj_char[*v].push_back(vt);
    }

    // if the species *v would have 0 characters, ignore it
    if (vec_adj_char[index].size() == 1) {
      vec_adj_char[index].clear();

      continue;
    }

    index++;
  }

  auto compare_size = [](const std::list<RBVertex>& a,
                         const std::list<RBVertex>& b) {
    return a.size() < b.size();
  };

  // sort vec_adj_char by size in ascending order
  std::sort(vec_adj_char.begin(), vec_adj_char.end(), compare_size);

  bool first_iteration = true;
  for (const std::list<RBVertex>& set : vec_adj_char) {
    // for each set of characters
    if (set.empty())
      continue;

    // v = species of gm
    const RBVertex v = set.front();

    // fill the list of characters names of v
    std::list<int> lcv;
    for (const RBVertex& cv : adj_char[v]) {
      lcv.push_back(gm[cv].index);
    }

    lcv.sort();

    if (first_iteration) {
      // first iteration of the loop:
      // add v to the Hasse diagram, and being the first vertex of the graph
      // there's no need to do any work
      add_vertex(gm[v].index, lcv, hasse);

      first_iteration = false;

      continue;
    }

    // new_edges will contain the list of edges that may be added to the Hasse
    // diagram: HDVertex is the source, int is the edge label
    std::list<std::pair<HDVertex, int>> new_edges;

    // check if there is a vertex with the same characters as v or if v needs
    // to be added to the Hasse diagram
    HDVertexIter hdv, hdv_end;
    std::tie(hdv, hdv_end) = vertices(hasse);
    for (; hdv != hdv_end; ++hdv) {
      // for each vertex in hasse

      if (lcv == hasse[*hdv].characters) {
        // v and hdv have the same characters

        // add v to the list of species in hdv
        hasse[*hdv].species.push_back(gm[v].index);

        break;
      }

      const std::list<int> lhdv = hasse[*hdv].characters;

      // TODO: check if edge building is done right

      // TODO: is_included might be superfluous
      // initialize new_edges if lhdv is a subset of lcv, with the structure:
      // *hdv -*ci-> v
      if (is_included(lhdv, lcv)) {
        // hdv is included in v
        for (const int& ci : lcv) {
          // for each character in hdv
          IntIter in = std::find(lhdv.cbegin(), lhdv.cend(), ci);

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

        // build a vertex for v and add it to the Hasse diagram
        const HDVertex u = add_vertex(gm[v].index, lcv, hasse);

        // build in_edges for the vertex and add them to the Hasse diagram
        for (const std::pair<HDVertex, int>& ei : new_edges) {
          // for each new edge to add to the Hasse diagram
          HDEdge edge;
          std::tie(edge, std::ignore) = add_edge(ei.first, u, hasse);
          hasse[edge].signedcharacters.push_back({ ei.second, State::gain });
        }

        break;
      }
    }
  }

  // Store the graph pointer into the Hasse diagram's graph properties
  hasse[boost::graph_bundle].g = &g;

  // Store the maximal reducible graph pointer into the Hasse diagram's graph
  // properties
  hasse[boost::graph_bundle].gm = &gm;

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

  // sort species names in each vertex
  std::tie(u, u_end) = vertices(hasse);
  for (; u != u_end; ++u) {
    std::list<int> species = hasse[*u].species;

    species.sort();

    hasse[*u].species.clear();
    hasse[*u].species.splice(hasse[*u].species.cend(), species);
  }
}
