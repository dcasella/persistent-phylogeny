#include "functions.hpp"
#include <boost/program_options.hpp>
#include <boost/python.hpp>


void conflicting_options(const boost::program_options::variables_map& vm,
                         const std::string& opt1, const std::string& opt2) {
  if (vm.count(opt1) && !vm[opt1].defaulted() &&
      vm.count(opt2) && !vm[opt2].defaulted()) {
    throw std::logic_error(
      std::string("conflicting options --") + opt1 + " and --" + opt2
    );
  }
}


int main(int argc, const char* argv[]) {
  // declare the vector of input files
  std::vector<std::string> files;

  // initialize options menu
  boost::program_options::options_description general_options(
    "Usage: ppp [OPTION...] FILE..."
    "\n"
    "Calculate a successful c-reduction for the matrix(ces) in FILE(s), if it "
    "exists."
    "\n\n"
    "Options"
  );

  general_options.add_options()
  // option: help message
  (
    "help,h",
    "Display this message.\n"
  )
  // option: verbose, print information on the ongoing operations
  (
    "verbose,v",
    boost::program_options::bool_switch(&logging::enabled),
    "Display the operations performed by the program.\n"
  )
  // option: testpy, test reduce output with a python script
  (
    "testpy,t",
    boost::program_options::bool_switch()->default_value(false),
    "Test the output of the algorithm with check_reduction.py.\n"
  )
  // option: exponential, test every possible combination of safe sources
  (
    "exponential,x",
    boost::program_options::bool_switch(&exponential::enabled),
    "Exponential version of the algorithm.\n"
    "(Mutually exclusive with --interactive)\n"
    "(Mutually exclusive with --nthsource)\n"
  )
  // option: interactive, let the user select which path to take
  (
    "interactive,i",
    boost::program_options::bool_switch(&interactive::enabled),
    "User input driven execution.\n"
    "(Mutually exclusive with --exponential)\n"
    "(Mutually exclusive with --nthsource)\n"
  )
  // option: maximal, read graph and reduce it to maximal
  (
    "maximal,m",
    boost::program_options::bool_switch()->default_value(false),
    "Run the algorithm on the maximal subgraph.\n"
  )
  // option: nthsource, pick the nth safe source instead of the first
  (
    "nthsource,n",
    boost::program_options::value<size_t>(&nthsource::index)
      ->default_value(0),
    "Select the nth safe source when possible.\n"
    "(Mutually exclusive with --exponential)\n"
    "(Mutually exclusive with --interactive)\n"
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
  // add the options menu and the hidden options to the available options
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

    conflicting_options(vm, "exponential", "interactive");

    conflicting_options(vm, "nthsource", "exponential");
    conflicting_options(vm, "nthsource", "interactive");

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

  if (files.size() > 1) {
    std::cout << "Running PPP on " << files.size() << " files."
              << std::endl << std::endl;
  }

  boost::python::object pymod;

  if (vm["testpy"].as<bool>()) {
    // initialize the python interpreter
    setenv("PYTHONPATH", "bin", 1);
    Py_Initialize();

    // import check_reduction.py
    pymod = boost::python::import("check_reduction");
  }

  size_t count_file = 0;
  for (const std::string& file : files) {
    // for each filename in files

    if (logging::enabled) {
      // verbosity enabled
      std::cout << "F  (" << file << ")" << std::endl;
    }
    else {
      // verbosity disabled
      if (files.size() > 1) {
        const double d_perc = std::floor(100.0 * count_file / files.size());
        const size_t perc = static_cast<size_t>(d_perc);

        if (perc < 10)
          std::cout << " ";

        std::cout << "\033[32m" << perc
                  << "\033[39m ("<< file << ")" << std::flush;
      }
      else {
        std::cout << "F  (" << file << ")" << std::flush;
      }
    }

    count_file++;

    RBGraph g;

    try {
      read_graph(file, g);

      std::stringstream keep_c{};

      if (vm["maximal"].as<bool>()) {
        RBGraph gm = maximal_reducible_graph(g);

        if (vm["testpy"].as<bool>()) {
          RBVertexIter v, v_end;
          std::tie(v, v_end) = vertices(gm);
          for (; v != v_end; ++v) {
            if (!is_character(*v, gm))
              continue;

            keep_c << g[*v].name.substr(1) << " ";
          }
        }

        g.clear();
        copy_graph(gm, g);
      }

      const std::list<SignedCharacter> output = reduce(g);

      std::stringstream reduction;
      for (const SignedCharacter& sc : output) {
        reduction << sc << " ";
      }

      if (vm["testpy"].as<bool>()) {
        if (vm["maximal"].as<bool>()) {
          // run the function check_reduction(filename, reduction), store its
          // output in pycheck
          boost::python::object pycheck = pymod.attr("check_reduction")
                                                    (file, reduction.str(),
                                                     keep_c.str());

          if (!boost::python::extract<bool>(pycheck)())
            // check_reduction(filename, reduction) returned False
            throw NoReduction();
        }
        else {
          // run the function check_reduction(filename, reduction), store its
          // output in pycheck
          boost::python::object pycheck = pymod.attr("check_reduction")
                                                    (file, reduction.str());

          if (!boost::python::extract<bool>(pycheck)())
            // check_reduction(filename, reduction) returned False
            throw NoReduction();
        }
      }

      if (!logging::enabled) {
        // verbosity disabled
        std::cout << '\r';
      }

      std::cout << "Ok (" << file << ")";

      if (logging::enabled) {
        // verbosity enabled
        if (exponential::enabled) {
          // exponential algorithm enabled
          std::cout << ": Successful reductions have been logged";
        }
        else {
          std::cout << ": < " << reduction.str() << ">";
        }
      }

      std::cout << std::endl;
    }
    catch (const boost::python::error_already_set& e) {
      if (!logging::enabled) {
        // verbosity disabled
        std::cout << '\r';
      }

      std::cout << "No (" << file << ")";

      if (logging::enabled) {
        // verbosity enabled
        std::cout << ": Python error";
      }

      std::cout << std::endl;
    }
    catch (const std::exception& e) {
      if (!logging::enabled) {
        // verbosity disabled
        std::cout << '\r';
      }

      std::cout << "No (" << file << ")";

      if (logging::enabled) {
        // verbosity enabled
        std::cout << ": " << e.what();
      }

      std::cout << std::endl;
    }
  }

  return 0;
}
