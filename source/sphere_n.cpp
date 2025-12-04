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

// Mathematical constants
/** @brief π constant with high precision */
static constexpr double PI = 3.14159265358979323846;
/** @brief π/2 constant for angle calculations */
static constexpr double HALF_PI = PI / 2.0;

// Global cache for memoization (module-level variables)
/** @brief Cache for storing Tp values of odd dimensions */
std::unordered_map<size_t, std::vector<double>> cacheOdd;
/** @brief Cache for storing Tp values of even dimensions */
std::unordered_map<size_t, std::vector<double>> cacheEven;

/**
 * @brief Global singleton class for precomputed trigonometric values and caching
 * 
 * This class manages precomputed lookup tables for trigonometric functions
 * and cached values for sphere generation. It uses memoization to avoid
 * recalculating expensive trigonometric operations.
 */
class Globals {
  private:
    std::vector<double> X;           ///< Precomputed x values: i * π / (N_POINTS - 1)
    std::vector<double> F2;          ///< Precomputed F2 values for n=2 case
    std::vector<double> NEG_COSINE;  ///< Precomputed -cos(x) values
    std::vector<double> SINE;        ///< Precomputed sin(x) values
    std::mutex cacheMutex;           ///< Mutex for thread-safe cache access
    std::unordered_map<size_t, std::vector<double>> cacheOdd;  ///< Cache for odd n values
    std::unordered_map<size_t, std::vector<double>> cacheEven; ///< Cache for even n values

    public:
    /**
     * @brief Get precomputed X values
     * @return const std::vector<double>& X coordinate values
     */
    const std::vector<double>& getX() const { return X; }
    
    /**
     * @brief Get precomputed F2 values
     * @return const std::vector<double>& F2 values for n=2
     */
    const std::vector<double>& getF2() const { return F2; }
    
    /**
     * @brief Get Tp values for dimension n with caching
     * @param n Dimension parameter
     * @return const std::vector<double>& Tp values for given dimension
     */
    const std::vector<double> &getTp(size_t n);

    /**
     * @brief Initialize global vectors with trigonometric values
     * 
     * Precomputes arrays of trigonometric values for efficient lookup:
     * - X: linearly spaced values from 0 to π
     * - NEG_COSINE: -cos(x) for each x in X
     * - SINE: sin(x) for each x in X  
     * - F2: (x + (-cos(x)) * sin(x)) / 2.0 for n=2 case
     */
    Globals() : X(lds2::N_POINTS), F2(lds2::N_POINTS), NEG_COSINE(lds2::N_POINTS), SINE(lds2::N_POINTS) {
                for (auto i = 0U; i < lds2::N_POINTS; ++i) {
            double x = i * PI / double(lds2::N_POINTS - 1);
                        X[i] = x;
                        NEG_COSINE[i] = -std::cos(x);
                        SINE[i] = std::sin(x);
                        F2[i] = (x + NEG_COSINE[i] * SINE[i]) / 2.0;
        }
    }

    /**
     * @brief Get Tp values for odd dimensions with memoization
     * 
     * Computes Tp values for odd n using the recursive formula:
     * Tp(n) = ((n-1) * Tp(n-2) + (-cos(x)) * sin(x)^(n-1)) / n
     * 
     * @param n Odd dimension parameter
     * @return const std::vector<double>& Tp values for dimension n
     */
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

    /**
     * @brief Get Tp values for even dimensions with memoization
     * 
     * Computes Tp values for even n using the recursive formula:
     * Tp(n) = ((n-1) * Tp(n-2) + (-cos(x)) * sin(x)^(n-1)) / n
     * Base case: Tp(0) = X (linearly spaced values from 0 to π)
     * 
     * @param n Even dimension parameter
     * @return const std::vector<double>& Tp values for dimension n
     */
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

