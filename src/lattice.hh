#ifndef LATTICE_HH_
#define LATTICE_HH_

//C++
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <complex>
#include <utility>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <map>
#include <bitset>
#include <stdexcept>
//XTensor
#include "xtensor/xfixed.hpp"
#include "xtensor/xtensor.hpp"
#include "xtensor/xio.hpp"
#include "xtensor-blas/xlinalg.hpp"
//Project
#include "misc.hh"

class Lattice {
	/*Class for holding a specific configuration and providing methods to load configs and compute plaquettes*/

private:
	std::array<size_t, 4> _shape;
	lattice _config;
	bool _verbose;
	bool _debug;

public:
	Lattice(std::array<size_t, 4>, std::string, bool, bool);
	~Lattice();
	std::string getDim(int);
	void readConfig(std::string);
};

#endif /* LATTICE_HH_ */