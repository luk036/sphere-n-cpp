#include <doctest/doctest.h>  // for Approx, ResultBuilder, TestCase

#include <span>                   // for span
#include <sphere_n/cylind_n.hpp>  // for cylin_n, halton_n, sphere3, sphere_n
#include <sphere_n/sphere_n.hpp>  // for cylin_n, halton_n, sphere3, sphere_n
#include <vector>                 // for vector

TEST_CASE("Sphere3") {
    const size_t base[] = {2, 3, 5};
    auto sp3gen = lds2::Sphere3(base);
    const auto res = sp3gen.pop();
    CHECK_EQ(res[1], doctest::Approx(0.896665));
}

// TEST_CASE("HaltonN") {
//     const size_t base[] = {2, 3, 5, 7};
//     auto hgen = lds2::HaltonN(base);
//     const auto res = hgen.pop();
//     CHECK_EQ(res[0], doctest::Approx(0.5));
// }

TEST_CASE("CylindN") {
    const size_t base[] = {2, 3, 5, 7};
    auto cygen = lds2::CylindN(base);
    const auto res = cygen.pop();
    CHECK_EQ(res[1], doctest::Approx(0.5896942325));
}

TEST_CASE("SphereN") {
    const size_t base[] = {2, 3, 5, 7, 11};
    auto spgen = lds2::SphereN(base);
    const auto res = spgen.pop();
    CHECK_EQ(res[1], doctest::Approx(0.320904));
}
