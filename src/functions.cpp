#include "functions.hpp"
#include <fstream>


//=============================================================================
// Boost functions (overloading)

// Red-Black Graph

void remove_vertex(const RBVertex v, RBGraph& g) {
  if (g[v].type == Type::species)
    num_species(g)--;
  else if (g[v].type == Type::character)
    num_characters(g)--;
  
  boost::remove_vertex(v, g);
}

RBVertex add_vertex(const std::string& name, const Type type, RBGraph& g) {
  RBVertex v = boost::add_vertex(g);
  g[v].name = name;
  g[v].type = type;
  
  if (g[v].type == Type::species)
    num_species(g)++;
  else if (g[v].type == Type::character)
    num_characters(g)++;
  
  return v;
}

std::pair<RBEdge, bool> add_edge(const RBVertex u, const RBVertex v,
                                 const Color color, RBGraph& g) {
  RBEdge e;
  bool exists;
  std::tie(e, exists) = boost::add_edge(u, v, g);
  g[e].color = color;
  
  return std::make_pair(e, exists);
}

// Hasse Diagram

HDVertex add_vertex(const std::list<std::string>& species,
                    const std::list<std::string>& characters, HDGraph& hasse) {
  HDVertex v = boost::add_vertex(hasse);
  hasse[v].species = species;
  hasse[v].characters = characters;
  
  return v;
}

std::pair<HDEdge, bool> add_edge(const HDVertex u, const HDVertex v,
                                 const std::list<CharacterState>& lcs,
                                 HDGraph& hasse) {
  HDEdge e;
  bool exists;
  std::tie(e, exists) = boost::add_edge(u, v, hasse);
  hasse[e].lcs = lcs;
  
  return std::make_pair(e, exists);
}


//=============================================================================
// General functions

// Red-Black Graph

void print_rbgraph(const RBGraph& g) {
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    std::cout << g[*v].name << ":";
    
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      std::cout << " -" << (g[*e].color == Color::red ? "r" : "-") << "- "
                << g[target(*e, g)].name
                << ";";
    }
    
    std::cout << std::endl;
  }
}

// File I/O

void read_graph(const std::string& filename, RBGraph& g) {
  std::vector<RBVertex> species, characters;
  bool first_line = true;
  std::string line;
  std::ifstream file(filename);
  
  size_t idx = 0;
  while (std::getline(file, line)) {
    // for each line in file
    std::istringstream iss(line);
    
    if (first_line) {
      // read rows and columns (species and characters)
      size_t num_s, num_c;
      
      iss >> num_s;
      iss >> num_c;
      
      species.resize(num_s);
      characters.resize(num_c);
      
      if (species.size() == 0 || characters.size() == 0) {
        // input file parsing error
        throw std::runtime_error(
          "Failed to read graph from file: badly formatted line 0"
        );
      }
      
      // insert species in the graph
      for (size_t j = 0; j < species.size(); ++j) {
        species[j] = add_vertex("s" + std::to_string(j + 1), Type::species, g);
      }
      
      // insert characters in the graph
      for (size_t j = 0; j < characters.size(); ++j) {
        characters[j] = add_vertex("c" + std::to_string(j + 1),
                                   Type::character, g);
      }
      
      first_line = false;
    }
    else {
      char value;
      
      // read binary matrix
      while (iss >> value) {
        bool red_edge = false;
        
        switch (value) {
          #ifdef DEBUG
          case '2':
            // permit red edges from input matrix only if debugging
            red_edge = true;
          #endif
          
          case '1': {
            // add edge between species[s_idx] and characters[c_idx]
            size_t s_idx = idx / characters.size(),
                   c_idx = idx % characters.size();
            
            if (s_idx >= species.size() || c_idx >= characters.size()) {
              // input file parsing error
              throw std::runtime_error(
                "Failed to read graph from file: oversized matrix"
              );
            }
            
            RBEdge edge;
            std::tie(edge, std::ignore) = add_edge(species[s_idx],
                                                   characters[c_idx], g);
            
            if (red_edge)
              g[edge].color = Color::red;
            
            break;
          }
          
          case '0':
            break;
          
          default:
            // input file parsing error
            throw std::runtime_error(
              "Failed to read graph from file: unexcepted value in matrix"
            );
        }
        
        ++idx;
      }
    }
  }
  
  if (species.size() == 0 || characters.size() == 0) {
    // input file parsing error
    throw std::runtime_error(
      "Failed to read graph from file: empty file"
    );
  }
}

// Hasse Diagram

