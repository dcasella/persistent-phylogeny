#include "functions.hpp"


int main(int argc, const char* argv[]) {
  RBGraph g;
  RBVertex s1, s2, s3, s4, s5, s6,
           c1, c2, c3, c4, c5, c6, c7, c8;

  s1 = add_vertex("s1", Type::species, g);
  s2 = add_vertex("s2", Type::species, g);
  s3 = add_vertex("s3", Type::species, g);
  s4 = add_vertex("s4", Type::species, g);
  s5 = add_vertex("s5", Type::species, g);
  s6 = add_vertex("s6", Type::species, g);
  c1 = add_vertex("c1", Type::character, g);
  c2 = add_vertex("c2", Type::character, g);
  c3 = add_vertex("c3", Type::character, g);
  c4 = add_vertex("c4", Type::character, g);
  c5 = add_vertex("c5", Type::character, g);
  c6 = add_vertex("c6", Type::character, g);
  c7 = add_vertex("c7", Type::character, g);
  c8 = add_vertex("c8", Type::character, g);

  add_edge(s1, c8, g);
  add_edge(s2, c3, g);
  add_edge(s2, c5, g);
  add_edge(s2, c6, g);
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
  add_edge(s6, c2, g);
  add_edge(s6, c3, g);
  add_edge(s6, c5, g);

  RBGraph g1(g);
  build_bimap(g1);

  realize({ "c3", State::gain }, g);
  realize({ "c5", State::gain }, g);
  realize({ "c2", State::gain }, g);
  realize({ "c4", State::lose }, g);

  realize({ { "c3", State::gain }, { "c5", State::gain },
            { "c2", State::gain }, { "c4", State::lose } }, g1);

  assert(num_species(g) == num_species(g1));
  assert(num_characters(g) == num_characters(g1));
  assert(num_edges(g) == num_edges(g1));

  std::cout << "realize: tests passed" << std::endl;

  return 0;
}
