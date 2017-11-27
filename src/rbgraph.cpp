#include "rbgraph.hpp"
#include <fstream>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>


//=============================================================================
// Boost functions (overloading)

void remove_vertex(const RBVertex v, RBGraph& g) {
  if (is_species(v, g))
    num_species(g)--;
  else
    num_characters(g)--;
  
  boost::remove_vertex(v, g);
}

RBVertex add_vertex(const std::string& name, const Type type, RBGraph& g) {
  RBVertex v = boost::add_vertex(g);
  g[v].name = name;
  g[v].type = type;
  
  if (is_species(v, g))
    num_species(g)++;
  else
    num_characters(g)++;
  
  return v;
}

std::pair<RBEdge, bool>
add_edge(const RBVertex u, const RBVertex v, const Color color, RBGraph& g) {
  RBEdge e;
  bool exists;
  std::tie(e, exists) = boost::add_edge(u, v, g);
  g[e].color = color;
  
  return std::make_pair(e, exists);
}


//=============================================================================
// General functions

std::ostream& operator<<(std::ostream& os, const RBGraph& g) {
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    os << g[*v].name << ":";
    
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      os << " -" << (is_red(*e, g) ? "r" : "-") << "- "
         << g[target(*e, g)].name << ";";
    }
    
    if (std::next(v) != v_end)
      os << std::endl;
  }
  
  return os;
}

// File I/O

void read_graph(const std::string& filename, RBGraph& g) {
  std::vector<RBVertex> species, characters;
  bool first_line = true;
  std::string line;
  std::ifstream file(filename);
  
  size_t index = 0;
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
        std::string v_name = "s" + std::to_string(j + 1);
        
        species[j] = add_vertex(v_name, Type::species, g);
      }
      
      // insert characters in the graph
      for (size_t j = 0; j < characters.size(); ++j) {
        std::string v_name = "c" + std::to_string(j + 1);
        
        characters[j] = add_vertex(v_name, Type::character, g);
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
          
          case '1':
            // add edge between species[s_index] and characters[c_index]
            {
              size_t s_index = index / characters.size(),
                     c_index = index % characters.size();
              
              if (s_index >= species.size() || c_index >= characters.size()) {
                // input file parsing error
                throw std::runtime_error(
                  "Failed to read graph from file: oversized matrix"
                );
              }
              
              RBEdge edge;
              std::tie(edge, std::ignore) = add_edge(
                species[s_index], characters[c_index], g
              );
              
              if (red_edge)
                g[edge].color = Color::red;
            }
            break;
          
          case '0':
            // ignore
            break;
          
          default:
            // input file parsing error
            throw std::runtime_error(
              "Failed to read graph from file: unexpected value in matrix"
            );
        }
        
        index++;
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


//=============================================================================
// Algorithm functions

bool is_active(const RBVertex v, const RBGraph& g) {
  if (!is_character(v, g))
    return false;
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (!is_red(*e, g) || !is_species(target(*e, g), g))
      return false;
  }
  
  return true;
}

bool is_inactive(const RBVertex v, const RBGraph& g) {
  if (!is_character(v, g))
    return false;
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (!is_black(*e, g) || !is_species(target(*e, g), g))
      return false;
  }
  
  return true;
}

void remove_singletons(RBGraph& g) {
  RBVertexIter v, v_end, next;
  std::tie(v, v_end) = vertices(g);
  for (next = v; v != v_end; v = next) {
    next++;
    remove_vertex_if(*v, if_singleton(), g);
  }
}

bool is_free(const RBVertex v, const RBGraph& g) {
  if (!is_character(v, g))
    return false;
  
  size_t count_species = 0;
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (!is_red(*e, g) || !is_species(target(*e, g), g))
      return false;
    
    count_species++;
  }
  
  if (count_species != num_species(g))
    return false;
  
  return true;
}

bool is_universal(const RBVertex v, const RBGraph& g) {
  if (!is_character(v, g))
    return false;
  
  size_t count_species = 0;
  
  RBOutEdgeIter e, e_end;
  std::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (!is_black(*e, g) || !is_species(target(*e, g), g))
      return false;
    
    count_species++;
  }
  
  if (count_species != num_species(g))
    return false;
  
  return true;
}

