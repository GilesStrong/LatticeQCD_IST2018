#ifndef MISC_HH_
#define MISC_HH_

typedef xt::xtensor_fixed<std::complex<double>, xt::xshape<3, 3>> su3Matrix;
typedef xt::xtensor_fixed<su3Matrix, xt::xshape<4>> direction;
typedef xt::xtensor<direction, 4> lattice;

#endif /* MISC_HH_ */