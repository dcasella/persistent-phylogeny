#include "functions.hpp"
#include <fstream>


//=============================================================================
// General functions

// Red-Black Graph

void print_graph(const RBGraph& g) {
  VertexIter v, v_end;
  boost::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    std::cout << g[*v].name << ":";
    
    OutEdgeIter e, e_end;
    boost::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      std::cout << " -" << (g[*e].color == Color::red ? "r" : "-") << "- "
                << g[target(*e, g)].name
                << ";";
    }
    
    std::cout << std::endl;
  }
}

// File I/O

void read_graph(const std::string filename, RBGraph& g) {
  std::vector<Vertex> species, characters;
  bool value, first_line = true;
  std::string line;
  std::ifstream file(filename);
  
  size_t idx = 0;
  while (std::getline(file, line)) {
    // for each line in file
    std::istringstream iss(line);
    
    if (first_line) {
      // read rows and columns (species and characters)
      iss >> g[boost::graph_bundle].num_species;
      iss >> g[boost::graph_bundle].num_characters;
      
      species.resize(g[boost::graph_bundle].num_species);
      characters.resize(g[boost::graph_bundle].num_characters);
      
      // insert species in the graph
      for (size_t j = 0; j < g[boost::graph_bundle].num_species; ++j) {
        Vertex s = add_vertex(g);
        g[s].name = ("s" + std::to_string(j + 1));
        
        species[j] = s;
      }
      
      // insert characters in the graph
      for (size_t j = 0; j < g[boost::graph_bundle].num_characters; ++j) {
        Vertex c = add_vertex(g);
        g[c].name = ("c" + std::to_string(j + 1));
        g[c].type = Type::character;
        
        characters[j] = c;
      }
      
      first_line = false;
    }
    else {
      // read binary matrix
      while (iss >> value) {
        if (value) {
          // add (black) edge between species[s_idx] and characters[c_idx]
          size_t s_idx = idx / g[boost::graph_bundle].num_characters,
                 c_idx = idx % g[boost::graph_bundle].num_characters;
          
          add_edge(species[s_idx], characters[c_idx], g);
        }
        
        ++idx;
      }
    }
  }
}

// std::tuple<size_t, size_t>
// read_matrix(const std::string filename, std::vector<bool>& m) {
//   size_t num_species, num_characters;
//   bool value, first_line = true;
//   std::string line;
//   std::ifstream file(filename);
  
//   size_t i = 0;
//   while (std::getline(file, line)) {
//     std::istringstream iss(line);
    
//     if (first_line) {
//       // read rows and columns (species and characters)
//       iss >> num_species;
//       iss >> num_characters;
      
//       m.resize(num_species * num_characters);
      
//       first_line = false;
//     }
//     else {
//       // read binary matrix
//       while (iss >> value) {
//         m[i] = value;
//         ++i;
//       }
//     }
//   }
  
//   return std::make_tuple(num_species, num_characters);
// }


//=============================================================================
// Algorithm functions

// Red-Black Graph

bool if_singleton::operator()(const Vertex v, RBGraph& g) const {
  return (out_degree(v, g) == 0);
}

void remove_singletons(RBGraph& g) {
  VertexIter v, v_end, next;
  boost::tie(v, v_end) = vertices(g);
  for (next = v; v != v_end; v = next) {
    ++next;
    remove_vertex_if(*v, if_singleton(), g);
  }
}

bool is_free(const Vertex v, const RBGraph& g) {
  if (g[v].type != Type::character)
    return false;
  
  size_t count_species = 0;
  
  OutEdgeIter e, e_end;
  boost::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (g[*e].color == Color::black || g[target(*e, g)].type != Type::species)
      return false;
    
    count_species++;
  }
  
  if (count_species != g[boost::graph_bundle].num_species)
    return false;
  
  return true;
}

