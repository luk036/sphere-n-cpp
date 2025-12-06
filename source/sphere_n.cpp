#include <stddef.h>  // for size_t

#include <algorithm>
#include <cassert>         // for assert
#include <cmath>           // for cos, sin, sqrt
#include <ldsgen/lds.hpp>  // for vdcorput, sphere
#include <memory>          // for unique_ptr, make_unique
#include <mutex>
#include <span>                   // for span
#include <sphere_n/sphere_n.hpp>  // for sphere_n, cylin_n, cylin_2
#include <unordered_map>          // for unordered_map
#include <variant>                // for visit, variant
#include <vector>                 // for vector

// Global variables (equivalent to Python's module-level variables)
static constexpr double PI = 3.14159265358979323846;
static constexpr double HALF_PI = PI / 2.0;

// Cache for memoization
std::unordered_map<size_t, std::vector<double>> cacheOdd;
std::unordered_map<size_t, std::vector<double>> cacheEven;

class Globals {
  private:
    std::vector<double> X;
    std::vector<double> F2;
    std::vector<double> NEG_COSINE;
    std::vector<double> SINE;
    std::mutex cacheMutex;
    std::unordered_map<size_t, std::vector<double>> cacheOdd;
    std::unordered_map<size_t, std::vector<double>> cacheEven;

    public:
    const std::vector<double>& getX() const { return X; }
    const std::vector<double>& getF2() const { return F2; }
    const std::vector<double> &getTp(size_t n);

    // Initialize global vectors (akin to initializing numpy arrays)
    Globals() : X(lds2::N_POINTS), F2(lds2::N_POINTS), NEG_COSINE(lds2::N_POINTS), SINE(lds2::N_POINTS) {
                for (auto i = 0U; i < lds2::N_POINTS; ++i) {
            double x = i * PI / double(lds2::N_POINTS - 1);
                        X[i] = x;
                        NEG_COSINE[i] = -std::cos(x);
                        SINE[i] = std::sin(x);
                        F2[i] = (x + NEG_COSINE[i] * SINE[i]) / 2.0;
        }
    }

    const std::vector<double> &getTpOdd(size_t n) {
        // std::lock_guard<std::mutex> lock(this->cacheMutex);
        auto &cache = ::cacheOdd;
        if (cache.find(n) != cache.end()) return cache[n];

        std::vector<double> result;
        if (n == 1) {
                        result = NEG_COSINE;
        } else {
                        std::vector<double> tpMinus2 = getTpOdd(n - 2);
                        result.resize(lds2::N_POINTS);
                        for (auto i = 0U; i < lds2::N_POINTS; ++i) {
                                result[i] = (double(n - 1) * tpMinus2[i]
                             + NEG_COSINE[i] * std::pow(SINE[i], n - 1))
                            / double(n);
            }
        }
        cache[n] = result;
        return cache[n];
    }

    const std::vector<double> &getTpEven(size_t n) {
        // std::lock_guard<std::mutex> lock(this->cacheMutex);
        auto &cache = ::cacheEven;
        if (cache.find(n) != cache.end()) return cache[n];

        std::vector<double> result;
        if (n == 0) {
                        result = X;
        } else {
            std::vector<double> tpMinus2 = this->getTpEven(n - 2);
                        result.resize(lds2::N_POINTS);
                        for (auto i = 0U; i < lds2::N_POINTS; ++i) {
                                result[i] = (double(n - 1) * tpMinus2[i]
                             + NEG_COSINE[i] * std::pow(SINE[i], n - 1))
                            / double(n);
            }
        }
        cache[n] = result;
        return cache[n];
    }


};

    const std::vector<double> &Globals::getTp(size_t n) {
        std::lock_guard<std::mutex> lock(this->cacheMutex);
        return (n % 2 == 0) ? this->getTpEven(n) : this->getTpOdd(n);
    }

static Globals GL{};

