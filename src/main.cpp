#include "functions.hpp"
#include <boost/program_options.hpp>
#include <boost/python.hpp>


int main(int argc, const char* argv[]) {
  // declare the vector of input files
  std::vector<std::string> files;

  // initialize options menu
  boost::program_options::options_description general_options(
    "Usage: ppp [OPTION...] FILE..."
    "\n"
    "Calculate a successful c-reduction for the matrix(ces) in FILE(s), if it exists."
    "\n\n"
    "Options"
  );
  // option: help message
  general_options.add_options()(
    "help,h",
    "Display this message."
  )
  // option: verbose mode, which prints information on the ongoing operations
  (
    "verbose,v",
    boost::program_options::bool_switch(&logging::enabled),
    "Display the operations performed by the program."
  )
  // option: full search, find every possible safe source during execution
  (
    "full",
    boost::program_options::bool_switch(&runtime::full),
    "Find all safe sources each iteration.\n"
    "(Enables --verbose)"
  )
  // option: interactive, let the user select which path to take
  (
    "interactive,i",
    boost::program_options::bool_switch(&runtime::interactive),
    "User input driven execution.\n"
    "(Enables --full and --verbose)"
  );

  // initialize hidden options (not shown in --help)
  boost::program_options::options_description hidden_options;
  // option: input files
  hidden_options.add_options()(
    "files",
    boost::program_options::value<std::vector<std::string>>(&files)
  );

  // initialize positional options
  boost::program_options::positional_options_description positional_options;
  // add input files to the positional options
  positional_options.add("files", -1);

  // initialize options
  boost::program_options::options_description cmdline_options;
  // add the options menu and the hidden options to the
  // available options
  cmdline_options.add(general_options).add(hidden_options);

  // initialize the variables map
  boost::program_options::variables_map vm;

  try {
    // parse the options given in input, and check them against positional and
    // cmdline options: store the result in vm
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
    // error while parsing the options given in input
    std::cerr << "Error: " << e.what() << "." << std::endl
              << "Try '" << argv[0] << " --help' for more information."
              << std::endl;

    return 1;
  }

  if (vm.count("help")) {
    // help option specified
    std::cerr << general_options << std::endl;

    return 1;
  }

  if (!vm.count("files")) {
    // no input files specified
    std::cerr << "Error: No input file specified." << std::endl
              << "Try '" << argv[0] << " --help' for more information."
              << std::endl;

    return 1;
  }

  // enable full search when interactive is enabled
  runtime::full |= runtime::interactive;

  // enable logging when full search is enabled
  logging::enabled |= runtime::full;

  std::vector<std::string>::const_iterator file = files.begin();
  for (; file < files.end(); ++file) {
    // for each filename in files
    RBGraph g;

    try {
      read_graph(*file, g);
    }
    catch (const std::exception& e) {
      std::cerr << "No (" << *file << ") " << e.what() << std::endl;

      continue;
    }

    try {
      // std::cout << "   (" << *file << ")" << std::endl;

      std::list<SignedCharacter> output = reduce(g);

      std::stringstream reduction;

      SignedCharacterIter sci = output.begin();
      for (; sci != output.end(); ++sci) {
        reduction << *sci << " ";
      }

      // initialize the python interpreter
      setenv("PYTHONPATH", "bin", 1);
      Py_Initialize();

      // import check_reduction.py
      boost::python::object pymod = boost::python::import("check_reduction");
      // run the function check_reduction(filename, reduction), store its
      // output in pycheck
      boost::python::object pycheck = pymod.attr("check_reduction")
                                                (*file, reduction.str());

      if (!boost::python::extract<bool>(pycheck)())
        // check_reduction(filename, reduction) returned False
        throw NoReduction();

      std::cout << "Ok (" << *file << ") < "
                << reduction.str() << ">" << std::endl;
    }
    catch (const NoReduction& e) {
      std::cout << "No (" << *file << ") " << e.what() << std::endl;
    }
  }

  return 0;
}
