#include "lattice.hh"

std::pair<bool, double> doubleCompare(double value, double target,
                                      double tolerence = 1.5e-15) {
    /*Compare whether two doubles are approximately equal*/
    double relDif = std::abs((value-target)/std::min(value, target));
    return std::make_pair(relDif < tolerence, relDif);
}

Lattice::Lattice(std::array<size_t, 4> shape, std::string configName, 
                 bool verbose=false, bool debug=false) {
    /*Initialise shape and read in configuration*/

    _shape = shape;
    _config = lattice(_shape);
    _verbose = verbose;
    _debug = debug;

    Lattice::readConfig(configName);
}

Lattice::~Lattice() { }

std::string Lattice::getDim(int m) {
    /*Lookup direction from iterator*/
    if (m == 0) return "x";
    if (m == 1) return "y";
    if (m == 2) return "z";
    if (m == 3) return "t";
    return "Invalid";
}

void Lattice::readConfig(std::string configName) {
    /*Read inconfiguration from file*/

    if (_verbose) std::cout << "Reading configuration from: " << configName << "\n";
    std::ifstream filein(configName.c_str(), std::ios::in | std::ios::binary);

    double real, imaginary;
    std::complex<double> det;

    //Lattice iteration
    for (size_t t = 0; t < _shape[3]; t++) { //Loop over t
        for (size_t z = 0; z < _shape[2]; z++) { //Loop over z
            for (size_t y = 0; y < _shape[1];y++) { //Loop over y
                for (size_t x = 0; x < _shape[0]; x++) { //Loop over x

                    direction tmp_direction;
                    //Directional SU(3) iteration
                    for (size_t d = 0; d < 4; d++) { //Loop through SU(3) matrices
                        if (_verbose) std::cout << "\nSU(3) matrix at lattice point (" << x << ", " << y << ", " << z << ", " << t << ") in " << Lattice::getDim(d) << " direction:\n";

                        su3Matrix tmp_su3Matrix;
                        //Elements of SU(3) matrix
                        for (size_t a = 0; a < 3; a++) { //Loop through columns of SU(3) matrix
                            for (size_t b = 0; b < 3; b++) { //Loop through rows of SU(3) matrix
                                filein.read((char*)&real, 8);
                                filein.read((char*)&imaginary, 8);
                                if (_verbose) std::cout << "(" << a << ", " << b << "): " << real << " + " << imaginary << "*i\n";
                                tmp_su3Matrix(a, b) = std::complex<double>{real, imaginary};
                            } 
                        } //Elements

                        det = xt::linalg::det(tmp_su3Matrix);
                        if (_verbose) std::cout << "Det = " << det << "\n";
                        if (!doubleCompare(det.real(), 1.0).first | !doubleCompare(det.imag()+1, 1.0).first) {
                            std::cout << "Matrix at (" << x << ", " << y << ", " << z << ", " << t << ") in " << Lattice::getDim(d) << " direction is not unitary\n";
                            std::cout << "Relative distances are: " << doubleCompare(det.real(), 1.0).second << " and " << doubleCompare(det.imag()+1, 1.0).second << "\n";
                            throw std::runtime_error("Non-unitary matrix");
                        }

                        tmp_direction(d) = tmp_su3Matrix;
                        if (_debug) throw std::runtime_error("Debug mode: Only print one SU(3) matrix");

                    } //SU(3) matrices
                    _config(x,y,z,t) = tmp_direction;
                }
            }
        }
    } //Lattice points

    filein.close();
}