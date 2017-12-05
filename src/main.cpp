#include "functions.hpp"
#include <boost/program_options.hpp>


int main(int argc, const char* argv[]) {
  std::vector<std::string> files;

  boost::program_options::options_description general_options(
    "Usage: ppp [OPTION...] FILE..."
    "\n"
    "Calculate a successful c-reduction for the FILE(s), if it exists."
    "\n\n"
    "Options"
  );
  general_options.add_options()(
    "help,h",
    "Display this message."
  )(
    "verbose,v",
    boost::program_options::bool_switch(&logging::enabled),
    "Display the operations performed by the algorithm as the program is "
    "running."
  )(
    "full",
    boost::program_options::bool_switch(&runtime::full),
    "Don't stop the iteration when a safe source is found."
  );

  boost::program_options::options_description hidden_options;
  hidden_options.add_options()(
    "files",
    boost::program_options::value<std::vector<std::string>>(&files)
  );

  boost::program_options::positional_options_description positional_options;
  positional_options.add("files", -1);

  boost::program_options::options_description cmdline_options;
  cmdline_options.add(general_options).add(hidden_options);

  boost::program_options::variables_map vm;

  try {
    boost::program_options::store(
      boost::program_options::command_line_parser(argc, argv)
        .positional(positional_options)
        .options(cmdline_options)
        .run(),
      vm
    );
    boost::program_options::notify(vm);
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "." << std::endl
              << "Try '" << argv[0] << " --help' for more information."
              << std::endl;

    return 1;
  }

  if (vm.count("help")) {
    std::cerr << general_options << std::endl;

    return 1;
  }

  if (!vm.count("files")) {
    std::cerr << "Error: No input file specified." << std::endl
              << "Try '" << argv[0] << " --help' for more information."
              << std::endl;

    return 1;
  }

  std::vector<std::string>::const_iterator file = files.begin();
  for (; file < files.end(); ++file) {
    RBGraph g;

    try {
      read_graph(*file, g);
    }
    catch (const std::exception& e) {
      std::cerr << "No (" << *file << ") " << e.what() << std::endl;

      continue;
    }

    try {
      std::list<SignedCharacter> output = reduce(g);

      std::cout << "Ok (" << *file << ") < ";

      SignedCharacterIter sci = output.begin();
      for (; sci != output.end(); ++sci) {
        std::cout << *sci << " ";
      }

      std::cout << ">" << std::endl;
    }
    catch (const NoReduction& e) {
      std::cout << "No (" << *file << ") " << e.what() << std::endl;
    }
  }

  return 0;
}
