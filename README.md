# LatticeQCD IST2018
Repo for lattice QCD project for IST QCD course, 2018

## Requirements:
- C++14
- [JSON](https://github.com/nlohmann/json)
- [OpenBLAS](https://github.com/xianyi/OpenBLAS)
- [LAPACK](https://github.com/Reference-LAPACK/lapack-release)
- [XTL](https://github.com/QuantStack/xtl)
- [XTensor](https://github.com/QuantStack/xtensor)
- [XTensor-BLAS](https://github.com/QuantStack/xtensor-blas)

## How to run:
1. Ensure depandancies installed and pats set
1. Build with `make` in top directory
1. Run using `./bin/main.exe`, `-d [deubug point]` can be used to exit once program reaches specified point (requires debug point in code), `-v [verbose point]` can be used to get verbose output of specifed function (must be set in code beforehand).

### Debug points
- load - exits after loading first SU(3) matrix from lattice config

### Verbosity points
- [anything] - prints progress of main program
- load - print progress of lattice config loading
- movePoint - prints calculation details of moving withing the lattice, e.g. to check boundary conditions
- calcPlaquette - prints values of link variables during plaquette computation and plaquette variable and trace
- calcMeanPlaquette - prints values of possible plaquttes at given point, and their various means

## Status
- Can compute mean of all plaquettes across lattice
- ~~Can compute means of spatial:spatial, spatial:temporal, and all plaquettes at given point~~
- ~~Code in place to compute plaquettes, but calculations are incorrect~~
- ~~Converted reader to class~~
- ~~Reads in entire configuration~~
- ~~Checks that SU(3) matrices really are unitary~~
- ~~Reads in first SU(3) matrix~~

## To do
- ~~Correct plaquette computation~~
- ~~Calculate mean of plaquettes at given point~~
- ~~Calculate plaquettes~~
- ~~Periodic boundary conditions on plaquette computation~~