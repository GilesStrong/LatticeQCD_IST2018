#include "main.hh"

bool debug = false;
bool verbose = false;
std::string defaultInput = "./Data/SU3_24_24_24_48_6.2000_1000_PHB_4_OR_7_dp.bin";
std::string defaultOutput = "Output/test.txt";
constexpr std::array<size_t, 4> param_Grid = {24, 24, 24, 48};

void showHelp() {
    /*Show help for input arguments*/
    std::cout << "-i : Input file name, default " << defaultInput << "\n";
    std::cout << "-o : Output file name, default " << defaultOutput << "\n";
    std::cout << "-d : Run in debug mode [0/1], default 1\n";
    std::cout << "-v : Run in verbose mode [0,1,2], default 1\n";
}

std::map<std::string, std::string> getOptions(int argc, char* argv[]) {
    /*Interpret input arguments*/
    std::map<std::string, std::string> options;
    options.insert(std::make_pair("-i", defaultInput)); //Input file
    options.insert(std::make_pair("-o", defaultOutput)); //Output name
    options.insert(std::make_pair("-d", "1")); //Debug mode
    options.insert(std::make_pair("-v", "1")); //Verbose mode

    if (argc >= 2) { //Check if help was requested
        std::string option(argv[1]);
        if (option == "-h" || option == "--help") {
            showHelp();
            options.clear();
            return options;
        }
    }

    for (int i = 1; i < argc; i = i+2) {
        std::string option(argv[i]);
        std::string argument(argv[i+1]);
        if (option == "-h" || option == "--help" || argument == "-h" || argument == "--help") { //Check if help was requested
            showHelp();
            options.clear();
            return options;
        }
        options[option] = argument;
    }
    return options;
}

int main(int argc, char *argv[]) {
    std::map<std::string, std::string> options = getOptions(argc, argv); //Get parsed arguments
    if (options.size() == 0) {
        return 1;
    }
    if (options["-d"] == "1") {
        debug = true;
        verbose = true;
        std::cout << "Running in debug mode\n";
    }
    else if (options["-v"] == "1") {
        verbose = true;
        std::cout << "Running with verbose output\n";
    }

    if (verbose) std::cout << "Loading config: " << options["-i"] << "\n";
	Lattice* config = new Lattice(param_Grid, options["-i"], verbose, debug);
    if (verbose) std::cout << "Config loaded\n";

}