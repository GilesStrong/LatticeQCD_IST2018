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

## Status
- Converted reader to class
- ~~Reads in entire configuration~~
- ~~Checks that SU(3) matrices really are unitary~~
- ~~Reads in first SU(3) matrix~~

## To do
- Calculate mean of plaquettes at given point
- Calculate plaquettes
- ~~Periodic boundary conditions on plaquette computation~~