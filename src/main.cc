#include "main.hh"

std::string debug = "";
std::string verbose = "";
std::string defaultInput = "./Data/SU3_24_24_24_48_6.2000_1000_PHB_4_OR_7_dp.bin";
std::string defaultOutput = "Output/SU3_24_24_24_48_6.2000_1000_PHB_4_OR_7_dp.csv";
constexpr std::array<size_t, 4> param_Grid = {24, 24, 24, 48};

void showHelp() {
    /*Show help for input arguments*/
    std::cout << "-i : Input file name, default " << defaultInput << "\n";
    std::cout << "-o : Output file name, default " << defaultOutput << "\n";
    std::cout << "-d : Run in debug point, default none\n";
    std::cout << "-v : Run in verbose point, default off\n";
}

std::map<std::string, std::string> getOptions(int argc, char* argv[]) {
    /*Interpret input arguments*/
    std::map<std::string, std::string> options;
    options.insert(std::make_pair("-i", defaultInput)); //Input file
    options.insert(std::make_pair("-o", defaultOutput)); //Output name
    options.insert(std::make_pair("-d", "")); //Debug mode
    options.insert(std::make_pair("-v", "")); //Verbose mode

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

void runWilsonExperiment(Lattice* config, std::string name) {
    /*Loop over range of R and T values and compute mean of corresponding Wilson loops*/
    std::ofstream outFile;
    outFile.precision(50);
    outFile.open(name);
    outFile << "R,T,Mean,Std\n";

    std::pair<double, double> mean;
    for (size_t R = 1; R <= 2;R++) {//config->getShape()[0]/2; R++) {
        for (size_t T = 1; T <= 2;T++) {//config->getShape()[3]/4; T++) {
            if (verbose != "") std::cout << "(R, T) = " << R << ", " << T << ", mean = ";
            mean = config->calcOverallMeanWilsonLoop(R, T);
            outFile << R << "," << T << "," << mean.first << "," << mean.second << "\n";
            if (verbose != "") std::cout << mean.first << "+-" << mean.second << "\n";
        }
    }

    outFile.close();
}

void runWilsonExperimentMP(Lattice* config, std::string name) {
    /*Loop over range of R and T values and compute mean of corresponding Wilson loops*/
    std::ofstream outFile;
    outFile.precision(50);
    outFile.open(name);
    outFile << "R,T,Mean\n";

    double mean;
    for (size_t R = 1; R <= config->getShape()[0]/2; R++) {
        for (size_t T = 1; T <= config->getShape()[3]/4; T++) {
            if (verbose != "") std::cout << "(R, T) = " << R << ", " << T << ", mean = ";
            mean = config->calcOverallMeanWilsonLoopMP(R, T);
            outFile << R << "," << T << "," << mean << "\n";
            if (verbose != "") std::cout << mean << "\n";
        }
    }

    outFile.close();
}

int main(int argc, char *argv[]) {
    std::map<std::string, std::string> options = getOptions(argc, argv); //Get parsed arguments
    if (options.size() == 0) {
        return 1;
    }
    debug = options["-d"];
    verbose = options["-v"];

     std::cout << "Loading config: " << options["-i"] << "\n";
	Lattice* config = new Lattice(param_Grid, options["-i"], verbose, debug);
    std::cout << "Config loaded\n";
    std::cout << "Running Wilson loop experiment and outputting results to: " << options["-o"] << "\n";
    runWilsonExperimentMP(config, options["-o"]);
}