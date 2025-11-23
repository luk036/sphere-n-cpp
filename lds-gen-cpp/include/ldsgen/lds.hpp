#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace ldsgen {
    // Minimal declarations to allow compilation
    class VdCorput {
    public:
        VdCorput(size_t base) {}
        double pop() { return 0.5; }  // Mock implementation
        void reseed(size_t seed) {}
    };

    class Sphere {
    public:
        Sphere(size_t base1, size_t base2) {}
        std::array<double, 3> pop() { return {0.0, 0.0, 0.0}; }  // Mock implementation
        void reseed(size_t seed) {}
    };

    class Circle {
    public:
        Circle(size_t base) {}
        std::array<double, 2> pop() { return {0.0, 0.0}; }  // Mock implementation
        void reseed(size_t seed) {}
    };
}