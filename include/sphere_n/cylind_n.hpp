#pragma once

#include <stddef.h>  // for size_t

#include <array>        // for array
#include <cassert>      // for assert
#include <memory>       // for unique_ptr, make_unique
#include <span>         // for span
#include <type_traits>  // for move, remove_reference<>::type
#include <variant>      // for visit, variant
#include <vector>       // for vector
// #include <xtensor/xarray.hpp>  // for xtensor, xarray

#include <ldsgen/lds.hpp>  // for VdCorput, Sphere

namespace lds2 {
    // using Arr = xt::xarray<double, xt::layout_type::row_major>;
    using ldsgen::Circle;
    using ldsgen::VdCorput;
    using std::array;
    using std::span;
    using std::vector;

    class CylindN;

    /**
     * @brief Variant type for cylindrical generator dispatching
     *
     * A std::variant that holds either a Circle (2D base case) or a CylindN
     * (recursive case). Used for runtime polymorphism in the recursive
     * cylindrical coordinate generation algorithm.
     */
    using CylindVariant = std::variant<std::unique_ptr<Circle>, std::unique_ptr<CylindN>>;

    /**
     * Generate using cylindrical coordinate method
     *
     * @verbatim
     *     z
     *     ^
     *     |
     *     |    . P(r,phi,z)
     *     |  .
     *     |.
     *     +---------> y
     *    /|
     *   / |
     *  /  |
     * x   |
     *     |
     *     v rho
     * @endverbatim
     */
    class CylindN {
      private:
        VdCorput vdc;
        CylindVariant c_gen;

      public:
        /**
         * @brief Construct a new CylindN object
         *
         * The `CylindN(span<const size_t> base)` is a constructor for
         * the `CylindN` class. It takes one parameter `base`, which is
         * used as the bases for generating the cylindrical sequence. The `explicit`
         * keyword indicates that this constructor can only be used for explicit
         * construction and not for implicit conversions.
         *
         * @param[in] base Span containing base numbers for sequence generation
         *
         * @verbatim
         *   Base: [b0, b1, b2, ..., bn]
         *         |   |   |        |
         *         v   v   v        v
         *   VdCorput CylindGen (recursive)
         *      |      |
         *      +------+
         *       cylindrical point
         * @endverbatim
         */
        explicit CylindN(span<const size_t> base) : vdc{base[0]} {
            const auto n = base.size();
            assert(n >= 2);
            if (n == 2) {
                this->c_gen = std::make_unique<Circle>(base[1]);
            } else {
                this->c_gen = std::make_unique<CylindN>(base.last(n - 1));
            }
        }

        /**
         * @brief Generate the next point using cylindrical coordinate method
         *
         * Generates a uniformly distributed point using cylindrical coordinates.
         * This method uses the Van der Corput sequence to generate the z-coordinate
         * and recursively generates the base dimensions, then combines them using
         * cylindrical coordinate transformation.
         *
         * @return vector<double> An (n+1)-dimensional point [x1, x2, ..., xn, z]
         *
         * @verbatim
         *   Sequence: v0, v1, v2, ...
         *              |
         *              v
         *   VdCorput -> CylindN -> [x1, x2, ..., xn, z]
         *      |      |
         *      v      v
         *   cos(phi)  sin(phi)
         * @endverbatim
         */
        auto pop() -> vector<double>;

        /**
         * @brief reseed
         *
         * The `reseed(size_t seed)` function is used to reset the state of the
         * sequence generator to a specific seed value. This allows the sequence
         * generator to start generating the sequence from the beginning, or from a
         * specific point in the sequence, depending on the value of the seed.
         *
         * @param[in] seed The seed value to reset to
         */
        auto reseed(size_t seed) -> void;
    };

}  // namespace lds2
