# LatticeQCD IST2018
Repo for lattice QCD project for IST QCD course, 2018

## Requirements:
### Experiment:
- C++14
- [JSON](https://github.com/nlohmann/json)
- [OpenBLAS](https://github.com/xianyi/OpenBLAS)
- [LAPACK](https://github.com/Reference-LAPACK/lapack-release)
- [XTL](https://github.com/QuantStack/xtl)
- [XTensor 0.16.3](https://github.com/QuantStack/xtensor/tree/0.16.3)
- [XTensor-BLAS 0.11.1](https://github.com/QuantStack/xtensor-blas/tree/0.11.1)

### Analysis:
- Python 3
- Jupyter
- Numpy
- Pandas
- Seaborn
- Matplotlib
- Scipy

## How to run:
1. Ensure depandancies installed and pats set
1. Build with `make` in top directory
1. Run using `./bin/main.exe`, `-d [deubug point]` can be used to exit once program reaches specified point (requires debug point in code), `-v [verbose point]` can be used to get verbose output of specifed function (must be set in code beforehand).

### Debug points
- load - exits after loading first SU(3) matrix from lattice config
- calcPlaquette - exits after calculating one plaquette
- calcWilsonLoop - exits after calculating one Wilson loop
- calcMeanWilsonLoopAtPoint - exits after calculating mean of Wilson loops at one point

### Verbosity points
- [anything] - prints progress of main program
- load - print progress of lattice config loading
- movePoint - prints calculation details of moving withing the lattice, e.g. to check boundary conditions
- calcPlaquette - prints values of link variables during plaquette computation and plaquette variable and trace
- calcMeanPlaquette - prints values of possible plaquttes at given point, and their various means
- getOverallPlaquetteMean
- calcWilsonLoop
- calcMeanWilsonLoopAtPoint
- calcOverallMeanWilsonLoop
- getWilsonLoopSample

## Status
- Multi-thread computation of Wilson loop means
- ~~Can save values of V(R,T) for given configuration as CSV and analyse in python~~
- ~~Can compute mean of Wilson loops for arbitrary spatial and temporal widths across entire lattice~~
- ~~Can compute Wilson loops for arbitrary spatial and temporal widths~~
- ~~Can compute mean of all plaquettes across lattice~~
- ~~Can compute means of spatial:spatial, spatial:temporal, and all plaquettes at given point~~
- ~~Code in place to compute plaquettes, but calculations are incorrect~~
- ~~Converted reader to class~~
- ~~Reads in entire configuration~~
- ~~Checks that SU(3) matrices really are unitary~~
- ~~Reads in first SU(3) matrix~~

## To do