bool is_universal(const Vertex v, const RBGraph& g) {
  if (g[v].type != Type::character)
    return false;
  
  size_t count_species = 0;
  
  OutEdgeIter e, e_end;
  boost::tie(e, e_end) = out_edges(v, g);
  for (; e != e_end; ++e) {
    if (g[*e].color == Color::red || g[target(*e, g)].type != Type::species)
      return false;
    
    count_species++;
  }
  
  if (count_species != g[boost::graph_bundle].num_species)
    return false;
  
  return true;
}

size_t connected_components(const RBGraph& g, RBGraphVector& components) {
  size_t num_comps;
  IndexMap map_index, map_comp;
  AssocMap i_map(map_index), c_map(map_comp);
  
  // fill vertex index map
  VertexIter v, v_end;
  size_t idx = 0;
  boost::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v, ++idx) {
    boost::put(i_map, *v, idx);
  }
  
  // get number of components and the components map
  num_comps = boost::connected_components(
    g, c_map, boost::vertex_index_map(i_map)
  );
  
  if (num_comps > 1) {
    // graph is disconnected
    // resize subgraph components
    components.resize(num_comps);
    // vertices: graphVertex => compVertex
    std::map<Vertex, Vertex> vertices;
    
    // initialize subgraph components
    for (size_t i = 0; i < num_comps; ++i) {
      components[i] = std::unique_ptr<RBGraph>(new RBGraph);
    }
    
    // add vertices to their respective subgraph
    IndexMap::iterator i = map_comp.begin();
    for (; i != map_comp.end(); ++i) {
      // for each vertex
      Vertex v = i->first;
      VertexSize comp = i->second;
      RBGraph* component = components[comp].get();
      
      vertices[v] = add_vertex(*component);
      (*component)[vertices[v]].name = g[v].name;
      (*component)[vertices[v]].type = g[v].type;
      
      if (g[v].type == Type::species)
        (*component)[boost::graph_bundle].num_species++;
      else
        (*component)[boost::graph_bundle].num_characters++;
    }
    
    // add edges to their respective vertices and subgraph
    i = map_comp.begin();
    for (; i != map_comp.end(); ++i) {
      // for each vertex
      Vertex v = i->first;
      Vertex new_v = vertices[v];
      VertexSize comp = i->second;
      RBGraph* component = components[comp].get();
      
      // prevent duplicate edges
      // if (g[v].type != Type::species)
      //   continue;
      
      OutEdgeIter e, e_end;
      boost::tie(e, e_end) = out_edges(v, g);
      for (; e != e_end; ++e) {
        // for each out edge
        Vertex new_vt = vertices[target(*e, g)];
        
        Edge edge;
        boost::tie(edge, std::ignore) = add_edge(new_v, new_vt, *component);
        (*component)[edge].color = g[*e].color;
      }
    }
    
    #ifdef DEBUG
    std::cout << "Connected components:" << std::endl;
    for (size_t i = 0; i < num_comps; ++i) {
      print_graph(*components[i].get());
      std::cout << std::endl;
    }
    #endif
  }
  #ifdef DEBUG
  else std::cout << "G connected" << std::endl;
  #endif
  
  return num_comps;
}

