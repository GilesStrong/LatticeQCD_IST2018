#include "lattice.hh"

std::pair<bool, double> doubleCompare(double value, double target,
                                      double tolerence = 1.5e-15) {
    /*Compare whether two doubles are approximately equal*/
    double relDif = std::abs((value-target)/std::min(value, target));
    return std::make_pair(relDif < tolerence, relDif);
}

Lattice::Lattice(std::array<size_t, 4> shape, std::string configName, 
                 std::string verbose=0, std::string debug="") {
    /*Initialise shape and read in configuration*/

    _shape = shape;
    _config = lattice(_shape);
    _verbose = verbose;
    _debug = debug;

    Lattice::readConfig(configName);
}

Lattice::~Lattice() { }

std::string Lattice::getDim(size_t m) {
    /*Lookup direction from iterator*/
    if (m == 0) return "x";
    if (m == 1) return "y";
    if (m == 2) return "z";
    if (m == 3) return "t";
    return "Invalid";
}

void Lattice::readConfig(std::string configName) {
    /*Read inconfiguration from file*/

    if (_verbose == "load") std::cout << "Reading configuration from: " << configName << "\n";
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
                        if (_verbose == "load") std::cout << "\nSU(3) matrix at lattice point (" << x << ", " << y << ", " << z << ", " << t << ") in " << Lattice::getDim(d) << " direction:\n";

                        su3Matrix tmp_su3Matrix;
                        //Elements of SU(3) matrix
                        for (size_t a = 0; a < 3; a++) { //Loop through columns of SU(3) matrix
                            for (size_t b = 0; b < 3; b++) { //Loop through rows of SU(3) matrix
                                filein.read((char*)&real, 8);
                                filein.read((char*)&imaginary, 8);
                                if (_verbose == "load") std::cout << "(" << a << ", " << b << "): " << real << " + " << imaginary << "*i\n";
                                tmp_su3Matrix(a, b) = std::complex<double>{real, imaginary};
                            } 
                        } //Elements

                        det = xt::linalg::det(tmp_su3Matrix);
                        if (_verbose == "load") std::cout << "Det = " << det << "\n";
                        if (!doubleCompare(det.real(), 1.0).first | !doubleCompare(det.imag()+1, 1.0).first) {
                            std::cout << "Matrix at (" << x << ", " << y << ", " << z << ", " << t << ") in " << Lattice::getDim(d) << " direction is not unitary\n";
                            std::cout << "Relative distances are: " << doubleCompare(det.real(), 1.0).second << " and " << doubleCompare(det.imag()+1, 1.0).second << "\n";
                            throw std::runtime_error("Non-unitary matrix");
                        }

                        tmp_direction(d) = tmp_su3Matrix;
                        if (_debug == "load") throw std::runtime_error("Debug mode: Only print one SU(3) matrix");

                    } //SU(3) matrices
                    _config(x,y,z,t) = tmp_direction;
                }
            }
        }
    } //Lattice points

    filein.close();
}

void Lattice::test(){
    /*Test function for random things*/
    std::array<size_t, 4> test = {0,2,4,5};
    //_config[test[0]][test[1]][test[2]][test[3]];
    std::cout << _config[test] << "\n";
}

size_t modulo(int a, int b) {
    /*Modulo that also works with negaive numbers*/
    return (a%b+b)%b;
}

std::array<size_t, 4> Lattice::movePoint(std::array<size_t, 4> point, size_t direction, int amount) {
    /*Move position in grid whilst respecting peiodic boundaries*/
    if (_verbose == "movePoint") {
        std::cout << "\nCurrect point: " << point[direction] << " and moving " << amount << " steps\n";
        std::cout << "Candidate point is: " << static_cast<int>(point[direction])+amount << "\n";
        std::cout << "Periodic boundary conditions means new point is: " << modulo(static_cast<int>(point[direction])+amount, _shape[direction]) << "\n";
    }
    point[direction] = modulo(static_cast<int>(point[direction])+amount, _shape[direction]);
    return point;
}

std::complex<double> Lattice::calcPlaquette(std::array<size_t, 4> point, std::pair<size_t, size_t> plane) {
    /*Calculate value of plaquette at specified starting gridpoint and 2D plane*/
    if (_verbose == "calcPlaquette") std::cout << "\nCalculating plaquette at (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ") in plane (" << getDim(plane.first) << ":" << getDim(plane.second) << ")\n";

    //Link in mu direction at point
    su3Matrix u = _config[point][plane.first]; 
    if (_verbose == "calcPlaquette") std::cout << "U matrix:\n" << u << "\n At point: (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ")\n";
    
    //Link in nu direction at point+mu
    std::array<size_t, 4> tmp_point = movePoint(point, plane.first, 1);
    su3Matrix v = _config[tmp_point][plane.second]; 
    if (_verbose == "calcPlaquette") {
        std::cout << "V matrix:\n" << v << "\n At point: (" << tmp_point[0] << "," << tmp_point[1] << "," << tmp_point[2] << "," << tmp_point[3] << ")\n";;
    }
    
    //Reverse of link in mu direction at point+nu
    tmp_point = movePoint(point, plane.second, 1);
    su3Matrix uprime = xt::conj(xt::transpose(_config[tmp_point][plane.first])); 
    if (_verbose == "calcPlaquette") {
        std::cout << "U prime matrix:\n" << _config[tmp_point][plane.first] << "\ncomplex transpose:\n" << uprime << "\n At point: (" << tmp_point[0] << "," << tmp_point[1] << "," << tmp_point[2] << "," << tmp_point[3] << ")\n";
    }
    
    //Reverse of link in nu direction at point
    su3Matrix vprime = xt::conj(xt::transpose(_config[point][plane.second]));
    if (_verbose == "calcPlaquette") std::cout << "V prime matrix:\n" << _config[tmp_point][plane.second] << "\ncomplex transpose:\n" << vprime << "\n At point: (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ")\n";

    //Compute plaquette
    su3Matrix product = u*v*uprime*vprime;
    if (_verbose == "calcPlaquette") std::cout << "Plaquette product:\n" << product << "\n";
    std::complex<double> trace = xt::sum(xt::diagonal(product))[0];
    if (_verbose == "calcPlaquette") std::cout << "Plaquette trace: " << trace << "\n\n";

    return trace;
}

std::complex<double> Lattice::calcMeanPlaquette(std::array<size_t, 4> point) {
    /*Compute mean value of possible plaquttes at given point*/
    if (_verbose == "calcMeanPlaquette") std::cout << "\nCalculating plaquettes at (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ")\n";
    xt::xtensor_fixed<std::complex<double>, xt::xshape<6>> plaquttes;

    //Compute all possible plaquttes at point
    size_t p = 0;
    for (size_t i = 0; i <= 2; i++) {
        for (size_t j = i+1; j <= 3; j++) {
            plaquttes[p] = calcPlaquette(point, {i,j});
            if (_verbose == "calcMeanPlaquette") std::cout << "Plaquette (" << getDim(i) << ":" << getDim(j) << ") is: " << plaquttes[p] << "\n";
            p++;
        }
    }

    //Compute means of plaquettes
    if (_verbose == "calcMeanPlaquette") {
        std::cout << "Mean spatial:spatial: " << xt::mean(xt::index_view(plaquttes, {{0},{1},{3}})) << "\n";
        std::cout << "Mean spatial:temporal: " << xt::mean(xt::index_view(plaquttes, {{2},{4},{5}})) << "\n";
    }
    std::complex<double> mean = xt::mean(plaquttes)[0];
    if (_verbose == "calcMeanPlaquette") std::cout << "Mean: " << mean << "\n";
    return mean;
}