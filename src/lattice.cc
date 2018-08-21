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
        std::cout << "U prime matrix:\n" << _config[tmp_point][plane.first] << "\nconjugate transpose:\n" << uprime << "\n At point: (" << tmp_point[0] << "," << tmp_point[1] << "," << tmp_point[2] << "," << tmp_point[3] << ")\n";
    }
    
    //Reverse of link in nu direction at point
    su3Matrix vprime = xt::conj(xt::transpose(_config[point][plane.second]));
    if (_verbose == "calcPlaquette") std::cout << "V prime matrix:\n" << _config[point][plane.second] << "\nconjugate transpose:\n" << vprime << "\n At point: (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ")\n";
    
    //Compute plaquette

    su3Matrix product = xt::linalg::dot(u, xt::linalg::dot(v, xt::linalg::dot(uprime, vprime)));
    if (_verbose == "calcPlaquette") std::cout << "Plaquette product:\n" << product << "\n";
    std::complex<double> trace = xt::sum(xt::diagonal(product))[0];
    if (_verbose == "calcPlaquette") std::cout << "Plaquette trace: " << trace << "\n\n";

    if (_debug == "calcPlaquette") throw std::runtime_error("Debug mode: Only try one product");

    return trace/3.;
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

double Lattice::getOverallPlaquetteMean() {
    /*Calculate means of plaquettes at all lattice points and return real part*/
    double sum = 0;
    double tmp_mean;
    size_t p = 0;
    
    //Lattice iteration
    for (size_t t = 0; t < _shape[3]; t++) { //Loop over t
        for (size_t z = 0; z < _shape[2]; z++) { //Loop over z
            for (size_t y = 0; y < _shape[1]; y++) { //Loop over y
                for (size_t x = 0; x < _shape[0]; x++) { //Loop over x
                    tmp_mean = calcMeanPlaquette({x,y,z,t}).real();
                    sum +=  tmp_mean;
                    if (_verbose == "getOverallPlaquetteMean") std::cout << "Mean at (" << x << "," << y << "," << z << "," << t << "): " << tmp_mean << "\n";
                    p++;
                }
            }
        }
    }

    double mean = sum/p;
    if (_verbose == "getOverallPlaquetteMean") std::cout << "Overall mean: " << mean << "\n";
    return mean;
}

std::complex<double> Lattice::calcWilsonLoop(std::array<size_t, 4> point, size_t spatialDimension, size_t R, size_t T) {
    /*Compute latice loop starting at given point with spatial width r and temporal width t in given spatial direction*/
    if (_verbose == "calcWilsonLoop") std::cout << "\nCalculating Wilson loop at (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ") in " << getDim(spatialDimension) << " direction for (R,T) = (" << R << "," << T << ")\n";
    
    su3Matrix product({{1,0,0},{0,1,0},{0,0,1}});
    su3Matrix tmp;

    //Reverse link in temporal direction
    for (size_t i = 0; i < T; i++) {
        tmp = xt::conj(xt::transpose(_config[point][3]));
        product = xt::linalg::dot(tmp, product);
        if (_verbose == "calcWilsonLoop") std::cout << "Reverse temporal link " << i << " at point: (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ") is\n" << _config[point][3] << "\nconjugate transpose\n" << tmp << "\n";
        point = movePoint(point, 3, 1);
    }

    //Reverse link in spatial direction
    for (size_t i = 0; i < R; i++) {
        tmp = xt::conj(xt::transpose(_config[point][spatialDimension]));
        product = xt::linalg::dot(tmp, product);
        if (_verbose == "calcWilsonLoop") std::cout << "Reverse spatial link " << i << " at point: (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ") is\n" << _config[point][spatialDimension] << "\nconjugate transpose\n" << tmp << "\n";
        point = movePoint(point, spatialDimension, 1);  
    }

    //Link in temporal direction
    for (size_t i = 0; i < T; i++) {
        point = movePoint(point, 3, -1);
        product = xt::linalg::dot(_config[point][3], product);
        if (_verbose == "calcWilsonLoop") std::cout << "Temporal link " << i << " at point: (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ") is\n" << _config[point][3] << "\n";
    }

    //Link in spatial direction
    for (size_t i = 0; i < R; i++) {
        point = movePoint(point, spatialDimension, -1);
        product = xt::linalg::dot(_config[point][spatialDimension], product);
        if (_verbose == "calcWilsonLoop") std::cout << "Spatial link " << i << " at point: (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ") is\n" << _config[point][spatialDimension] << "\n";
    }

    //Compute trace
    if (_verbose == "calcWilsonLoop") std::cout << "Wilson loop product:\n" << product << "\n";
    std::complex<double> trace = xt::sum(xt::diagonal(product))[0];
    if (_verbose == "calcWilsonLoop") std::cout << "Wilson loop trace: " << trace << "\n\n";

    if (_debug == "calcWilsonLoop") throw std::runtime_error("Debug mode: Only try one product");

    return trace/3.;
}

