#pragma once

#include <stddef.h>  // for size_t

#include <array>        // for array
#include <cassert>      // for assert
#include <gsl/span>     // for span
#include <memory>       // for unique_ptr, make_unique
#include <type_traits>  // for move, remove_reference<>::type
#include <variant>      // for visit, variant
#include <vector>       // for vector
// #include <xtensor/xarray.hpp>  // for xtensor, xarray

#include <ldsgen/lds.hpp>  // for VdCorput, Sphere

namespace lds2 {
    // using Arr = xt::xarray<double, xt::layout_type::row_major>;
    using gsl::span;
    using ldsgen::Circle;
    using ldsgen::VdCorput;
    using std::array;
    using std::vector;

    class CylindN;

    using CylindVariant = std::variant<std::unique_ptr<Circle>, std::unique_ptr<CylindN>>;

    /** Generate using cylindrical coordinate method */
    class CylindN {
      private:
        VdCorput vdc;
        CylindVariant c_gen;

      public:
        explicit CylindN(span<const size_t> base) : vdc{base[0]} {
            const auto n = base.size();
            assert(n >= 2);
            if (n == 2) {
                this->c_gen = std::make_unique<Circle>(base[1]);
            } else {
                this->c_gen = std::make_unique<CylindN>(base.last(n - 1));
            }
        }

        auto pop() -> vector<double>;

        auto reseed(size_t seed) -> void;
    };

}  // namespace lds2
