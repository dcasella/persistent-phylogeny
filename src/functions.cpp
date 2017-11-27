#include "functions.hpp"
#include "safe_chain_exception.hpp"
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
  #ifdef DEBUG
  std::cout << "initialize_vertex: [ ";
  
  std::list<std::string>::const_iterator si = hasse[v].species.begin();
  for (; si != hasse[v].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "]" << std::endl;
  #endif
  
  // ignore
}

void
safe_chain_dfs_visitor::start_vertex(const HDVertex v, const HDGraph& hasse) {
  #ifdef DEBUG
  std::cout << "start_vertex: [ ";
  
  std::list<std::string>::const_iterator si = hasse[v].species.begin();
  for (; si != hasse[v].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "]" << std::endl;
  #endif
  
  source_v = v;
}

void
safe_chain_dfs_visitor::discover_vertex(
    const HDVertex v,
    const HDGraph& hasse) {
  #ifdef DEBUG
  std::cout << "discover_vertex: [ ";
  
  std::list<std::string>::const_iterator si = hasse[v].species.begin();
  for (; si != hasse[v].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "]" << std::endl;
  #endif
  
  last_v = v;
}

void
safe_chain_dfs_visitor::examine_edge(const HDEdge e, const HDGraph& hasse) {
  #ifdef DEBUG
  HDVertex vs, vt;
  std::tie(vs, vt) = incident(e, hasse);
  
  std::cout << "examine_edge: [ ";
  
  std::list<std::string>::const_iterator si = hasse[vs].species.begin();
  for (; si != hasse[vs].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "] -";
  
  SignedCharacterIter sci = hasse[e].signedcharacters.begin();
  for (; sci != hasse[e].signedcharacters.end(); ++sci) {
    std::cout << *sci;
    
    if (std::next(sci) != hasse[e].signedcharacters.end())
      std::cout << ",";
  }
  
  std::cout << "-> [ ";
  
  si = hasse[vt].species.begin();
  for (; si != hasse[vt].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "]" << std::endl;
  #endif
  
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
  #ifdef DEBUG
  HDVertex vs, vt;
  std::tie(vs, vt) = incident(e, hasse);
  
  std::cout << "tree_edge: [ ";
  
  std::list<std::string>::const_iterator si = hasse[vs].species.begin();
  for (; si != hasse[vs].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "] -";
  
  SignedCharacterIter sci = hasse[e].signedcharacters.begin();
  for (; sci != hasse[e].signedcharacters.end(); ++sci) {
    std::cout << *sci;
    
    if (std::next(sci) != hasse[e].signedcharacters.end())
      std::cout << ",";
  }
  
  std::cout << "-> [ ";
  
  si = hasse[vt].species.begin();
  for (; si != hasse[vt].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "]" << std::endl;
  #endif
  
  // ignore
}

void
safe_chain_dfs_visitor::back_edge(const HDEdge e, const HDGraph& hasse) const {
  #ifdef DEBUG
  HDVertex vs, vt;
  std::tie(vs, vt) = incident(e, hasse);
  
  std::cout << "back_edge: [ ";
  
  std::list<std::string>::const_iterator si = hasse[vs].species.begin();
  for (; si != hasse[vs].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "] -";
  
  SignedCharacterIter sci = hasse[e].signedcharacters.begin();
  for (; sci != hasse[e].signedcharacters.end(); ++sci) {
    std::cout << *sci;
    
    if (std::next(sci) != hasse[e].signedcharacters.end())
      std::cout << ",";
  }
  
  std::cout << "-> [ ";
  
  si = hasse[vt].species.begin();
  for (; si != hasse[vt].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "]" << std::endl;
  #endif
  
  // ignore
}

void
safe_chain_dfs_visitor::forward_or_cross_edge(
    const HDEdge e,
    const HDGraph& hasse) const {
  #ifdef DEBUG
  HDVertex vs, vt;
  std::tie(vs, vt) = incident(e, hasse);
  
  std::cout << "forward_or_cross_edge: [ ";
  
  std::list<std::string>::const_iterator si = hasse[vs].species.begin();
  for (; si != hasse[vs].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "] -";
  
  SignedCharacterIter sci = hasse[e].signedcharacters.begin();
  for (; sci != hasse[e].signedcharacters.end(); ++sci) {
    std::cout << *sci;
    
    if (std::next(sci) != hasse[e].signedcharacters.end())
      std::cout << ",";
  }
  
  std::cout << "-> [ ";
  
  si = hasse[vt].species.begin();
  for (; si != hasse[vt].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "]" << std::endl;
  #endif
  
  // ignore
}

void
safe_chain_dfs_visitor::finish_vertex(const HDVertex v, const HDGraph& hasse) {
  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // orig_g or orig_gm are uninitialized
    return;
  
  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);
  
  #ifdef DEBUG
  std::cout << "finish_vertex: [ ";
  
  std::list<std::string>::const_iterator si = hasse[v].species.begin();
  for (; si != hasse[v].species.end(); ++si) {
    std::cout << *si << " ";
  }
  
  std::cout << "]" << std::endl;
  #endif
  
  if (m_lsc == nullptr)
    // m_lsc is uninitialized
    return;
  
  if (last_v != v)
    // v is not the last vertex in the chain, which means the visit is
    // backtracking, so ignore it and keep going
    return;
  
  // source_v holds the source vertex of the chain
  // m_lsc holds the list of SignedCharacters representing the chain
  
  #ifdef DEBUG
  std::cout << "S(C): < ";
  
  SignedCharacterIter sci = m_lsc->begin();
  for (; sci != m_lsc->end(); ++sci) {
    std::cout << *sci << " ";
  }
  
  std::cout << ">" << std::endl;
  #endif
  
  // TODO: confirm this is correct
  if (m_lsc->empty()) {
    // m_lsc being empty means it's not actually a chain
    #ifdef DEBUG
    std::cout << "Empty chain" << std::endl;
    #endif
    
    return;
  }
  
  RBGraph gm_test;
  RBVertexIMap map_index;
  RBVertexIAssocMap i_map(map_index);
  
  // fill the vertex index map i_map
  RBVertexIter u, u_end;
  std::tie(u, u_end) = vertices(gm);
  for (size_t index = 0; u != u_end; ++u, ++index) {
    boost::put(i_map, *u, index);
  }
  
  // copy gm to gm_test
  copy_graph(gm, gm_test, boost::vertex_index_map(i_map));
  
  // update gm_test's number of species and characters
  num_species(gm_test) = num_species(gm);
  num_characters(gm_test) = num_characters(gm);
  
  // test if m_lsc is a safe chain
  std::tie(*m_lsc, std::ignore) = realize(*m_lsc, gm_test);
  
  #ifdef DEBUG
  std::cout << gm_test << std::endl << std::endl;
  #endif
  
  if (is_redsigma(gm_test)) {
    #ifdef DEBUG
    std::cout << "Found red Σ-graph" << std::endl;
    #endif
    
    m_lsc->clear();
    return;
  }
  
  #ifdef DEBUG
  std::cout << "No red Σ-graph" << std::endl;
  #endif
  
  // TODO: test safe source?
  
  throw SafeChain();
}


//=============================================================================
// Algorithm functions

bool safe_source(const HDVertex v, const HDGraph& hasse) {
  if (orig_g(hasse) == nullptr || orig_gm(hasse) == nullptr)
    // uninitialized graph properties
    return false;
  
  RBGraph g_test;
  RBVertexMap map_vertex;
  RBVertexIMap map_index;
  RBVertexAssocMap v_map(map_vertex);
  RBVertexIAssocMap i_map(map_index);
  
  // retrieve graphs and maps from hasse
  RBGraph g = *orig_g(hasse);
  RBGraph gm = *orig_gm(hasse);
  
  // fill the vertex index map i_map
  RBVertexIter u, u_end;
  std::tie(u, u_end) = vertices(g);
  for (size_t index = 0; u != u_end; ++u, ++index) {
    boost::put(i_map, *u, index);
  }
  
  // copy g to g_test, fill the vertex map v_map (and map_vertex)
  copy_graph(
    g, g_test, boost::vertex_index_map(i_map).orig_to_copy(v_map)
  );
  
  // update g_test's number of species and characters
  num_species(g_test) = num_species(g);
  num_characters(g_test) = num_characters(g);
  
  // realize the list of species of v in g_test
  std::list<std::string>::const_iterator i = hasse[v].species.begin();
  for (; i != hasse[v].species.end(); ++i) {
    // for each species (name) in v
    RBVertexIter u, u_end, in;
    std::tie(u, u_end) = vertices(g_test);
    // get the corresponding vertex in g_test
    in = find_vertex(u, u_end, *i, g_test);
    
    if (in == u_end)
      return false;
    
    // realize the species (= realize its characters)
    realize(*in, g_test);
  }
  
  // if the realization didn't induce a red Σ-graph, v is a safe source
  return !is_redsigma(g_test);
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
    #ifdef DEBUG
    std::cout << "Safe chain: < ";
    
    SignedCharacterIter sci = output.begin();
    for (; sci != output.end(); ++sci) {
      std::cout << *sci << " ";
    }
    
    std::cout << ">" << std::endl << std::endl;
    #endif
    
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
  
  #ifdef DEBUG
  std::cout << std::endl
            << "Working on G:" << std::endl
            << g << std::endl << std::endl;
  #endif
  
  // cleanup graph from dead vertices
  // TODO: check if this is needed (realize already does this?)
  remove_singletons(g);
  
  if (is_empty(g)) {
    // if graph is empty
    // return the empty sequence
    #ifdef DEBUG
    std::cout << "G empty"
              << std::endl << std::endl;
    #endif
    
    // return < >
    return output;
  }
  
  #ifdef DEBUG
  std::cout << "G not empty" << std::endl;
  #endif
  
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
      #ifdef DEBUG
      std::cout << "G free character " << g[*v].name
                << std::endl << std::endl;
      #endif
      
      std::list<SignedCharacter> lsc;
      std::tie(lsc, std::ignore) = realize({ g[*v].name, State::lose }, g);
      
      output.splice(output.end(), lsc);
      output.splice(output.end(), reduce(g));
      
      // return < v-, reduce(g) >
      return output;
    }
  }
  
  #ifdef DEBUG
  std::cout << "G no free characters" << std::endl;
  #endif
  
  // realize universal characters in the graph
  // TODO: check if this is needed (realize already does this?)
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_universal(*v, g)) {
      // if v is universal
      // realize v+
      // return < v+, reduce(g) >
      #ifdef DEBUG
      std::cout << "G universal character " << g[*v].name
                << std::endl << std::endl;
      #endif
      
      std::list<SignedCharacter> lsc;
      std::tie(lsc, std::ignore) = realize({ g[*v].name, State::gain }, g);
      
      output.splice(output.end(), lsc);
      output.splice(output.end(), reduce(g));
      
      // return < v+, reduce(g) >
      return output;
    }
  }
  
  #ifdef DEBUG
  std::cout << "G no universal characters" << std::endl;
  #endif
  
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
  RBGraph gm = maximal_reducible_graph(gm);
  
  #ifdef DEBUG
  std::cout << "Gm:" << std::endl << gm << std::endl << std::endl;
  #endif
  
  // p = Hasse diagram for gm (Grb|Cm)
  HDGraph p;
  hasse_diagram(p, g, gm);
  
  #ifdef DEBUG
  std::cout << "Hasse:" << std::endl
            << p << std::endl << std::endl;
  #endif
  
  if (num_edges(p) == 0)
    // no edges means there are no chains in the Hasse diagram
    throw std::runtime_error("Could not reduce graph");
  
  // TODO: understand safesource/safechain correlation
  // sc = safe chain for g (Grb)
  std::list<SignedCharacter> sc;
  bool s_safe;
  std::tie(sc, s_safe) = safe_chain(p);
  
  if (!s_safe)
    // p has no safe [source|chain]
    throw std::runtime_error("Could not reduce graph");
  
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
  
  // find the vertex in g whose name is sc.character
  RBVertexIter v, v_end, in;
  std::tie(v, v_end) = vertices(g);
  in = find_vertex(v, v_end, sc.character, g);
  
  if (in == v_end)
    // g has no vertex named sc.character
    return std::make_pair(output, false);
  
  // cv = current character vertex
  RBVertex cv = *in;
  
  RBVertexIMap map_index, map_comp;
  RBVertexIAssocMap i_map(map_index), c_map(map_comp);
  
  // fill vertex index map
  std::tie(v, v_end) = vertices(g);
  for (size_t index = 0; v != v_end; ++v, ++index) {
    boost::put(i_map, *v, index);
  }
  
  // build the components map
  boost::connected_components(g, c_map, boost::vertex_index_map(i_map));
  
  if (sc.state == State::gain && is_inactive(cv, g)) {
    // c+ and c is inactive
    #ifdef DEBUG
    std::cout << sc << " (+ and inactive): ";
    #endif
    
    // realize the character c+:
    // - add a red edge between c and each species in D(c) \ N(c)
    // - delete all black edges incident on c
    std::tie(v, v_end) = vertices(g);
    for (; v != v_end; ++v) {
      if (is_character(*v, g) || map_comp[*v] != map_comp[cv])
        continue;
      // for each species in the same connected component of cv
      
      #ifdef DEBUG
      std::cout << g[*v].name << " -> ";
      #endif
      
      RBEdge e;
      bool exists;
      std::tie(e, exists) = edge(*v, cv, g);
      
      if (exists) {
        // there is an edge (black) between *v and cv
        #ifdef DEBUG
        std::cout << "del; ";
        #endif
        
        remove_edge(e, g);
      }
      else {
        // there isn't an edge between *v and cv
        #ifdef DEBUG
        std::cout << "red; ";
        #endif
        
        add_edge(*v, cv, Color::red, g);
      }
    }
    
    #ifdef DEBUG
    std::cout << std::endl;
    #endif
  }
  else if (sc.state == State::lose && is_active(cv, g)) {
    // c- and c is active
    #ifdef DEBUG
    std::cout << sc << " (- and active)" << std::endl;
    #endif
    
    // realize the character c-:
    // - delete all edges incident on c
    clear_vertex(cv, g);
  }
  else {
    #ifdef DEBUG
    std::cout << "Could not realize " << sc << std::endl;
    #endif
    
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
      #ifdef DEBUG
      std::cout << "G universal character " << g[*v].name << std::endl;
      #endif
      
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
      #ifdef DEBUG
      std::cout << "G free character " << g[*v].name << std::endl;
      #endif
      
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