void print_hdgraph(const HDGraph& g) {
  HDVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    std::cout << "[ ";
    
    std::list<std::string>::const_iterator i = g[*v].species.begin();
    for (; i != g[*v].species.end(); ++i)
      std::cout << *i << " ";
    
    std::cout << "( ";
    
    i = g[*v].characters.begin();
    for (; i != g[*v].characters.end(); ++i)
      std::cout << *i << " ";
    
    std::cout << ") ]:";
    
    HDOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      HDVertex vt = target(*e, g);
      
      std::cout << " -";
      
      std::list<CharacterState>::const_iterator j = g[*e].lcs.begin();
      for (; j != g[*e].lcs.end(); ++j) {
        std::cout << *j;
        
        if (std::next(j) != g[*e].lcs.end())
          std::cout << ",";
      }
      
      std::cout << "-> [ ";
      
      i = g[vt].species.begin();
      for (; i != g[vt].species.end(); ++i)
        std::cout << *i << " ";
      
      std::cout << "( ";
      
      i = g[vt].characters.begin();
      for (; i != g[vt].characters.end(); ++i)
        std::cout << *i << " ";
      
      std::cout << ") ];";
    }
    
    std::cout << std::endl;
  }
}


//=============================================================================
// Algorithm functions

// Red-Black Graph

bool is_active(const RBVertex v, const RBGraph& g) {
  if (g[v].type != Type::character)
    return false;
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (g[*e].color == Color::black || g[target(*e, g)].type != Type::species)
      return false;
  }
  
  return true;
}

bool is_inactive(const RBVertex v, const RBGraph& g) {
  if (g[v].type != Type::character)
    return false;
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (g[*e].color == Color::red || g[target(*e, g)].type != Type::species)
      return false;
  }
  
  return true;
}

void remove_singletons(RBGraph& g) {
  RBVertexIter v, v_end, next;
  std::tie(v, v_end) = vertices(g);
  for (next = v; v != v_end; v = next) {
    ++next;
    remove_vertex_if(*v, if_singleton(), g);
  }
}

bool is_free(const RBVertex v, const RBGraph& g) {
  if (g[v].type != Type::character)
    return false;
  
  size_t count_species = 0;
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (g[*e].color == Color::black || g[target(*e, g)].type != Type::species)
      return false;
    
    count_species++;
  }
  
  if (count_species != num_species(g))
    return false;
  
  return true;
}

bool is_universal(const RBVertex v, const RBGraph& g) {
  if (g[v].type != Type::character)
    return false;
  
  size_t count_species = 0;
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (g[*e].color == Color::red || g[target(*e, g)].type != Type::species)
      return false;
    
    count_species++;
  }
  
  if (count_species != num_species(g))
    return false;
  
  return true;
}

size_t connected_components(RBGraphVector& components, const RBGraph& g) {
  size_t num_comps;
  RBIndexMap map_index, map_comp;
  RBAssocMap i_map(map_index), c_map(map_comp);
  
  // fill vertex index map
  RBVertexIter v, v_end;
  size_t idx = 0;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v, ++idx) {
    boost::put(i_map, *v, idx);
  }
  
  // get number of components and the components map
  num_comps = boost::connected_components(g, c_map,
                                          boost::vertex_index_map(i_map));
  
  if (num_comps > 1) {
    // graph is disconnected
    // resize subgraph components
    components.resize(num_comps);
    // vertices: graphVertex => compVertex
    std::map<RBVertex, RBVertex> vertices;
    
    // initialize subgraph components
    for (size_t i = 0; i < num_comps; ++i) {
      components[i] = std::make_unique<RBGraph>();
    }
    
    // add vertices to their respective subgraph
    RBIndexMap::iterator i = map_comp.begin();
    for (; i != map_comp.end(); ++i) {
      // for each vertex
      RBVertex v = i->first;
      RBVertexSize comp = i->second;
      RBGraph* component = components[comp].get();
      
      vertices[v] = add_vertex(g[v].name, g[v].type, *component);
      
      if (g[v].type == Type::species)
        num_species(*component)++;
      else
        num_characters(*component)++;
    }
    
    // add edges to their respective vertices and subgraph
    i = map_comp.begin();
    for (; i != map_comp.end(); ++i) {
      // for each vertex
      RBVertex v = i->first;
      RBVertex new_v = vertices[v];
      RBVertexSize comp = i->second;
      RBGraph* component = components[comp].get();
      
      // prevent duplicate edges
      // if (g[v].type != Type::species)
      //   continue;
      
      RBOutEdgeIter e, e_end;
      std::tie(e, e_end) = out_edges(v, g);
      for (; e != e_end; ++e) {
        // for each out edge
        RBVertex new_vt = vertices[target(*e, g)];
        
        RBEdge edge;
        std::tie(edge, std::ignore) = add_edge(new_v, new_vt, g[*e].color,
                                               *component);
      }
    }
    
    #ifdef DEBUG
    std::cout << "Connected components:" << std::endl;
    for (size_t i = 0; i < num_comps; ++i) {
      print_rbgraph(*components[i].get());
      std::cout << std::endl;
    }
    #endif
  }
  #ifdef DEBUG
  else {
    std::cout << "G connected" << std::endl;
  }
  #endif
  
  return num_comps;
}