size_t connected_components(RBGraphVector& components, const RBGraph& g) {
  size_t num_comps;
  RBVertexIMap map_index, map_comp;
  RBVertexIAssocMap i_map(map_index), c_map(map_comp);
  
  // fill the vertex index map i_map
  RBVertexIter v, v_end;
  size_t index = 0;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v, ++index) {
    boost::put(i_map, *v, index);
  }
  
  // get number of components and the components map
  num_comps = boost::connected_components(
    g, c_map, boost::vertex_index_map(i_map)
  );
  
  // how map_comp is structured (after running boost::connected_components):
  // map_comp[i] => < vertex_in_g, component_index >
  
  components.clear();
  
  if (num_comps > 1) {
    // graph is disconnected
    std::map<RBVertex, RBVertex> vertices;
    
    // how vertices is going to be structured:
    // vertices[vertex_in_g] => vertex_in_component
    
    // resize subgraph components
    components.resize(num_comps);
    
    // initialize subgraph components
    for (size_t i = 0; i < num_comps; ++i) {
      components[i] = std::make_unique<RBGraph>();
    }
    
    // add vertices to their respective subgraph
    RBVertexIMap::iterator i = map_comp.begin();
    for (; i != map_comp.end(); ++i) {
      // for each vertex
      RBVertex v = i->first;
      RBVertexSize comp = i->second;
      RBGraph* component = components[comp].get();
      
      // add the vertex to *component and copy its descriptor in vertices[v]
      vertices[v] = add_vertex(g[v].name, g[v].type, *component);
    }
    
    // add edges to their respective vertices and subgraph
    i = map_comp.begin();
    for (; i != map_comp.end(); ++i) {
      // for each vertex
      RBVertex v = i->first;
      
      // prevent duplicate edges
      if (!is_species(v, g))
        continue;
      
      RBVertex new_v = vertices[v];
      RBVertexSize comp = i->second;
      RBGraph* component = components[comp].get();
      
      RBOutEdgeIter e, e_end;
      std::tie(e, e_end) = out_edges(v, g);
      for (; e != e_end; ++e) {
        // for each out edge
        RBVertex new_vt = vertices[target(*e, g)];
        
        RBEdge edge;
        std::tie(edge, std::ignore) = add_edge(
          new_v, new_vt, g[*e].color, *component
        );
      }
    }
    
    #ifdef DEBUG
    std::cout << "Connected components:" << std::endl;
    
    for (size_t i = 0; i < num_comps; ++i) {
      std::cout << *components[i].get() << std::endl << std::endl;
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
  
  // how sets is going to be structured:
  // sets[C] => < List of adjacent species to C >
  
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (!is_character(*v, g))
      continue;
    // for each character vertex
    
    // build v's set of adjacent species
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      RBVertex vt = target(*e, g);
      
      // if v is active or connected to random nodes ignore it
      if (is_red(*e, g) || !is_species(vt, g))
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
      
      RBVertexIter kk = sets[*cmv].begin();
      for (; kk != sets[*cmv].end(); ++kk) {
        std::cout << g[*kk].name << " ";
      }
      
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
        RBVertexIter in = std::find(sets[*cmv].begin(), sets[*cmv].end(), *sv);
        
        // keep count of how many species are included (or not found) in
        // the list of cmv's adjacent species
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
            // the list of adjacent species to v is a superset of the list of
            // adjacent species to cmv, which means cmv can be replaced
            // by v in the list of maximal characters Cm
            #ifdef DEBUG
            std::cout << " subst" << std::endl;
            #endif
            
            cm.push_front(*v);
            cm.remove(*(cmv++));
            skip_cycle = true;
            break;
          }
          else if (count_incl < sets[*cmv].size() && count_excl > 0) {
            // the list of adjacent species to v is neither a superset nor a
            // subset of the list of adjacent species to cmv, which means
            // v may be a new maximal character
            #ifdef DEBUG
            std::cout << " add, not subset";
            #endif
            
            keep_char = true;
          }
          else if (count_incl == sets[*cmv].size()) {
            // the list of adjacent species to v is the same as the list of
            // adjacent species to cmv, so v can be ignored in the next
            // iterations on the characters in Cm
            #ifdef DEBUG
            std::cout << " ignore, same set" << std::endl;
            #endif
            
            skip_cycle = true;
            break;
          }
          else if (count_excl == 0) {
            // the list of adjacent species to v is a subset of the list of
            // adjacent species to cmv, meaning v can be ignored in the
            // next iterations on the characters in Cm
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
          // after all the iterations keep_char is true if the list of adjacent
          // species to v is neither a superset nor a subset of the lists of
          // adjacent species to the characters in Cm, so v is a maximal
          // characters and can be added to the list of maximal characters Cm
          #ifdef DEBUG
          std::cout << "\tadd" << std::endl;
          #endif
          
          cm.push_front(*v);
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
  
  // how sets is going to be structured:
  // sets[index] => < C, List of adjacent species to C >
  
  // sets is used to sort the lists by number of elements, this is why we store
  // the list of adjacent species to C. While we store C to be able to access
  // v_map[C] in costant time
  
  // how v_map is going to be structured:
  // v_map[C] => < List of adjacent species to C >
  
  size_t index = 0;
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (!is_character(*v, g))
      continue;
    // for each character vertex
    
    sets[index].push_back(*v);
    
    // build v's set of adjacent species
    RBOutEdgeIter e, e_end;
    std::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      RBVertex vt = target(*e, g);
      
      // if v is active or connected to random nodes ignore it
      if (is_red(*e, g) || !is_species(vt, g))
        break;
      
      sets[index].push_back(vt);
      v_map[*v].push_back(vt);
    }
    
    index++;
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
      
      RBVertexIter kk = v_map[*cmv].begin();
      for (; kk != v_map[*cmv].end(); ++kk) {
        std::cout << g[*kk].name << " ";
      }
      
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
        RBVertexIter in = std::find(
          v_map[*cmv].begin(), v_map[*cmv].end(), *sv
        );
        
        // keep count of how many species are included (or not found) in
        // the list of cmv's adjacent species
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
            // the list of adjacent species to v is neither a superset nor a
            // subset of the list of adjacent species to cmv, which means
            // v may be a new maximal character
            #ifdef DEBUG
            std::cout << " add, not subset";
            #endif
            
            keep_char = true;
          }
          else if (count_incl == v_map[*cmv].size()) {
            // the list of adjacent species to v is the same as the list of
            // adjacent species to cmv, so v can be ignored in the next
            // iterations on the characters in Cm
            #ifdef DEBUG
            std::cout << " ignore, same set" << std::endl;
            #endif
            
            skip_cycle = true;
            break;
          }
          else if (count_excl == 0) {
            // the list of adjacent species to v is a subset of the list of
            // adjacent species to cmv, meaning v can be ignored in the
            // next iterations on the characters in Cm
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
          // after all the iterations keep_char is true if the list of adjacent
          // species to v is neither a superset nor a subset of the lists of
          // adjacent species to the characters in Cm, so v is a maximal
          // characters and can be added to the list of maximal characters Cm
          #ifdef DEBUG
          std::cout << "\tadd" << std::endl;
          #endif
          
          cm.push_front(v);
        }
      }
      
      #ifdef DEBUG
      std::cout << std::endl;
      #endif
    }
  }
  
  return cm;
}

