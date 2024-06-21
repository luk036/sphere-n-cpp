#include <cassert>
#include <cmath>
#include <memory>
#include <variant>
#include <vector>

// Assuming VdCorput and Circle classes are defined elsewhere
// and have methods like pop() and reseed(int).

const double PI = 3.14159265358979323846;

// Helper function to mimic Rust's pop_vec behavior
std::vector<double> toVec(const std::vector<double>& vec) {
    return vec;  // Simplified for demonstration; in practice, this might convert from another
                 // format
}

using CylindVariant = std::variant<std::unique_ptr<Circle>, std::unique_ptr<CylindN>>;

class CylindN {
  public:
    explicit CylindN(size_t n, std::vector<int> base) {
        assert(n >= 2);
        if (n == 2) {
            c_gen = std::make_unique<Circle>(base[1]);
        } else {
            c_gen
                = std::make_unique<CylindN>(n - 1, std::vector<int>(base.begin() + 1, base.end()));
        }
        vdc = VdCorput(base[0]);
    }

    std::vector<double> popVec() {
        double cosphi = 2.0 * vdc.pop() - 1.0;  // map to [-1, 1];
        double sinphi = sqrt(1.0 - cosphi * cosphi);
        std::vector<double> res
            = std::visit([](auto&& gen) { return toVec(gen->popVec()); }, *c_gen);
        for (double& xi : res) {
            xi *= sinphi;
        }
        res.push_back(cosphi);
        return res;
    }

    void reseed(size_t seed) {
        vdc.reseed(seed);
        std::visit([](auto&& gen) { gen->reseed(seed); }, *c_gen);
    }

  private:
    VdCorput vdc;
    CylindVariant c_gen;
};

// The Circle class needs to be updated to fit this pattern, ensuring it has popVec and reseed
// methods.

// Usage example:
int main() {
    std::vector<int> base = {10, 5, 3};  // Example base values
    std::unique_ptr<CylindN> cyl = std::make_unique<CylindN>(3, base);
    cyl->reseed(42);  // Reseed for reproducibility
    std::vector<double> result = cyl->popVec();
    // Process result...
    return 0;
}