std::list<RBVertex> maximal_characters(const RBGraph& g) {
  std::list<RBVertex> cm;
  std::map<RBVertex, std::list<RBVertex>> sets;
  size_t count_incl, count_excl;
  bool keep_char, skip_cycle;
  
  /* How sets is going to be structured:
   * sets[C] => < List of adjacent species to C >
   */
  
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (g[*v].type != Type::character)
      continue;
    // for each character vertex
    
    // build v's set of adjacent species
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      RBVertex vt = target(*e, g);
      
      // if v is active or connected to random nodes ignore it
      if (g[*e].color == Color::red || g[vt].type != Type::species)
        break;
      
      sets[*v].push_back(vt);
    }
    
    if (cm.empty()) {
      // cm being empty means v can be added without further checks
      cm.push_back(*v);
      continue;
    }
    
    #ifdef DEBUG
    std::cout << g[*v].name << std::endl;
    #endif
    
    // sets[*v] now contains the list of species adjacent to v
    
    skip_cycle = false;
    
    // check if sets[*v] is subset of the species adjacent to cmv
    RBVertexIter cmv = cm.begin(), cmv_end = cm.end();
    for (; cmv != cmv_end; ++cmv) {
      // for each species in cm
      if (skip_cycle)
        break;
      
      #ifdef DEBUG
      std::cout << "curr Cm: " << g[*cmv].name << " = { ";
      RBVertexIter kk = sets[*cmv].begin(), kk_end = sets[*cmv].end();
      for (; kk != kk_end; ++kk) std::cout << g[*kk].name << " ";
      std::cout << "}:" << std::endl;
      #endif
      
      count_incl = 0; count_excl = 0;
      keep_char = false;
      
      RBVertexIter sv = sets[*v].begin(), sv_end = sets[*v].end();
      for (; sv != sv_end; ++sv) {
        // for each species adjacent to v, S(C#)
        #ifdef DEBUG
        std::cout << "S(" << g[*v].name << "): " << g[*sv].name << " -> ";
        #endif
        
        // find sv in the list of cmv's adjacent species
        RBVertexIter in = std::find(sets[*cmv].begin(), sets[*cmv].end(),
                                    *sv);
        
        /* keep count of how many species are included (or not found) in
         * the list of cmv's adjacent species
         */
        if (in != sets[*cmv].end())
          count_incl++;
        else
          count_excl++;
        
        #ifdef DEBUG
        std::cout << count_incl << " " << count_excl;
        #endif
        
        if (std::next(sv) == sv_end) {
          // last iteration on the species in the list has been performed
          if (count_incl == sets[*cmv].size() && count_excl > 0) {
            /* the list of adjacent species to v is a superset of the list of
             * adjacent species to cmv, which means cmv can be replaced
             * by v in the list of maximal characters Cm
             */
            #ifdef DEBUG
            std::cout << " subst" << std::endl;
            #endif
            
            cm.push_front(*v);
            cm.remove(*(cmv++));
            skip_cycle = true;
            break;
          }
          else if (count_incl < sets[*cmv].size() && count_excl > 0) {
            /* the list of adjacent species to v is neither a superset nor a
             * subset of the list of adjacent species to cmv, which means
             * v may be a new maximal character
             */
            #ifdef DEBUG
            std::cout << " add, not subset";
            #endif
            
            keep_char = true;
          }
          else if (count_incl == sets[*cmv].size()) {
            /* the list of adjacent species to v is the same as the list of
             * adjacent species to cmv, so v can be ignored in the next
             * iterations on the characters in Cm
             */
            #ifdef DEBUG
            std::cout << " ignore, same set" << std::endl;
            #endif
            
            skip_cycle = true;
            break;
          }
          else if (count_excl == 0) {
            /* the list of adjacent species to v is a subset of the list of
             * adjacent species to cmv, meaning v can be ignored in the
             * next iterations on the characters in Cm
             */
            #ifdef DEBUG
            std::cout << " ignore, subset" << std::endl;
            #endif
            
            skip_cycle = true;
            break;
          }
          else {
            // how we ended up here nobody knows
            #ifdef DEBUG
            std::cout << " idk";
            #endif
          }
        }
        
        #ifdef DEBUG
        std::cout << std::endl;
        #endif
      }
      
      if (std::next(cmv) == cmv_end) {
        // last iteration on the characters in the list has been performed
        if (keep_char) {
          /* after all the iterations keep_char is true if the list of adjacent
           * species to v is neither a superset nor a subset of the lists of
           * adjacent species to the characters in Cm, so v is a maximal
           * characters and can be added to the list of maximal characters Cm
           */
          cm.push_front(*v);
          #ifdef DEBUG
          std::cout << "\tadd" << std::endl;
          #endif
        }
      }
      
      #ifdef DEBUG
      std::cout << std::endl;
      #endif
    }
  }
  
  return cm;
}

// TODO: test which one is faster

