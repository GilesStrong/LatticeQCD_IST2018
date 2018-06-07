#include "testreader.hh"

bool debug = false;
bool verbose = 0;
double tolerence = 1.5e-15;
std::string defaultInput = "../Data/SU3_24_24_24_48_6.2000_1000_PHB_4_OR_7_dp.bin";
std::string defaultOutput = "Output/test.txt";

std::pair<bool, double> doubleCompare(double value, double target) {
    double relDif = std::abs((value-target)/std::min(value, target));
    return std::make_pair(relDif < tolerence, relDif);
}

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
    if (options["-d"] == "1") {
        debug = true;
        verbose = true;
        std::cout << "Running in debug mode\n";
    }
    else if (options["-v"] == "1") {
        verbose = true;
        std::cout << "Running with verbose output\n";
    }

    if (verbose) std::cout << "Reading configuration from: " << options["-i"] << "\n";
    std::ifstream filein(options["-i"].c_str(), std::ios::in | std::ios::binary);
    double real, imaginary;
    std::complex<double> det;

    constexpr std::array<size_t, 4> param_Grid = {24, 24, 24, 48};
    typedef xt::xtensor_fixed<std::complex<double>, xt::xshape<3, 3>> su3Matrix;
    typedef xt::xtensor_fixed<su3Matrix, xt::xshape<4>> direction;
    typedef xt::xtensor_fixed<direction, xt::xshape<param_Grid[0], param_Grid[1], param_Grid[2], param_Grid[3]>> lattice;

    lattice config();

    //Lattice iteration
    for (int t = 0; t < param_Grid[3]; t++) { //Loop over t
        for (int z = 0; z < param_Grid[2]; z++) { //Loop over z
            for (int y = 0; y < param_Grid[1];y++) { //Loop over y
                for (int x = 0; x < param_Grid[0]; x++) { //Loop over x

                    direction tmp_direction();
                    //Directional SU(3) iteration
                    for (int d = 0; d < 4; d++) { //Loop through SU(3) matrices
                        if (verbose) std::cout << "\nSU(3) matrix at lattice point (" << x << ", " << y << ", " << z << ", " << t << ") in " << getDim(d) << " direction:\n";

                        su3Matrix tmp_su3Matrix(std::complex<double>{0,0});
                        //std::cout << tmp_su3Matrix(0) << "\n";
                        //Elements of SU(3) matrix
                        for (int a = 0; a < 3; a++) { //Loop through columns of SU(3) matrix
                            for (int b = 0; b < 3; b++) { //Loop through rows of SU(3) matrix
                                filein.read((char*)&real, 8);
                                filein.read((char*)&imaginary, 8);
                                if (verbose) std::cout << "(" << a << ", " << b << "): " << real << " + " << imaginary << "*i\n";
                                tmp_su3Matrix(a, b) = std::complex<double>{real, imaginary};
                            } 
                        } //Elements

                        det = xt::linalg::det(tmp_su3Matrix);
                        if (verbose) std::cout << "Det = " << det << "\n";
                        if (!doubleCompare(det.real(), 1.0).first | !doubleCompare(det.imag()+1, 1.0).first) {
                            std::cout << "Matrix at (" << x << ", " << y << ", " << z << ", " << t << ") in " << getDim(d) << " direction is not unitary\n";
                            std::cout << "Relative distances are: " << doubleCompare(det.real(), 1.0).second << " and " << doubleCompare(det.imag()+1, 1.0).second << "\n";
                            return 2;
                        }
                        if (debug) return 2; //Only print one SU(3) matrix

                    } //SU(3) matrices
                }
            }
        }
    } //Lattice points

    filein.close();
    return 0;
}