RBGraph maximal_reducible_graph(const RBGraph& g) {
  // cm has to be a list of vertices of g for this implementation to work
  RBGraph gm;
  std::list<RBVertex> cm;
  RBVertexIMap map_index;
  RBVertexIAssocMap i_map(map_index);
  
  // fill the vertex index map i_map
  RBVertexIter v, v_end;
  size_t index = 0;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v, ++index) {
    boost::put(i_map, *v, index);
  }
  
  // copy g to gm, fill the vertex map v_map
  copy_graph(g, gm, boost::vertex_index_map(i_map));
  
  num_species(gm) = num_species(g);
  num_characters(gm) = num_characters(g);
  
  cm = maximal_characters2(gm);
  
  #ifdef DEBUG
  std::cout << "Cm = { ";
  std::list<RBVertex>::iterator i = cm.begin();
  for (; i != cm.end(); ++i) std::cout << gm[*i].name << " ";
  std::cout << "}" << std::endl;
  #endif
  
  RBVertexIter next;
  std::tie(v, v_end) = vertices(gm);
  for (next = v; v != v_end; v = next) {
    next++;
    
    if (!is_character(*v, gm))
      // skip non-character vertices
      continue;
    
    remove_vertex_if(*v, if_not_maximal(cm), gm);
  }
  
  // TODO: add removesingletons(gm);?
  
  return gm;
}

RBVertexIter
find_vertex(RBVertexIter v, RBVertexIter v_end,
            const std::string& name, const RBGraph& g) {
  for (; v != v_end; ++v) {
    if (g[*v].name == name)
      return v;
  }
  
  return v_end;
}

bool is_redsigma(const RBGraph& g) {
  RBVertexIter v, v_end;
  std::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (!is_character(*v, g))
      continue;
    
    if (is_inactive(*v, g) || is_free(*v, g))
      return false;
  }
  
  // Return True if g isn't empty (Empty graph isn't red sigma)
  return !is_empty(g);
}