std::list<RBVertex> maximal_characters2(const RBGraph& g) {
  std::list<RBVertex> cm;
  std::vector<std::list<RBVertex>> sets(num_characters(g));
  std::map<RBVertex, std::list<RBVertex>> v_map;
  size_t count_incl, count_excl;
  bool keep_char, skip_cycle;
  
  /* How sets is going to be structured:
   * sets[index] => < C, List of adjacent species to C >
   *
   * sets is used to sort the lists by number of elements, this is why we store
   * the list of adjacent species to C. While we store C to be able to access
   * v_map[C] in costant time
   *
   * How v_map is going to be structured:
   * v_map[C] => < List of adjacent species to C >
   */
  
  size_t idx = 0;
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (g[*v].type != Type::character)
      continue;
    // for each character vertex
    
    sets[idx].push_back(*v);
    
    // build v's set of adjacent species
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      RBVertex vt = target(*e, g);
      
      // if v is active or connected to random nodes ignore it
      if (g[*e].color == Color::red || g[vt].type != Type::species)
        break;
      
      sets[idx].push_back(vt);
      v_map[*v].push_back(vt);
    }
    
    ++idx;
  }
  
  // sort sets by size in descending order
  std::sort(sets.begin(), sets.end(), descending_size);
  
  for (size_t i = 0; i < sets.size(); ++i) {
    // for each set of species
    RBVertex v = sets[i].front();
    
    if (sets[i].size() == sets[0].size()) {
      // both sets[i] and sets[0] are maximal
      // or i = 0, which still means sets[i] and sets[0] maximal
      cm.push_back(v);
      continue;
    }
    
    #ifdef DEBUG
    std::cout << g[v].name << std::endl;
    #endif
    
    skip_cycle = false;
    
    // check if sc is subset of the species adjacent to cmv
    RBVertexIter cmv = cm.begin(), cmv_end = cm.end();
    for (; cmv != cmv_end; ++cmv) {
      // for each species in cm
      if (skip_cycle)
        break;
      
      #ifdef DEBUG
      std::cout << "curr Cm: " << g[*cmv].name << " = { ";
      RBVertexIter kk = v_map[*cmv].begin(), kk_end = v_map[*cmv].end();
      for (; kk != kk_end; ++kk) std::cout << g[*kk].name << " ";
      std::cout << "}:" << std::endl;
      #endif
      
      count_incl = 0; count_excl = 0;
      keep_char = false;
      
      RBVertexIter sv = v_map[v].begin(), sv_end = v_map[v].end();
      for (; sv != sv_end; ++sv) {
        // for each species adjacent to v, S(C#)
        #ifdef DEBUG
        std::cout << "S(" << g[v].name << "): " << g[*sv].name << " -> ";
        #endif
        
        // find sv in the list of cmv's adjacent species
        RBVertexIter in = std::find(v_map[*cmv].begin(), v_map[*cmv].end(),
                                    *sv);
        
        /* keep count of how many species are included (or not found) in
         * the list of cmv's adjacent species
         */
        if (in != v_map[*cmv].end())
          count_incl++;
        else
          count_excl++;
        
        #ifdef DEBUG
        std::cout << count_incl << " " << count_excl;
        #endif
        
        if (std::next(sv) == sv_end) {
          // last iteration on the species in the list has been performed
          if (count_incl < v_map[*cmv].size() && count_excl > 0) {
            /* the list of adjacent species to v is neither a superset nor a
             * subset of the list of adjacent species to cmv, which means
             * v may be a new maximal character
             */
            #ifdef DEBUG
            std::cout << " add, not subset";
            #endif
            
            keep_char = true;
          }
          else if (count_incl == v_map[*cmv].size()) {
            /* the list of adjacent species to v is the same as the list of
             * adjacent species to cmv, so v can be ignored in the next
             * iterations on the characters in Cm
             */
            #ifdef DEBUG
            std::cout << " ignore, same set" << std::endl;
            #endif
            
            skip_cycle = true;
            break;
          }
          else if (count_excl == 0) {
            /* the list of adjacent species to v is a subset of the list of
             * adjacent species to cmv, meaning v can be ignored in the
             * next iterations on the characters in Cm
             */
            #ifdef DEBUG
            std::cout << " ignore, subset" << std::endl;
            #endif
            
            skip_cycle = true;
            break;
          }
          else {
            // how we ended up here nobody knows
            #ifdef DEBUG
            std::cout << " idk";
            #endif
          }
        }
        
        #ifdef DEBUG
        std::cout << std::endl;
        #endif
      }
      
      if (std::next(cmv) == cmv_end) {
        // last iteration on the characters in the list has been performed
        if (keep_char) {
          /* after all the iterations keep_char is true if the list of adjacent
           * species to v is neither a superset nor a subset of the lists of
           * adjacent species to the characters in Cm, so v is a maximal
           * characters and can be added to the list of maximal characters Cm
           */
          cm.push_front(v);
          #ifdef DEBUG
          std::cout << "\tadd" << std::endl;
          #endif
        }
      }
      
      #ifdef DEBUG
      std::cout << std::endl;
      #endif
    }
  }
  
  return cm;
}

