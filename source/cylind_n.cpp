#include <cmath>                  // for cos, sin, sqrt
#include <ldsgen/lds.hpp>         // for vdcorput, sphere
#include <sphere_n/cylind_n.hpp>  // for sphere_n, cylin_n, cylin_2
#include <vector>                 // for vector

namespace lds2 {
    using std::array;
    using std::cos;
    using std::sin;
    using std::sqrt;
    using std::vector;

    /**
     * Generate next point using cylindrical coordinates
     *
     * ```svgbob
     *   VdCorput -> cosphi -> [sinphi*base_dim, cosphi]
     *                |              /
     *   BaseDim ----->+-------------+ 
     *   Result: [x1, x2, ..., xn, z] using cylindrical method
     * ```
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

}  // namespace lds2
