#pragma once

/** @file cylind_n.hpp
 *  @brief Cylindrical-coordinate generator (CylindN) for N-dimensional spheres.
 */

#include <memory>
#include <mutex>
#include <span>
#include <vector>

#include <ldsgen/lds.hpp>  // for Circle, VdCorput

namespace ldsgen {

    /**
     * @brief Base class for cylindrical-coordinate generators
     *
     * Provides the common interface for all cylindrical sequence generators.
     * Mirrors ldsgen::SphereGen so cylind_n stays independent of sphere_n.hpp.
     */
    class CylindGen {
      public:
        CylindGen() = default;
        CylindGen(const CylindGen&) = default;
        CylindGen(CylindGen&&) noexcept = default;
        CylindGen& operator=(const CylindGen&) = default;
        CylindGen& operator=(CylindGen&&) noexcept = default;
        virtual ~CylindGen() = default;
        virtual std::vector<double> pop() = 0;
        virtual void reseed(unsigned long seed) = 0;
    };

    /**
     * @brief Wrapper class to make Circle compatible with the CylindGen interface.
     */
    class CircleWrapper : public CylindGen {
      public:
        explicit CircleWrapper(unsigned long base);
        std::vector<double> pop() override;
        void reseed(unsigned long seed) override;

      private:
        Circle circle_;
        mutable std::mutex mutex_;
    };

    /**
     * @brief N-dimensional sphere sequence generator using cylindrical coordinates.
     *
     * Recursively builds a point on S^(n-1) as P = (sqrt(1 - z^2) * P_{n-1}, z), where
     * z = 2*VdC - 1 is in [-1, 1] and P_{n-1} comes from the lower-dimensional generator,
     * with a Circle generator as the 2D base case. Thread-safe (internal mutex).
     */
    class CylindN : public CylindGen {
      public:
        explicit CylindN(std::span<const unsigned long> base);
        std::vector<double> pop() override;
        void reseed(unsigned long seed) override;

      private:
        VdCorput vdc_;
        std::unique_ptr<CylindGen> c_gen_;
        unsigned int n_;
        mutable std::mutex mutex_;
    };
}  // namespace ldsgen