void maximal_reducible_graph(const std::list<RBVertex>& cm, RBGraph& g) {
  // remove non-maximal characters
  RBVertexIter v, v_end, next;
  std::tie(v, v_end) = vertices(g);
  for (next = v; v != v_end; v = next) {
    ++next;
    
    if (g[*v].type == Type::character)
      remove_vertex_if(*v, if_not_maximal(cm), g);
  }
}

bool is_included(const std::list<std::string>& a,
                 const std::list<std::string>& b) {
  std::list<std::string>::const_iterator i = a.begin();
  for (; i != a.end(); ++i) {
    std::list<std::string>::const_iterator in;
    in = std::find(b.begin(), b.end(), *i);
    
    if (in == b.end())
      // exit the function at the first string of a not present in b
      return false;
  }
  
  return true;
}

void hasse_diagram(HDGraph& hasse, const RBGraph& g) {
  std::vector<std::list<RBVertex>> sets(num_species(g));
  std::map<RBVertex, std::list<RBVertex>> v_map;
  
  /* How sets is going to be structured:
   * sets[index] => < S, List of adjacent characters to S >
   *
   * sets is used to sort the lists by number of elements, this is why we store
   * the list of adjacent characters to S. While we store S to be able to
   * access v_map[S] in costant time
   *
   * How v_map is going to be structured:
   * v_map[S] => < List of adjacent characters to S >
   */
  
  size_t idx = 0;
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (g[*v].type != Type::species)
      continue;
    // for each species vertex
    
    #ifdef DEBUG
    std::cout << "C(" << g[*v].name << ") = { ";
    #endif
    
    sets[idx].push_back(*v);
    
    // build v's set of adjacent characters
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      RBVertex vt = target(*e, g);
      
      #ifdef DEBUG
      std::cout << g[vt].name << " ";
      #endif
      
      sets[idx].push_back(vt);
      v_map[*v].push_back(vt);
    }
    
    #ifdef DEBUG
    std::cout << "}" << std::endl;
    #endif
    
    ++idx;
  }
  
  #ifdef DEBUG
  std::cout << std::endl;
  #endif
  
  // sort sets by size in ascending order
  std::sort(sets.begin(), sets.end(), ascending_size);
  
  for (size_t i = 0; i < sets.size(); ++i) {
    // for each set of characters
    RBVertex v = sets[i].front();
    
    // fill the list of characters names of v
    std::list<std::string> lcv;
    RBVertexIter cv = v_map[v].begin(), cv_end = v_map[v].end();
    for (; cv != cv_end; ++cv)
      lcv.push_back(g[*cv].name);
    
    if (i == 0) {
      /* first iteration of the loop:
       * add v to the Hasse diagram, and being the first vertex of the graph
       * there's no need to do any work.
       */
      #ifdef DEBUG
      std::cout << "Hasse.addV " << g[v].name << std::endl << std::endl;
      #endif
      
      add_vertex(g[v].name, lcv, hasse);
      
      continue;
    }
    
    #ifdef DEBUG
    std::cout << "C(" << g[v].name << ") = { ";
    std::list<std::string>::const_iterator kk = lcv.begin();
    for (; kk != lcv.end(); ++kk) std::cout << *kk << " ";
    std::cout << "}:" << std::endl;
    #endif
    
    /* new_edges will contain the list of edges that may be added to the
     * Hasse diagram: HDVertex is the source, std::string is the edge label
     */
    std::list<std::pair<HDVertex, std::string>> new_edges;
    
    /* check if there is a vertex with the same characters as v or
     * if v needs to be added to the Hasse diagram
     */
    HDVertexIter hdv, hdv_end;
    std::tie(hdv, hdv_end) = vertices(hasse);
    for (; hdv != hdv_end; ++hdv) {
      // for each vertex in hasse
      #ifdef DEBUG
      std::cout << "hdv: ";
      kk = hasse[*hdv].species.begin();
      for (; kk != hasse[*hdv].species.end(); ++kk) std::cout << *kk << " ";
      std::cout << "= { ";
      kk = hasse[*hdv].characters.begin();
      for (; kk != hasse[*hdv].characters.end(); ++kk) std::cout << *kk << " ";
      std::cout << "} -> ";
      #endif
      
      if (lcv == hasse[*hdv].characters) {
        // v and hdv have the same characters
        #ifdef DEBUG
        std::cout << "mod" << std::endl;
        #endif
        
        // add v (species) to the list of vertices in hdv
        hasse[*hdv].species.push_back(g[v].name);
        
        break;
      }
      
      std::list<std::string> lhdv = hasse[*hdv].characters;
      
      // TODO: find the correct way to build the edges
      
      if (is_included(lhdv, lcv)) {
        // hdv is included in v
        std::list<std::string>::const_iterator ci, ci_end;
        ci = lcv.begin(); ci_end = lcv.end();
        for (; ci != ci_end; ++ci) {
          // for each character in hdv
          std::list<std::string>::const_iterator in;
          in = std::find(lhdv.begin(), lhdv.end(), *ci);
          
          if (in == lhdv.end()) {
            // character is not present in lhdv
            new_edges.push_back(std::make_pair(*hdv, *ci));
          }
        }
      }
      
      if (std::next(hdv) == hdv_end) {
        // last iteration on the characters in the list has been performed
        #ifdef DEBUG
        std::cout << "add" << std::endl;
        #endif
        
        // build a vertex for v and add it to the Hasse diagram
        HDVertex u = add_vertex(g[v].name, lcv, hasse);
        
        // build in_edges for the vertex and add them to the Hasse diagram
        std::list<std::pair<HDVertex, std::string>>::iterator ei, ei_end;
        ei = new_edges.begin(), ei_end = new_edges.end();
        for (; ei != ei_end; ++ei) {
          // for each new edge to add to the Hasse diagram
          HDEdge edge;
          std::tie(edge, std::ignore) = add_edge(ei->first, u, hasse);
          hasse[edge].lcs.push_back({ ei->second, State::gain });
          
          #ifdef DEBUG
          std::cout << "Hasse.addE ";
          kk = hasse[ei->first].species.begin();
          for (; kk != hasse[ei->first].species.end(); ++kk) std::cout << *kk << " ";
          std::cout << "-";
          std::list<CharacterState>::const_iterator jj = hasse[edge].lcs.begin();
          for (; jj != hasse[edge].lcs.end(); ++jj) {
            std::cout << *jj;
            if (std::next(jj) != hasse[edge].lcs.end())
              std::cout << ",";
          }
          std::cout << "-> ";
          kk = hasse[u].species.begin();
          for (; kk != hasse[u].species.end(); ++kk) std::cout << *kk << " ";
          std::cout << std::endl;
          #endif
        }
        
        break;
      }
      #ifdef DEBUG
      else {
        std::cout << "ignore";
      }
      
      std::cout << std::endl;
      #endif
    }
    
    #ifdef DEBUG
    std::cout << std::endl;
    #endif
  }
  
  #ifdef DEBUG
  std::cout << "Before transitive reduction:" << std::endl;
  print_hdgraph(hasse);
  std::cout << std::endl;
  #endif
  
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
        /* for each out-edge
         * source -> u -> target
         */
        HDEdge e;
        bool exists;
        std::tie(e, exists) = edge(source(*ie, hasse), target(*oe, hasse),
                                   hasse);
        
        if (!exists)
          // no transitivity between source and target
          continue;
        
        /* remove source -> target, which breaks the no-transitivty rule in
         * the Hasse diagram we need, because a path between source and target
         * already exists in the form: source -> u -> target
         */
        remove_edge(e, hasse);
      }
    }
  }
}