std::list<Vertex> maximal_characters(const RBGraph& g) {
  std::list<Vertex> cm;
  std::map< Vertex, std::list<Vertex> > sets;
  int count_incl, count_excl;
  bool keep_char, skip_cycle;
  
  VertexIter v, v_end;
  boost::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (g[*v].type != Type::character)
      continue;
    // for each character vertex
    
    // build v's set of adjacent species
    OutEdgeIter e, e_end;
    boost::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      Vertex vt = target(*e, g);
      
      if (g[*e].color == Color::red || g[vt].type != Type::species) {
        // v is active or connected to random nodes
        break;
      }
      
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
    VertexIter cmv = cm.begin(), cmv_end = cm.end();
    while (cmv != cmv_end) {
      // for each species in cm
      if (skip_cycle)
        break;
      
      #ifdef DEBUG
      std::cout << "curr Cm: " << g[*cmv].name << " = { ";
      VertexIter kk = sets[*cmv].begin(), kk_end = sets[*cmv].end();
      for (; kk != kk_end; ++kk) std::cout << g[*kk].name << " ";
      std::cout << "}:" << std::endl;
      #endif
      
      count_incl = 0; count_excl = 0;
      keep_char = false;
      
      VertexIter sv = sets[*v].begin(), sv_end = sets[*v].end();
      while (sv != sv_end) {
        // for each species adjacent to v, S(C#)
        #ifdef DEBUG
        std::cout << "S(" << g[*v].name << "): " << g[*sv].name << " -> ";
        #endif
        
        // find sv in the list of cmv's adjacent species
        VertexIter in = std::find(sets[*cmv].begin(), sets[*cmv].end(),
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
            std::cout << " add? not subset?";
            #endif
            
            keep_char = true;
          }
          else if (count_incl == sets[*cmv].size()) {
            /* the list of adjacent species to v is the same as the list of
             * adjacent species to cmv, so v can be ignored in the next
             * iterations on the characters in Cm
             */
            #ifdef DEBUG
            std::cout << " ignore, same set?" << std::endl;
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
            std::cout << " ignore, subset?" << std::endl;
            #endif
            
            skip_cycle = true;
            break;
          }
          else {
            // how we ended up here nobody knows
            #ifdef DEBUG
            std::cout << " idk?";
            #endif
          }
        }
        
        #ifdef DEBUG
        std::cout << std::endl;
        #endif
        
        ++sv;
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
      
      ++cmv;
    }
  }
  
  return cm;
}

// TODO: test which one is faster

std::list<Vertex> maximal_characters2(const RBGraph& g) {
  std::list<Vertex> cm;
  std::vector< std::list<Vertex> > sets(g[boost::graph_bundle].num_characters);
  std::map< Vertex, std::list<Vertex> > v_map;
  int count_incl, count_excl;
  bool keep_char, skip_cycle;
  
  size_t idx = 0;
  VertexIter v, v_end;
  boost::tie(v, v_end) = vertices(g);
  for (; v != v_end; ++v) {
    if (g[*v].type != Type::character)
      continue;
    // for each character vertex
    
    sets[idx].push_back(*v);
    
    // build v's set of adjacent species
    OutEdgeIter e, e_end;
    boost::tie(e, e_end) = out_edges(*v, g);
    for (; e != e_end; ++e) {
      Vertex vt = target(*e, g);
      if (g[*e].color == Color::red || g[vt].type != Type::species) {
        // v is active or connected to random nodes
        break;
      }
      
      sets[idx].push_back(vt);
      v_map[*v].push_back(vt);
    }
    
    ++idx;
  }
  
  // sort sets by size in descending order
  std::sort(sets.begin(), sets.end(), descending_size);
  
  cm.push_back(sets[0].front());
  
  for (size_t i = 1; i < sets.size(); ++i) {
    // for each set of species
    Vertex v = sets[i].front();
    
    if (sets[i].size() == sets[0].size()) {
      // both sets[i] and sets[0] are maximal
      cm.push_back(v);
      continue;
    }
    
    #ifdef DEBUG
    std::cout << g[v].name << std::endl;
    #endif
    
    skip_cycle = false;
    
    // check if sc is subset of the species adjacent to cmv
    VertexIter cmv = cm.begin(), cmv_end = cm.end();
    while (cmv != cmv_end) {
      // for each species in cm
      if (skip_cycle)
        break;
      
      #ifdef DEBUG
      std::cout << "curr Cm: " << g[*cmv].name << " = { ";
      VertexIter kk = v_map[*cmv].begin(), kk_end = v_map[*cmv].end();
      for (; kk != kk_end; ++kk) std::cout << g[*kk].name << " ";
      std::cout << "}:" << std::endl;
      #endif
      
      count_incl = 0; count_excl = 0;
      keep_char = false;
      
      VertexIter sv = v_map[v].begin(), sv_end = v_map[v].end();
      while (sv != sv_end) {
        // for each species adjacent to v, S(C#)
        #ifdef DEBUG
        std::cout << "S(" << g[v].name << "): " << g[*sv].name << " -> ";
        #endif
        
        // find sv in the list of cmv's adjacent species
        VertexIter in = std::find(v_map[*cmv].begin(), v_map[*cmv].end(),
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
            std::cout << " add? not subset?";
            #endif
            
            keep_char = true;
          }
          else if (count_incl == v_map[*cmv].size()) {
            /* the list of adjacent species to v is the same as the list of
             * adjacent species to cmv, so v can be ignored in the next
             * iterations on the characters in Cm
             */
            #ifdef DEBUG
            std::cout << " ignore, same set?" << std::endl;
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
            std::cout << " ignore, subset?" << std::endl;
            #endif
            
            skip_cycle = true;
            break;
          }
          else {
            // how we ended up here nobody knows
            #ifdef DEBUG
            std::cout << " idk?";
            #endif
          }
        }
        
        #ifdef DEBUG
        std::cout << std::endl;
        #endif
        
        ++sv;
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
      
      ++cmv;
    }
  }
  
  return cm;
}

