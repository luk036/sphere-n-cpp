#include <stddef.h>  // for size_t

#include <cassert>  // for assert
#include <cmath>    // for cos, sin, sqrt
#include <cmath>
#include <gsl/span>  // for span
#include <iostream>
#include <ldsgen/lds.hpp>  // for vdcorput, sphere
#include <memory>          // for unique_ptr, make_unique
#include <mutex>
#include <sphere_n/sphere_n.hpp>  // for sphere_n, cylin_n, cylin_2
#include <tuple>                  // for tuple
#include <type_traits>            // for move, remove_reference<>::type
#include <unordered_map>          // for unordered_map
#include <unordered_map>
#include <variant>  // for visit, variant
#include <vector>   // for vector
#include <vector>
#include <xtensor/xaccessible.hpp>  // for xconst_accessible
#include <xtensor/xarray.hpp>       // for xtensor, xarray
#include <xtensor/xbuilder.hpp>     // for linspace
#include <xtensor/xfunction.hpp>    // for xfunction
#include <xtensor/xgenerator.hpp>   // for xgenerator
#include <xtensor/xiterator.hpp>    // for linear_begin
#include <xtensor/xlayout.hpp>      // for layout_type, layout_type::row...
#include <xtensor/xmath.hpp>        // for cos, interp, pow, sin, numeri...
#include <xtensor/xoperation.hpp>   // for xfunction_type_t, operator*
#include <xtensor/xtensor.hpp>      // for xtensor_container

// Global variables (equivalent to Python's module-level variables)
const double PI = 3.14159265358979323846;
const int N_POINTS = 300;
std::vector<double> X(N_POINTS);
std::vector<double> NEG_COSINE(N_POINTS);
std::vector<double> SINE(N_POINTS);

// Initialize global vectors (akin to initializing numpy arrays)
void initializeGlobals() {
    for (int i = 0; i < N_POINTS; ++i) {
        double x = i * PI / (N_POINTS - 1);
        X[i] = x;
        NEG_COSINE[i] = -std::cos(x);
        SINE[i] = std::sin(x);
    }
}

// Cache for memoization
std::unordered_map<int, std::vector<double>> cacheOdd;
std::unordered_map<int, std::vector<double>> cacheEven;
std::mutex cacheMutex;

const std::vector<double> &getTpOdd(int n) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    auto &cache = cacheOdd;
    if (cache.find(n) != cache.end()) return cache[n];

    std::vector<double> result;
    if (n == 1) {
        result = NEG_COSINE;
    } else {
        std::vector<double> tpMinus2 = getTpOdd(n - 2);
        result.resize(N_POINTS);
        for (int i = 0; i < N_POINTS; ++i) {
            result[i] = ((n - 1) * tpMinus2[i] + NEG_COSINE[i] * std::pow(SINE[i], n - 1)) / n;
        }
    }
    cache[n] = result;
    return cache[n];
}

const std::vector<double> &getTpEven(int n) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    auto &cache = cacheEven;
    if (cache.find(n) != cache.end()) return cache[n];

    std::vector<double> result;
    if (n == 0) {
        result = X;
    } else {
        std::vector<double> tpMinus2 = getTpEven(n - 2);
        result.resize(N_POINTS);
        for (int i = 0; i < N_POINTS; ++i) {
            result[i] = ((n - 1) * tpMinus2[i] + NEG_COSINE[i] * std::pow(SINE[i], n - 1)) / n;
        }
    }
    cache[n] = result;
    return cache[n];
}

const std::vector<double> &getTp(int n) {
    if (n % 2 == 0) {
        return getTpEven(n);
    } else {
        return getTpOdd(n);
    }
}

// int main() {
//     initializeGlobals();
//     std::cout << "Result for n=3: ";
//     for (double val : getTp(3)) {
//         std::cout << val << " ";
//     }
//     std::cout << std::endl;
//     return 0;
// }

namespace lds2 {
    using gsl::span;
    using std::array;
    using std::cos;
    using std::sin;
    using std::sqrt;
    using std::unordered_map;
    using std::vector;