// Helper function to mimic Rust's interp
static double interp(const std::vector<double> &x, const std::vector<double> &X, double val) {
    // A simple linear interpolation for demonstration purposes
    auto pos = std::upper_bound(X.begin(), X.end(), val) - X.begin();
    auto len = std::distance(X.begin(), X.end());
    if (pos == 0) return x[0];
    if (pos == len) return x.back();
    auto spos = size_t(pos);
    double fraction = (val - X[spos - 1]) / (X[spos] - X[spos - 1]);
    return x[spos - 1] + fraction * (x[spos] - x[spos - 1]);
}

namespace lds2 {
    using std::array;
    using std::cos;
    using std::sin;
    using std::span;
    using std::sqrt;
    using std::vector;

    /**
     * Sphere3 constructor - creates a 3-sphere generator
     *
     * ```svgbob
     *   base[0] -> VdCorput
     *              |
     *   base[1], base[2] -> Sphere (for 2-sphere)
     *   Result: 3-sphere point [x, y, z, w]
     * ```
     */
    Sphere3::Sphere3(span<const size_t> base) : vdc{base[0]}, sphere2{base[1], base[2]} {}

    /**
     * @brief Generate next point on 3-sphere
     *
     * @return array<double, 4>
     *
     * ```svgbob
     *   VdCorput -> ti -> xi -> [sin(xi)*s0, sin(xi)*s1, sin(xi)*s2, cos(xi)]
     *                |
     *   Sphere2 -----> [s0, s1, s2]
     *   Result: [x, y, z, w] on 3-sphere
     * ```
     */
    auto Sphere3::pop() -> array<double, 4> {
        const auto ti = HALF_PI * this->vdc.pop();  // map to [0, pi/2];
        // const auto &tp = GL.getTp(2);
        // const auto xi = ::interp(GL.X, tp, ti);
                const auto xi = ::interp(GL.getX(), GL.getF2(), ti);
        const auto cosxi = cos(xi);
        const auto sinxi = sin(xi);
        const auto [s0, s1, s2] = this->sphere2.pop();
        return {sinxi * s0, sinxi * s1, sinxi * s2, cosxi};
    }

    /**
     * SphereN constructor - creates an n-sphere generator using recursion
     *
     * ```svgbob
     *   base[0] -> VdCorput
     *              |
     *   base[1..n] -> SphereGen (Sphere3 for n=4, recursive SphereN for n>4)
     *   Result: n-sphere point [x1, x2, ..., xn+1]
     * ```
     */
    SphereN::SphereN(std::span<const size_t> base) : vdc{base[0]} {
        const auto m = base.size();
        assert(m >= 4);
        // Arr tp_minus2;
        if (m == 4) {
            this->s_gen = std::make_unique<Sphere3>(base.subspan(1, 3));
        } else {
            this->s_gen = std::make_unique<SphereN>(base.last(m - 1));
        }
        this->n = m - 1;
        // this->tp = ((n - 1.0) * tp_minus2 + NEG_COSINE * xt::pow(SINE, n - 1.0))
        // / n;
    }

    /**
     * Generate next point on n-sphere using recursive approach
     *
     * ```svgbob
     *   VdCorput -> vd -> ti -> xi -> [sin(xi)*lower_dim, cos(xi)]
     *                |              /
     *   LowerDim --->+-------------+
     *   Result: [x1, x2, ..., xn, xn+1] on n-sphere
     * ```
     */
    auto SphereN::pop() -> vector<double> {
        const auto vd = this->vdc.pop();
        const auto &tp = GL.getTp(this->n);
        const auto ti = tp[0] + (tp[tp.size() - 1] - tp[0]) * vd;  // map to [t0, tm-1];
        const auto xi = ::interp(GL.getX(), tp, ti);
        const auto sinphi = sin(xi);

        auto res = std::visit([](auto &t) {
            using T = decltype(t->pop());
            if constexpr (std::is_same_v<T, std::vector<double>>) {
                return t->pop();
            } else {
                return to_vector(t->pop());
            }
        }, this->s_gen);

        for (auto &elem : res) {
            elem *= sinphi;
        }
        res.emplace_back(cos(xi));
        return res;
    }
}  // namespace lds2