RBVertexIter find_vertex(RBVertexIter v, RBVertexIter v_end,
                         const std::string& name, const RBGraph& g) {
  for (; v != v_end; ++v) {
    if (g[*v].name == name)
      return v;
  }
  
  return v_end;
}

HDVertexIter find_source(HDVertexIter v, HDVertexIter v_end,
                         const HDGraph& hasse) {
  for (; v != v_end; ++v) {
    if (in_degree(*v, hasse) == 0)
      return v;
  }
  
  return v_end;
}

bool is_redsigma(const RBGraph& g) {
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (g[*v].type != Type::character)
      continue;
    
    if (is_inactive(*v, g) || is_free(*v, g))
      return false;
  }
  
  // Return True if g isn't empty (Empty graph isn't red sigma)
  return !is_empty(g);
}

bool safe_source(const HDVertex v, const RBGraph& g, const HDGraph& hasse) {
  RBGraph g1(g);
  
  std::list<std::string>::const_iterator i = hasse[v].species.begin();
  for (; i != hasse[v].species.end(); ++i) {
    RBVertexIter u, u_end, in;
    std::tie(u, u_end) = vertices(g1);
    in = find_vertex(u, u_end, *i, g1);
    // for each species in v
    
    if (in == u_end)
      return false;
    
    realize(*in, g1);
  }
  
  return !is_redsigma(g1);
}