    /**
     * @brief Get Tp values for any dimension
     * 
     * Dispatches to getTpEven or getTpOdd based on the parity of n.
     * This is the main interface for accessing Tp lookup tables.
     * 
     * @param n Dimension parameter
     * @return const std::vector<double>& Tp values for dimension n
     */
    const std::vector<double> &Globals::getTp(size_t n) {
        std::lock_guard<std::mutex> lock(this->cacheMutex);
        return (n % 2 == 0) ? this->getTpEven(n) : this->getTpOdd(n);
    }

/** @brief Global singleton instance of the Globals class */
static Globals GL{};

/**
 * @brief Linear interpolation helper function
 * 
 * Performs linear interpolation between points in the lookup tables.
 * This mimics Rust's interp functionality for smooth value transitions.
 * 
 * @param x Y-values corresponding to the lookup table
 * @param X X-values (typically the precomputed angle values)
 * @param val The value to interpolate at
 * @return double Interpolated value
 */
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

/**
 * @brief lds2 namespace for low discrepancy sequence generation
 * 
 * This namespace contains classes and functions for generating low discrepancy
 * sequences on n-dimensional spheres using Van der Corput sequences and
 * spherical coordinate transformations.
 */
namespace lds2 {
    using std::array;
    using std::cos;
    using std::sin;
    using std::span;
    using std::sqrt;
    using std::vector;

    /**
     * @brief Construct a new Sphere3 object
     * 
     * Creates a 3-sphere generator using Van der Corput sequence for the first
     * dimension and a Sphere generator for the remaining 2 dimensions.
     * 
     * @param base Span containing base numbers for sequence generation
     *             - base[0]: base for VdCorput sequence
     *             - base[1], base[2]: bases for Sphere (2-sphere) generator
     * 
     * The generator creates points on the 3-sphere surface using the formula:
     * [sin(xi)*s0, sin(xi)*s1, sin(xi)*s2, cos(xi)]
     * where [s0, s1, s2] is a point on the 2-sphere and xi is interpolated.
     */
    Sphere3::Sphere3(span<const size_t> base) : vdc{base[0]}, sphere2{base[1], base[2]} {}

    /**
     * @brief Generate the next point on the 3-sphere
     * 
     * Generates a uniformly distributed point on the surface of a 3-sphere
     * using the Van der Corput sequence and spherical coordinate transformation.
     * 
     * @return std::array<double, 4> A 4-dimensional point [x, y, z, w] on the 3-sphere
     * 
     * The algorithm:
     * 1. Generate Van der Corput sequence value and map to [0, π/2]
     * 2. Interpolate to get xi angle using precomputed lookup tables
     * 3. Generate a 2-sphere point [s0, s1, s2]
     * 4. Transform to 3-sphere: [sin(xi)*s0, sin(xi)*s1, sin(xi)*s2, cos(xi)]
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
     * @brief Construct a new SphereN object
     * 
     * Creates an n-sphere generator using recursive decomposition. For n=4,
     * uses Sphere3 as the base case. For n>4, recursively creates SphereN
     * objects to handle the lower dimensions.
     * 
     * @param base Span containing base numbers for sequence generation
     *             - base[0]: base for VdCorput sequence (first dimension)
     *             - base[1..n]: bases for recursive sphere generator
     * 
     * The recursive structure allows generation of points on any n-sphere
     * by nesting lower-dimensional sphere generators.
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
     * @brief Generate the next point on the n-sphere
     * 
     * Generates a uniformly distributed point on the surface of an n-sphere
     * using recursive decomposition and spherical coordinate transformation.
     * 
     * @return std::vector<double> An (n+1)-dimensional point on the n-sphere
     * 
     * The algorithm:
     * 1. Generate Van der Corput sequence value for the first dimension
     * 2. Map to appropriate range using precomputed lookup tables (getTp)
     * 3. Interpolate to get xi angle
     * 4. Recursively generate lower-dimensional sphere point
     * 5. Transform: [sin(xi)*lower_dim_point, cos(xi)]
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

    /**
     * @brief Reset the sequence generator to a specific seed
     * 
     * Resets both the Van der Corput sequence and the recursive sphere
     * generator to the specified seed value, allowing reproducible
     * sequence generation.
     * 
     * @param seed The seed value to reset to
     */
    auto SphereN::reseed(size_t seed) -> void {
        this->vdc.reseed(seed);
        std::visit([seed](auto &t) { t->reseed(seed); }, this->s_gen);
    }
}  // namespace lds2
