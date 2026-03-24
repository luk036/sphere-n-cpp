#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace ldsgen {
    /**
     * @brief Van der Corput sequence generator
     *
     * The `VdCorput` class generates a low-discrepancy Van der Corput sequence,
     * which is a sequence of points in the interval [0, 1) that are
     * uniformly distributed. This sequence is commonly used in quasi-Monte Carlo
     * methods and is defined by the radical inverse function with a given base.
     *
     * @see https://en.wikipedia.org/wiki/Van_der_Corput_sequence
     */
    class VdCorput {
      public:
        /**
         * @brief Construct a new VdCorput generator
         * @param base The base for the radical inverse function (must be >= 2)
         */
        VdCorput(size_t base) {}

        /**
         * @brief Generate the next value in the Van der Corput sequence
         * @return double The next value in the sequence [0, 1)
         */
        double pop() { return 0.5; }  // Mock implementation

        /**
         * @brief Reset the generator to a specific seed
         * @param seed The seed value for reproducible sequence generation
         */
        void reseed(size_t seed) {}
    };

    /**
     * @brief Unit sphere (2-sphere) point generator
     *
     * The `Sphere` class generates uniformly distributed points on the surface
     * of a unit sphere (3D ball surface) using the Van der Corput sequence.
     * Each point is represented as a 3-dimensional array [x, y, z] where
     * x² + y² + z² = 1.
     *
     * ```
     *        z
     *        ^
     *        |    . P(x,y,z)
     *        |  .
     *        |.
     *        +---------> y
     *       /
     *      /
     *     x
     * ```
     */
    class Sphere {
      public:
        /**
         * @brief Construct a new Sphere generator
         * @param base1 Base for the first Van der Corput sequence
         * @param base2 Base for the second Van der Corput sequence
         */
        Sphere(size_t base1, size_t base2) {}

        /**
         * @brief Generate the next point on the unit sphere
         * @return std::array<double, 3> A point [x, y, z] on the sphere surface
         */
        std::array<double, 3> pop() { return {0.0, 0.0, 0.0}; }  // Mock implementation

        /**
         * @brief Reset the generator to a specific seed
         * @param seed The seed value for reproducible sequence generation
         */
        void reseed(size_t seed) {}
    };

    /**
     * @brief Unit circle point generator
     *
     * The `Circle` class generates uniformly distributed points on the circumference
     * of a unit circle using the Van der Corput sequence. Each point is represented
     * as a 2-dimensional array [x, y] where x² + y² = 1.
     *
     * ```
     *        y
     *        ^
     *        |   . P(x,y)
     *        |  .
     *        | .
     *        |.
     *        +-------------> x
     * ```
     */
    class Circle {
      public:
        /**
         * @brief Construct a new Circle generator
         * @param base The base for the Van der Corput sequence
         */
        Circle(size_t base) {}

        /**
         * @brief Generate the next point on the unit circle
         * @return std::array<double, 2> A point [x, y] on the circle
         */
        std::array<double, 2> pop() { return {0.0, 0.0}; }  // Mock implementation

        /**
         * @brief Reset the generator to a specific seed
         * @param seed The seed value for reproducible sequence generation
         */
        void reseed(size_t seed) {}
    };
}