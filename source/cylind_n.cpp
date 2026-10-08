#include <cmath>
#include <memory>
#include <mutex>
#include <sphere_n/cylind_n.hpp>
#include <stdexcept>
#include <vector>

namespace ldsgen {

    // --- CircleWrapper ---

    CircleWrapper::CircleWrapper(unsigned long base) : circle_(base) {}

    std::vector<double> CircleWrapper::pop() {
        std::scoped_lock lock(mutex_);
        auto arr = circle_.pop();
        return {arr.begin(), arr.end()};
    }

    void CircleWrapper::reseed(unsigned long seed) {
        std::scoped_lock lock(mutex_);
        circle_.reseed(seed);
    }

    // --- CylindN ---

    CylindN::CylindN(std::span<const unsigned long> base)
        : vdc_(base[0]), n_(static_cast<unsigned int>(base.size() - 1)) {
        if (n_ < 1) {
            throw std::invalid_argument("CylindN requires at least 2 bases");
        }

        if (n_ == 1) {
            c_gen_ = std::make_unique<CircleWrapper>(base[1]);
        } else {
            std::vector<unsigned long> sub_base(base.begin() + 1, base.end());
            c_gen_ = std::make_unique<CylindN>(sub_base);
        }
    }

    std::vector<double> CylindN::pop() {
        std::scoped_lock lock(mutex_);
        double cosphi = 2.0 * vdc_.pop() - 1.0;  // map to [-1, 1]
        double sinphi = std::sqrt(1.0 - cosphi * cosphi);

        auto sub_point = c_gen_->pop();
        std::vector<double> result;
        result.reserve(sub_point.size() + 1);
        for (double s : sub_point) result.emplace_back(sinphi * s);
        result.emplace_back(cosphi);
        return result;
    }

    void CylindN::reseed(unsigned long seed) {
        std::scoped_lock lock(mutex_);
        vdc_.reseed(seed);
        c_gen_->reseed(seed);
    }

}  // namespace ldsgen