bool if_not_maximal::operator()(const Vertex v, RBGraph& g) const {
  return (
    std::find(cm.begin(), cm.end(), v) == cm.end()
  );
}

void maximal_reducible_graph(RBGraph& g, std::list<Vertex> cm) {
  // remove non-maximal characters
  VertexIter v, v_end, next;
  boost::tie(v, v_end) = vertices(g);
  for (next = v; v != v_end; v = next) {
    ++next;
    
    if (g[*v].type == Type::character) {
      remove_vertex_if(*v, if_not_maximal(cm), g);
    }
  }
}


//=============================================================================
// Algorithm main functions

std::list<std::string> reduce(RBGraph& g) {
  std::list<std::string> output;
  
  // cleanup graph from dead vertices
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
  
  VertexIter v, v_end;
  boost::tie(v, v_end) = vertices(g);
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
      
      // realize(v-, g)
      // ...
      
      output.push_back(g[*v].name + "-");
      output.splice(output.end(), reduce(g));
      
      // return < v-, reduce(g) >
      return output;
    }
  }
  
  #ifdef DEBUG
  std::cout << "G no free characters" << std::endl;
  #endif
  
  boost::tie(v, v_end) = vertices(g);
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
      
      // realize(v+, g)
      // ...
      
      output.push_back(g[*v].name + "+");
      output.splice(output.end(), reduce(g));
      
      // return < v+, reduce(g) >
      return output;
    }
  }
  
  #ifdef DEBUG
  std::cout << "G no universal characters" << std::endl;
  #endif
  
  RBGraphVector components;
  size_t num_comps = connected_components(g, components);
  if (num_comps > 1) {
    /* if graph is not connected
     * build subgraphs (connected components) g1, g2, etc.
     * return < reduce(g1), reduce(g2), ... >
     */
    
    for (size_t i = 0; i < num_comps; ++i)
      output.splice(output.end(), reduce(*components[i].get()));
    
    // return < reduce(g1), reduce(g2), ... >
    return output;
  }
  
  RBGraph g_cm(g);
  std::list<Vertex> cm = maximal_characters2(g_cm);
  maximal_reducible_graph(g_cm, cm);
  
  #ifdef DEBUG
  std::cout << "Cm = { ";
  std::list<Vertex>::iterator i = cm.begin();
  for (; i != cm.end(); ++i) std::cout << g[*i].name << " ";
  std::cout << "}" << std::endl
            << "Gcm:" << std::endl;
  print_graph(g_cm);
  #endif
  
  // ...
  
  return output;
}
