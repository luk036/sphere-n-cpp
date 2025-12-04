#include <cmath>                  // for cos, sin, sqrt
#include <ldsgen/lds.hpp>         // for vdcorput, sphere
#include <sphere_n/cylind_n.hpp>  // for sphere_n, cylin_n, cylin_2
#include <vector>                 // for vector

/**
 * @brief lds2 namespace for low discrepancy sequence generation
 * 
 * This namespace contains classes and functions for generating low discrepancy
 * sequences using cylindrical coordinate methods.
 */
namespace lds2 {
    using std::array;
    using std::cos;
    using std::sin;
    using std::sqrt;
    using std::vector;

    /**
     * @brief Generate the next point using cylindrical coordinate method
     * 
     * Generates a uniformly distributed point using cylindrical coordinates.
     * This method uses the Van der Corput sequence to generate the z-coordinate
     * and recursively generates the base dimensions, then combines them using
     * cylindrical coordinate transformation.
     * 
     * @return std::vector<double> An (n+1)-dimensional point [x1, x2, ..., xn, z]
     * 
     * The algorithm:
     * 1. Generate Van der Corput value and map to [-1, 1] for cos(phi)
     * 2. Calculate sin(phi) = sqrt(1 - cos²(phi))
     * 3. Recursively generate base dimensions
     * 4. Transform: [sin(phi)*base_dimensions, cos(phi)]
     */
    auto CylindN::pop() -> vector<double> {
        const auto cosphi = 2.0 * this->vdc.pop() - 1.0;  // map to [-1, 1];
        const auto sinphi = sqrt(1.0 - cosphi * cosphi);
        auto res = std::visit(
            [](auto &t) {
                using T = std::decay_t<decltype(*t)>;
                if constexpr (std::is_same_v<T, Circle>) {
                    auto arr = t->pop();
                    return vector<double>(arr.begin(), arr.end());
                } else if constexpr (std::is_same_v<T, CylindN>) {
                    return t->pop();
                } else {
                    return vector<double>{};
                }
            },
            this->c_gen);
        for (auto &xi : res) {
            xi *= sinphi;
        }
        res.push_back(cosphi);
        return res;
    }

    /**
     * @brief Reset the sequence generator to a specific seed
     * 
     * Resets both the Van der Corput sequence and the recursive cylindrical
     * generator to the specified seed value, allowing reproducible
     * sequence generation.
     * 
     * @param seed The seed value to reset to
     */
    auto CylindN::reseed(size_t seed) -> void {
        this->vdc.reseed(seed);
        std::visit([seed](auto &t) { t->reseed(seed); }, this->c_gen);
    }

}  // namespace lds2
