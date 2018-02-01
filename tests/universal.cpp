#include "rbgraph.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  RBVertex s3, s4, s5,
           c1, c2, c3, c4, c5, c7;

  s3 = add_vertex("s3", Type::species, g);
  s4 = add_vertex("s4", Type::species, g);
  s5 = add_vertex("s5", Type::species, g);
  c1 = add_vertex("c1", Type::character, g);
  c2 = add_vertex("c2", Type::character, g);
  c3 = add_vertex("c3", Type::character, g);
  c4 = add_vertex("c4", Type::character, g);
  c5 = add_vertex("c5", Type::character, g);
       add_vertex("c6", Type::character, g);
  c7 = add_vertex("c7", Type::character, g);
       add_vertex("c8", Type::character, g);

  add_edge(s3, c2, g);
  add_edge(s3, c3, g);
  add_edge(s3, c4, Color::red, g);
  add_edge(s4, c1, g);
  add_edge(s4, c2, g);
  add_edge(s4, c4, Color::red, g);
  add_edge(s5, c1, g);
  add_edge(s5, c2, g);
  add_edge(s5, c3, g);
  add_edge(s5, c4, Color::red, g);
  add_edge(s5, c5, g);
  add_edge(s5, c7, g);

  assert(is_universal(s3, g) == false);
  assert(is_universal(c5, g) == false);
  assert(is_universal(c4, g) == false);
  assert(is_universal(c2, g) == true);

  std::cout << "universal: tests passed" << std::endl;

  return 0;
}