    using Arr = xt::xarray<double, xt::layout_type::row_major>;
    static const double PI = xt::numeric_constants<double>::PI;
    static const double HALF_PI = PI / 2.0;
    static const Arr X = xt::linspace(0.0, PI, 300);
    static const Arr NEG_COSINE = -xt::cos(X);
    static const Arr SINE = xt::sin(X);

    static auto get_tp(size_t n) -> const Arr & {
        static auto cache = unordered_map<size_t, Arr>{{0, X}, {1, NEG_COSINE}};
        if (cache.find(n) != cache.end()) {
            return cache[n];
        }
        const auto &tp_minus2 = get_tp(n - 2);
        const auto n1 = double(n) - 1.0;
        cache[n] = (n1 * tp_minus2 + NEG_COSINE * xt::pow(SINE, n1)) / double(n);
        return cache[n];
    }

    Sphere3::Sphere3(span<const size_t> base) : vdc{base[0]}, sphere2{base[1], base[2]} {}

    /**
     * @brief
     *
     * @return array<double, 4>
     */
    auto Sphere3::pop() -> array<double, 4> {
        const auto ti = HALF_PI * this->vdc.pop();  // map to [0, pi/2];
        const auto &tp = get_tp(3);
        const auto xi = xt::interp(xt::xtensor<double, 1>{ti}, tp, X);
        const auto cosxi = cos(xi[0]);
        const auto sinxi = sin(xi[0]);
        const auto [s0, s1, s2] = this->sphere2.pop();
        return {sinxi * s0, sinxi * s1, sinxi * s2, cosxi};
    }

    SphereN::SphereN(gsl::span<const size_t> base) : vdc{base[0]} {
        const auto m = base.size();
        assert(m >= 4);
        Arr tp_minus2;
        if (m == 4) {
            // tp_minus2 = NEG_COSINE;
            // this->s_gen = std::make_unique<Sphere3>(base.subspan(1, 3));
            this->s_gen = std::make_unique<Sphere3>(base.subspan(1, 3));
        } else {
            auto s_minus1 = std::make_unique<SphereN>(base.last(m - 1));
            // tp_minus2 = s_minus1->get_tp_minus1();
            this->s_gen = std::move(s_minus1);
        }
        this->n = m - 1;
        // this->tp = ((n - 1.0) * tp_minus2 + NEG_COSINE * xt::pow(SINE, n - 1.0))
        // / n;
    }

    auto SphereN::pop() -> vector<double> {
        const auto vd = this->vdc.pop();
        const auto &tp = get_tp(this->n);
        const auto ti = tp[0] + (tp[tp.size() - 1] - tp[0]) * vd;  // map to [t0, tm-1];
        const auto xi = xt::interp(xt::xtensor<double, 1>{ti}, tp, X);
        const auto sinphi = sin(xi[0]);
        auto res = std::visit(
            [](auto &t) {
                using T = std::decay_t<decltype(*t)>;
                if constexpr (std::is_same_v<T, Sphere3>) {
                    auto arr = t->pop();
                    return vector<double>(arr.begin(), arr.end());
                } else if constexpr (std::is_same_v<T, SphereN>) {
                    return t->pop();
                } else {
                    return vector<double>{};
                }
            },
            this->s_gen);
        for (auto &elem : res) {
            elem *= sinphi;
        }
        res.emplace_back(cos(xi[0]));
        return res;
    }

    auto CylinN::pop() -> vector<double> {
        const auto cosphi = 2.0 * this->vdc.pop() - 1.0;  // map to [-1, 1];
        const auto sinphi = sqrt(1.0 - cosphi * cosphi);
        auto res = std::visit(
            [](auto &t) {
                using T = std::decay_t<decltype(*t)>;
                if constexpr (std::is_same_v<T, Circle>) {
                    auto arr = t->pop();
                    return vector<double>(arr.begin(), arr.end());
                } else if constexpr (std::is_same_v<T, CylinN>) {
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