std::complex<double> Lattice::calcMeanWilsonLoopAtPoint(std::array<size_t, 4> point, size_t R, size_t T) {
    /*Calulcate mean Wilson loop with spatial width r and temporal width t across all spatial dimensions at given point*/
    if (_verbose == "calcMeanWilsonLoopAtPoint") std::cout << "\nCalculating mean Wilson loop at (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ") for (R,T) = (" << R << "," << T << ")\n";
    
    xt::xtensor_fixed<std::complex<double>, xt::xshape<3>> loops;

    for (size_t i = 0; i < 3; i++) {
        loops[i] = calcWilsonLoop(point, i, R, T);
        if (_verbose == "calcMeanWilsonLoopAtPoint") std::cout << "\n Wilson loop at (" << point[0] << "," << point[1] << "," << point[2] << "," << point[3] << ") in " << getDim(i) << " direction for (R,T) = (" << R << "," << T << ") is " << loops[i] << "\n";
    }

    std::complex<double> mean = xt::mean(loops)[0];
    if (_verbose == "calcMeanWilsonLoopAtPoint") std::cout << "Mean: " << mean << "\n";
    if (_debug == "calcMeanWilsonLoopAtPoint") throw std::runtime_error("Debug mode: Only try one point");

    return mean;
}

std::pair<double, double> Lattice::calcOverallMeanWilsonLoop(size_t R, size_t T) {
    /*Get mean of all Wilson loops*/
    if (_verbose == "calcOverallMeanWilsonLoop") std::cout << "\nCalculating all Wilson loops of (R,T) = (" << R << "," << T << ")\n";
    xt::xtensor<double, 1> traces = getWilsonLoopSample(R, T);
    double mean = xt::mean(traces)[0];
    double std = sqrt(xt::sum(xt::pow(traces-mean, 2))[0]/static_cast<double>(traces.size()));

    if (_verbose == "calcOverallMeanWilsonLoop") std::cout << "Mean: " << xt::mean(traces) << "+-" << std << "\n";
    return std::make_pair(mean, std);
}

std::array<size_t, 4> Lattice::getShape() {
    return _shape;
}

xt::xtensor<double, 1> Lattice::getWilsonLoopSample(size_t R, size_t T) {
    /*Calculate all Wilson loops of spatial width r and temporal width t across entire lattice*/
    if (_verbose == "getWilsonLoopSample") std::cout << "\nCalculating all Wilson loops of (R,T) = (" << R << "," << T << ")\n";

    xt::xtensor<double, 1> traces = xt::xtensor<double, 1>(std::array<size_t, 1>{_shape[0]*_shape[1]*_shape[2]*_shape[3]*3});

    //Lattice iteration
    int p = 0;
    for (size_t t = 0; t < _shape[3]; t++) { //Loop over t
        for (size_t z = 0; z < _shape[2]; z++) { //Loop over z
            for (size_t y = 0; y < _shape[1]; y++) { //Loop over y
                for (size_t x = 0; x < _shape[0]; x++) { //Loop over x
                    //Direction iteration
                    for (size_t i = 0; i < 3; i++) {
                        traces[p] = calcWilsonLoop({x,y,z,t}, i, R, T).real();
                        if (_verbose == "getWilsonLoopSample") std::cout << "Loop at (" << x << "," << y << "," << z << "," << t << ") in direction " << getDim(i) << ": " << traces[p] << "\n";
                        p++;
                    }
                }
            }
        }
    }

    if (_verbose == "getWilsonLoopSample") std::cout << "Overall mean: " << xt::mean(traces) << "\n";
    return traces;
}

double Lattice::calcOverallMeanWilsonLoopMP(size_t R, size_t T) {
    /*Calculate mean of all Wilson loops of spatial width r and temporal width t across entire lattice with multi processing*/
    double sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (size_t t = 0; t < _shape[3]; t++) { //Loop over t
        for (size_t z = 0; z < _shape[2]; z++) { //Loop over z
            for (size_t y = 0; y < _shape[1]; y++) { //Loop over y
                for (size_t x = 0; x < _shape[0]; x++) { //Loop over x
                    for (size_t i = 0; i < 3; i++) { //Direction iteration
                        sum += calcWilsonLoop({x,y,z,t}, i, R, T).real();
                    }
                }
            }
        }
    }

    return sum/(_shape[0]*_shape[1]*_shape[2]*_shape[3]*3);
}