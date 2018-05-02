#include "testreader.hh"

bool debug = false;

/*template <class Real,  int Ncolors>
    class _su3 {
    _complex<Real> e[Ncolors][Ncolors];
};*/

void showHelp() {
    /*Show help for input arguments*/
    std::cout << "-i : Input file name, default ./Data/SU3_24_24_24_48_6.2000_1000_PHB_4_OR_7_dp.bin\n";
    std::cout << "-o : Output file name, default Output/test.txt\n";
    std::cout << "-d : Run in debug mode [0/1], default 0\n";
}

std::map<std::string, std::string> getOptions(int argc, char* argv[]) {
    /*Interpret input arguments*/
    std::map<std::string, std::string> options;
    options.insert(std::make_pair("-i", "./Data/SU3_24_24_24_48_6.2000_1000_PHB_4_OR_7_dp.bin")); //Input file
    options.insert(std::make_pair("-o", "Output/test.txt")); //Output name
    options.insert(std::make_pair("-d", "0")); //Debug mode

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

std::string getDim(int m) {
    /*Lookup direction from iterator*/
    if (m == 0) return "x";
    if (m == 1) return "y";
    if (m == 2) return "z";
    if (m == 3) return "t";
}

int main(int argc, char *argv[]) {
    /*Read in file to check correct reading*/
    std::map<std::string, std::string> options = getOptions(argc, argv); //Get parsed arguments
    if (options.size() == 0) {
        return 1;
    }

    std::string configName(options["-o"]);
    std::ifstream filein(configName.c_str(), std::ios::in | std::ios::binary);
    std::vector<int> param_Grid(4,24);
    param_Grid[3] = 48; //Time dimension
    double real, imaginary;

    //Lattice iteration
    for (int t = 0; t < param_Grid[3]; t++) { //Loop over t
        for (int k = 0; k < param_Grid[2]; k++) { //Loop over z
            for (int j = 0; j < param_Grid[1];j ++) { //Loop over y
                for (int i = 0; i < param_Grid[0]; i++) { //Loop over x

                    //Directional SU(3) iteration
                    for (int m = 0; m < 4; m++) { //Loop through SU(3) matrices
                        std::cout << "\nSU(3) matrix at lattice point (" << i << ", " << j << ", " << k << ", " << t << ") in " << getDim(m) << " direction:\n";

                        //Elements of SU(3) matrix
                        for (int a = 0; a < 3; a++) { //Loop through columns of SU(3) matrix
                            for (int b = 0; b < 3; b++) { //Loop through rows of SU(3) matrix
                                filein.read((char*)&real, 8);
                                filein.read((char*)&imaginary, 8);
                                std::cout << "(" << a << ", " << b << "): " << real << " + " << imaginary << "*i\n";
                            } 
                        } //Elements
                        return 2; //Only print one SU(3) matrix

                    } //SU(3) matrices

                }
            }
        }
    } //Lattice points

    filein.close();
    return 0;
}