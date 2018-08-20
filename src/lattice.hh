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
#include "xtensor/xindex_view.hpp"
//Project
#include "misc.hh"

class Lattice {
	/*Class for holding a specific configuration and providing methods to load configs and compute plaquettes*/

private:
	std::array<size_t, 4> _shape;
	lattice _config;
	std::string _verbose;
	std::string _debug;

public:
	Lattice(std::array<size_t, 4>, std::string, std::string, std::string);
	~Lattice();
	std::string getDim(size_t);
	void readConfig(std::string);
	std::complex<double> calcPlaquette(std::array<size_t, 4>, std::pair<size_t, size_t>);
	std::complex<double> calcMeanPlaquette(std::array<size_t, 4>);
	std::array<size_t, 4> movePoint(std::array<size_t, 4>, size_t, int);
	double getOverallPlaquetteMean();
	std::complex<double> calcWilsonLoop(std::array<size_t, 4>, size_t, size_t, size_t);
	std::complex<double> calcMeanWilsonLoopAtPoint(std::array<size_t, 4>, size_t, size_t);
	std::complex<double> calcOverallMeanWilsonLoop(size_t, size_t);
	xt::xtensor<double, 1> getWilsonLoopSample(size_t, size_t);
	xt::xtensor<double, 1> jackknifeWilson(size_t, size_t);
	std::pair<double, double> getJackknifeWilsonMean(size_t, size_t);
	std::array<size_t, 4> getShape();

	void test();
};

#endif /* LATTICE_HH_ */