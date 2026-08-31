#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase

#include <ldsgen/sphere_n.hpp>      // for sphere3, sphere_n
#include <sphere_n/cylind_n.hpp>    // for cylind_n
#include <vector>                 // for vector

TEST_CASE("Sphere3") {
    const unsigned long base[] = {2, 3, 5};
    auto sp3gen = ldsgen::Sphere3(base);
    const auto res = sp3gen.pop();
    CHECK_EQ(res[0], doctest::Approx(0.2913440162992141));
    CHECK_EQ(res[1], doctest::Approx(0.8966646826186098));
    CHECK_EQ(res[2], doctest::Approx(-0.33333333333333337));
    CHECK_EQ(res[3], doctest::Approx(6.123233995736766e-17));
}

// TEST_CASE("HaltonN") {
//     const size_t base[] = {2, 3, 5, 7};
//     auto hgen = ldsgen::HaltonN(base);
//     const auto res = hgen.pop();
//     CHECK_EQ(res[0], doctest::Approx(0.5));
// }

TEST_CASE("CylindN") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto cygen = ldsgen::CylindN(base);
    const auto res = cygen.pop();
    CHECK_EQ(res[1], doctest::Approx(0.5896942325));
}

TEST_CASE("SphereN") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto spgen = ldsgen::SphereN(base);
    const auto res = spgen.pop();
    CHECK_EQ(res[1], doctest::Approx(0.320904));
}