std::pair<std::list<CharacterState>, bool> safe_chain(const RBGraph& g,
                                                      const RBGraph& g_cm,
                                                      const HDGraph& hasse) {
  /* TODO: implement safe_chain by using
   *   http://www.boost.org/doc/libs/1_62_0/libs/graph/doc/depth_first_search.html
   * with an implementation of
   *   http://www.boost.org/doc/libs/1_62_0/libs/graph/doc/DFSVisitor.html
   */
  
  std::list<CharacterState> output;
  bool safe = false;
  
  HDVertexIter v, v_end;
  std::tie(v, v_end) = vertices(hasse);
  for (; v != v_end; ++v) {
    v = find_source(v, v_end, hasse);
    // for each source in hasse
    
    if (v == v_end)
      // hasse has no sources left
      break;
    
    HDVertex curr = *v;
    std::list<CharacterState> lc;
    
    #ifdef DEBUG
    std::cout << "Source: [ ";
    std::list<std::string>::const_iterator kk = hasse[curr].species.begin();
    for (; kk != hasse[curr].species.end(); ++kk) std::cout << *kk << " ";
    std::cout << "]" << std::endl
              << "C: < ";
    #endif
    
    while (out_degree(curr, hasse) > 0) {
      #ifdef DEBUG
      std::cout << "[ ";
      kk = hasse[curr].species.begin();
      for (; kk != hasse[curr].species.end(); ++kk) std::cout << *kk << " ";
      std::cout << "] ";
      #endif
      
      HDOutEdgeIter edge;
      std::tie(edge, std::ignore) = out_edges(curr, hasse);
      lc.insert(lc.end(), hasse[*edge].lcs.begin(), hasse[*edge].lcs.end());
      
      curr = target(*edge, hasse);
    }
    
    #ifdef DEBUG
    std::cout << "[ ";
    kk = hasse[curr].species.begin();
    for (; kk != hasse[curr].species.end(); ++kk) std::cout << *kk << " ";
    std::cout << "] >" << std::endl 
              << "S(C): < ";
    std::list<CharacterState>::const_iterator jj = lc.begin();
    for (; jj != lc.end(); ++jj) std::cout << *jj << " ";
    std::cout << ">" << std::endl;
    #endif
    
    RBGraph g_cm_test(g_cm);
    std::tie(output, std::ignore) = realize(lc, g_cm_test);
    
    #ifdef DEBUG
    print_rbgraph(g_cm_test);
    #endif
    
    if (!is_redsigma(g_cm_test)) {
      #ifdef DEBUG
      std::cout << "No red Σ-graph" << std::endl;
      #endif
      
      // TODO: test safe source?
      
      safe = true;
      
      break;
    }
    else {
      #ifdef DEBUG
      std::cout << "Found red Σ-graph" << std::endl;
      #endif
      
      output.clear();
    }
    
    #ifdef DEBUG
    std::cout << std::endl;
    #endif
  }
  
  return std::make_pair(output, safe);
}


//=============================================================================
// Algorithm main functions

std::list<CharacterState> reduce(RBGraph& g) {
  std::list<CharacterState> output;
  
  #ifdef DEBUG
  std::cout << std::endl
            << "Working on G:" << std::endl;
  print_rbgraph(g);
  std::cout << std::endl;
  #endif
  
  // cleanup graph from dead vertices
  // TODO: check if this is needed (realize already removes singletons)
  remove_singletons(g);
  
  if (is_empty(g)) {
    /* if graph is empty
     * return the empty sequence
     */
    #ifdef DEBUG
    std::cout << "G empty"
              << std::endl << std::endl;
    #endif
    
    return output;
  }
  
  #ifdef DEBUG
  std::cout << "G not empty" << std::endl;
  #endif
  
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_free(*v, g)) {
      /* if v is free
       * realize v-
       * return < v-, reduce(g) >
       */
      #ifdef DEBUG
      std::cout << "G free character " << g[*v].name
                << std::endl << std::endl;
      #endif
      
      std::list<CharacterState> lc;
      std::tie(lc, std::ignore) = realize({ g[*v].name, State::lose }, g);
      
      output.splice(output.end(), lc);
      output.splice(output.end(), reduce(g));
      
      // return < v-, reduce(g) >
      return output;
    }
  }
  
  #ifdef DEBUG
  std::cout << "G no free characters" << std::endl;
  #endif
  
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_universal(*v, g)) {
      /* if v is universal
       * realize v+
       * return < v+, reduce(g) >
       */
      #ifdef DEBUG
      std::cout << "G universal character " << g[*v].name
                << std::endl << std::endl;
      #endif
      
      std::list<CharacterState> lc;
      std::tie(lc, std::ignore) = realize({ g[*v].name, State::gain }, g);
      
      output.splice(output.end(), lc);
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
    /* if graph is not connected
     * build subgraphs (connected components) g1, g2, etc.
     * return < reduce(g1), reduce(g2), ... >
     */
    for (size_t i = 0; i < num_comps; ++i)
      output.splice(output.end(), reduce(*components[i].get()));
    
    return output;
  }
  
  /* cm = Cm, maximal characters of g (Grb)
   * g_cm = Grb|Cm, maximal reducible graph of g (Grb)
   */
  RBGraph g_cm(g);
  std::list<RBVertex> cm = maximal_characters2(g_cm);
  maximal_reducible_graph(cm, g_cm);
  
  #ifdef DEBUG
  std::cout << "Cm = { ";
  std::list<RBVertex>::const_iterator kk = cm.begin();
  for (; kk != cm.end(); ++kk) std::cout << g_cm[*kk].name << " ";
  std::cout << "}" << std::endl
            << "Gcm:" << std::endl;
  print_rbgraph(g_cm);
  std::cout << std::endl;
  #endif
  
  // p = Hasse diagram for g_cm (Grb|Cm)
  HDGraph p;
  hasse_diagram(p, g_cm);
  
  #ifdef DEBUG
  std::cout << "Hasse:" << std::endl;
  print_hdgraph(p);
  std::cout << std::endl;
  #endif
  
  // sc = safe chain for g (Grb)
  std::list<CharacterState> sc;
  bool s_safe;
  std::tie(sc, s_safe) = safe_chain(g, g_cm, p);
  
  if (!s_safe)
    // p has no safe source
    throw std::runtime_error("Could not reduce graph");
  
  std::list<CharacterState> lc;
  std::tie(lc, std::ignore) = realize(sc, g);
  
  output.splice(output.end(), lc);
  output.splice(output.end(), reduce(g));
  
  // return < sc, reduce(g) >
  return output;
}

