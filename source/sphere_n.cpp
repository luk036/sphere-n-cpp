#include <stddef.h>  // for size_t

#include <algorithm>
#include <cassert>         // for assert
#include <cmath>           // for cos, sin, sqrt
#include <gsl/span>        // for span
#include <ldsgen/lds.hpp>  // for vdcorput, sphere
#include <memory>          // for unique_ptr, make_unique
// #include <mutex>
#include <sphere_n/sphere_n.hpp>  // for sphere_n, cylin_n, cylin_2
#include <unordered_map>          // for unordered_map
#include <variant>                // for visit, variant
#include <vector>                 // for vector

// Global variables (equivalent to Python's module-level variables)
static const double PI = 3.14159265358979323846;
static const double HALF_PI = PI / 2.0;

class Globals {
  public:
    const size_t N_POINTS = 300;
    std::vector<double> X;

  private:
    std::vector<double> NEG_COSINE;
    std::vector<double> SINE;
    // Cache for memoization
    std::unordered_map<int, std::vector<double>> cacheOdd;
    std::unordered_map<int, std::vector<double>> cacheEven;
    // std::mutex cacheMutex;

  public:
    // Initialize global vectors (akin to initializing numpy arrays)
    Globals() : X(N_POINTS), NEG_COSINE(N_POINTS), SINE(N_POINTS) {
        for (auto i = 0U; i < this->N_POINTS; ++i) {
            double x = i * PI / (this->N_POINTS - 1);
            this->X[i] = x;
            this->NEG_COSINE[i] = -std::cos(x);
            this->SINE[i] = std::sin(x);
        }
    }

    const std::vector<double> &getTpOdd(int n) {
        // std::lock_guard<std::mutex> lock(this->cacheMutex);
        auto &cache = this->cacheOdd;
        if (cache.find(n) != cache.end()) return cache[n];

        std::vector<double> result;
        if (n == 1) {
            result = this->NEG_COSINE;
        } else {
            std::vector<double> tpMinus2 = this->getTpOdd(n - 2);
            result.resize(this->N_POINTS);
            for (auto i = 0U; i < this->N_POINTS; ++i) {
                result[i]
                    = ((n - 1) * tpMinus2[i] + this->NEG_COSINE[i] * std::pow(this->SINE[i], n - 1))
                      / n;
            }
        }
        cache[n] = result;
        return cache[n];
    }

    const std::vector<double> &getTpEven(int n) {
        // std::lock_guard<std::mutex> lock(this->cacheMutex);
        auto &cache = this->cacheEven;
        if (cache.find(n) != cache.end()) return cache[n];

        std::vector<double> result;
        if (n == 0) {
            result = this->X;
        } else {
            std::vector<double> tpMinus2 = this->getTpEven(n - 2);
            result.resize(this->N_POINTS);
            for (auto i = 0U; i < this->N_POINTS; ++i) {
                result[i]
                    = ((n - 1) * tpMinus2[i] + this->NEG_COSINE[i] * std::pow(this->SINE[i], n - 1))
                      / n;
            }
        }
        cache[n] = result;
        return cache[n];
    }

    inline const std::vector<double> &getTp(int n) {
        if (n % 2 == 0) {
            return this->getTpEven(n);
        } else {
            return this->getTpOdd(n);
        }
    }
};

static Globals GL{};

// Helper function to mimic Rust's interp
static double interp(const std::vector<double> &x, const std::vector<double> &X, double val) {
    // A simple linear interpolation for demonstration purposes
    auto pos = std::upper_bound(X.begin(), X.end(), val) - X.begin();
    auto len = std::distance(X.begin(), X.end());
    if (pos == 0) return x[0];
    if (pos == len) return x.back();
    double fraction = (val - X[pos - 1]) / (X[pos] - X[pos - 1]);
    return x[pos - 1] + fraction * (x[pos] - x[pos - 1]);
}

namespace lds2 {
    using gsl::span;
    using std::array;
    using std::cos;
    using std::sin;
    using std::sqrt;
    using std::vector;

    Sphere3::Sphere3(span<const size_t> base) : vdc{base[0]}, sphere2{base[1], base[2]} {}

    /**
     * @brief
     *
     * @return array<double, 4>
     */
    auto Sphere3::pop() -> array<double, 4> {
        const auto ti = HALF_PI * this->vdc.pop();  // map to [0, pi/2];
        const auto &tp = GL.getTp(3);
        const auto xi = ::interp(GL.X, tp, ti);
        const auto cosxi = cos(xi);
        const auto sinxi = sin(xi);
        const auto [s0, s1, s2] = this->sphere2.pop();
        return {sinxi * s0, sinxi * s1, sinxi * s2, cosxi};
    }

    SphereN::SphereN(gsl::span<const size_t> base) : vdc{base[0]} {
        const auto m = base.size();
        assert(m >= 4);
        // Arr tp_minus2;
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
        const auto &tp = GL.getTp(this->n);
        const auto ti = tp[0] + (tp[tp.size() - 1] - tp[0]) * vd;  // map to [t0, tm-1];
        const auto xi = ::interp(GL.X, tp, ti);
        const auto sinphi = sin(xi);
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
        res.emplace_back(cos(xi));
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