std::pair<std::list<CharacterState>, bool> realize(const CharacterState& c,
                                                   RBGraph& g) {
  std::list<CharacterState> output;
  
  // find vertex whose name is c.character in g
  RBVertexIter v, v_end, in;
  std::tie(v, v_end) = vertices(g);
  in = find_vertex(v, v_end, c.character, g);
  
  if (in == v_end)
    // vertex whose name is c.character doesn't exist in g
    return std::make_pair(output, false);
  
  RBVertex cv = *in;
  
  RBIndexMap map_index, map_comp;
  RBAssocMap i_map(map_index), c_map(map_comp);
  
  // fill vertex index map
  std::tie(v, v_end) = vertices(g);
  for (size_t idx = 0; v != v_end; ++v, ++idx) {
    boost::put(i_map, *v, idx);
  }
  
  // build the components map
  boost::connected_components(g, c_map, boost::vertex_index_map(i_map));
  
  if (c.state == State::gain && is_inactive(cv, g)) {
    // c+ and c is inactive
    #ifdef DEBUG
    std::cout << c << " (+ and inactive):" << std::endl;
    #endif
    
    std::tie(v, v_end) = vertices(g);
    for (; v != v_end; ++v) {
      if (g[*v].type == Type::character || map_comp[*v] != map_comp[cv])
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
        std::cout << "rm;\t";
        #endif
        
        remove_edge(e, g);
      }
      else {
        // there isn't an edge between *v and cv
        #ifdef DEBUG
        std::cout << "red;\t";
        #endif
        
        add_edge(*v, cv, Color::red, g);
      }
    }
    
    #ifdef DEBUG
    std::cout << std::endl;
    #endif
  }
  else if (c.state == State::lose && is_active(cv, g)) {
    // c- and c is active
    #ifdef DEBUG
    std::cout << c << " (- and active)" << std::endl;
    #endif
    
    clear_vertex(cv, g);
  }
  else {
    #ifdef DEBUG
    std::cout << "Could not realize " << c << std::endl;
    #endif
    
    return std::make_pair(output, false);
  }
  
  output.push_back(c);
  
  remove_singletons(g);
  
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_universal(*v, g)) {
      /* if v is universal
       * realize v+
       */
      #ifdef DEBUG
      std::cout << "G universal character " << g[*v].name
                << std::endl << std::endl;
      #endif
      
      std::list<CharacterState> lc;
      std::tie(lc, std::ignore) = realize({ g[*v].name, State::gain }, g);
      
      output.splice(output.end(), lc);
      
      return std::make_pair(output, true);
    }
  }
  
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    // for each vertex
    if (is_free(*v, g)) {
      /* if v is free
       * realize v-
       */
      #ifdef DEBUG
      std::cout << "G free character " << g[*v].name
                << std::endl << std::endl;
      #endif
      
      std::list<CharacterState> lc;
      std::tie(lc, std::ignore) = realize({ g[*v].name, State::lose }, g);
      
      output.splice(output.end(), lc);
      
      return std::make_pair(output, true);
    }
  }
  
  return std::make_pair(output, true);
}

std::pair<std::list<CharacterState>, bool> realize(const RBVertex& v,
                                                   RBGraph& g) {
  std::list<CharacterState> lc;
  
  if (g[v].type != Type::species)
    return std::make_pair(lc, false);
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    RBVertex u = target(*e, g);
    
    if (is_inactive(u, g))
      lc.push_back({ g[u].name, State::gain });
  }
  
  return realize(lc, g);
}

std::pair<std::list<CharacterState>, bool> realize(
    const std::list<CharacterState>& lc,
    RBGraph& g) {
  std::list<CharacterState> output;
  bool feasible = true;
  
  std::list<CharacterState>::const_iterator i = lc.begin();
  for (; i != lc.end(); ++i) {
    std::list<CharacterState> lcs;
    bool feasible_realize;
    std::tie(lcs, feasible_realize) = realize(*i, g);
    
    output.splice(output.end(), lcs);
    
    if (!feasible_realize)
      feasible = false;
  }
  
  return std::make_pair(output, feasible);
}
