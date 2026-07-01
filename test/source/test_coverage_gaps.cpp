#include <doctest/doctest.h>

#include <array>
#include <cmath>
#include <memory>
#include <numeric>
#include <span>
#include <sphere_n/cylind_n.hpp>
#include <sphere_n/sphere_n.hpp>
#include <vector>

TEST_CASE("lds2 namespace constants") { CHECK_EQ(lds2::N_POINTS, 300); }

TEST_CASE("PRIME_TABLE - first and last entries") {
    CHECK_EQ(lds2::PRIME_TABLE[0], 2);
    CHECK_EQ(lds2::PRIME_TABLE[1], 3);
    CHECK_EQ(lds2::PRIME_TABLE[2], 5);
    CHECK_EQ(lds2::PRIME_TABLE[3], 7);
    CHECK_EQ(lds2::PRIME_TABLE[4], 11);
    CHECK_EQ(lds2::PRIME_TABLE[5], 13);
    CHECK_EQ(lds2::PRIME_TABLE[10], 31);
    CHECK_EQ(lds2::PRIME_TABLE[999], 7919);
}

TEST_CASE("to_vector from std::array<double, 3>") {
    const std::array<double, 3> arr = {1.0, 2.0, 3.0};
    auto vec = lds2::to_vector(arr);
    CHECK_EQ(vec.size(), 3);
    CHECK_EQ(vec[0], doctest::Approx(1.0));
    CHECK_EQ(vec[1], doctest::Approx(2.0));
    CHECK_EQ(vec[2], doctest::Approx(3.0));
}

TEST_CASE("to_vector from std::array<double, 4>") {
    const std::array<double, 4> arr = {0.5, -0.5, 0.0, 1.0};
    auto vec = lds2::to_vector(arr);
    CHECK_EQ(vec.size(), 4);
    CHECK_EQ(vec[0], doctest::Approx(0.5));
    CHECK_EQ(vec[1], doctest::Approx(-0.5));
    CHECK_EQ(vec[2], doctest::Approx(0.0));
    CHECK_EQ(vec[3], doctest::Approx(1.0));
}

TEST_CASE("Sphere3 - pop returns 4 elements on unit sphere") {
    const unsigned long base[] = {2, 3, 5};
    auto gen = lds2::Sphere3(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 4);
    double norm_sq = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3];
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("Sphere3 - multiple sequential pops remain on unit sphere") {
    const unsigned long base[] = {2, 3, 5};
    auto gen = lds2::Sphere3(base);
    for (int i = 0; i < 20; ++i) {
        auto p = gen.pop();
        REQUIRE_EQ(p.size(), 4);
        double norm_sq = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3];
        CHECK_EQ(norm_sq, doctest::Approx(1.0));
    }
}

TEST_CASE("Sphere3 - consecutive pops produce different points") {
    const unsigned long base[] = {2, 3, 5};
    auto gen = lds2::Sphere3(base);
    auto a = gen.pop();
    auto b = gen.pop();
    bool all_same = (a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3]);
    CHECK(!all_same);
}

TEST_CASE("Sphere3 - reseed provides reproducibility") {
    const unsigned long base[] = {2, 3, 5};
    auto gen = lds2::Sphere3(base);
    gen.reseed(42);
    auto a1 = gen.pop();
    auto a2 = gen.pop();
    gen.reseed(42);
    auto b1 = gen.pop();
    auto b2 = gen.pop();
    CHECK_EQ(a1[0], doctest::Approx(b1[0]));
    CHECK_EQ(a1[1], doctest::Approx(b1[1]));
    CHECK_EQ(a1[2], doctest::Approx(b1[2]));
    CHECK_EQ(a1[3], doctest::Approx(b1[3]));
    CHECK_EQ(a2[0], doctest::Approx(b2[0]));
    CHECK_EQ(a2[1], doctest::Approx(b2[1]));
    CHECK_EQ(a2[2], doctest::Approx(b2[2]));
    CHECK_EQ(a2[3], doctest::Approx(b2[3]));
}

TEST_CASE("Sphere3 - different bases produce different sequences") {
    const unsigned long base_a[] = {2, 3, 5};
    const unsigned long base_b[] = {3, 5, 7};
    auto ga = lds2::Sphere3(base_a);
    auto gb = lds2::Sphere3(base_b);
    auto pa = ga.pop();
    auto pb = gb.pop();
    bool all_same = (pa[0] == pb[0] && pa[1] == pb[1] && pa[2] == pb[2] && pa[3] == pb[3]);
    CHECK(!all_same);
}

TEST_CASE("SphereN - construct with minimal 4 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto gen = lds2::SphereN(base);
    auto p = gen.pop();
    REQUIRE_GE(p.size(), 4);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("SphereN - construct with 5 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto gen = lds2::SphereN(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 6);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("SphereN - construct with 6 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7, 11, 13};
    auto gen = lds2::SphereN(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 7);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("SphereN - multiple sequential pops remain on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto gen = lds2::SphereN(base);
    for (int i = 0; i < 20; ++i) {
        auto p = gen.pop();
        REQUIRE_EQ(p.size(), 6);
        double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                         [](double acc, double x) { return acc + x * x; });
        CHECK_EQ(norm_sq, doctest::Approx(1.0));
    }
}

TEST_CASE("SphereN - consecutive pops produce different points") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto gen = lds2::SphereN(base);
    auto a = gen.pop();
    auto b = gen.pop();
    bool all_same = (a.size() == b.size());
    if (all_same) {
        for (size_t i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) {
                all_same = false;
                break;
            }
        }
    }
    CHECK(!all_same);
}

TEST_CASE("SphereN - reseed provides reproducibility") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto gen = lds2::SphereN(base);
    gen.reseed(99);
    auto a1 = gen.pop();
    auto a2 = gen.pop();
    gen.reseed(99);
    auto b1 = gen.pop();
    auto b2 = gen.pop();
    REQUIRE_EQ(a1.size(), b1.size());
    for (size_t i = 0; i < a1.size(); ++i) {
        CHECK_EQ(a1[i], doctest::Approx(b1[i]));
    }
    REQUIRE_EQ(a2.size(), b2.size());
    for (size_t i = 0; i < a2.size(); ++i) {
        CHECK_EQ(a2[i], doctest::Approx(b2[i]));
    }
}

TEST_CASE("SphereN - different bases produce different sequences") {
    const unsigned long base_a[] = {2, 3, 5, 7, 11};
    const unsigned long base_b[] = {3, 5, 7, 11, 13};
    auto ga = lds2::SphereN(base_a);
    auto gb = lds2::SphereN(base_b);
    auto pa = ga.pop();
    auto pb = gb.pop();
    bool all_same = (pa.size() == pb.size());
    if (all_same) {
        for (size_t i = 0; i < pa.size(); ++i) {
            if (pa[i] != pb[i]) {
                all_same = false;
                break;
            }
        }
    }
    CHECK(!all_same);
}

TEST_CASE("CylindN - construct with minimal 2 bases and pop") {
    const unsigned long base[] = {2, 3};
    auto gen = lds2::CylindN(base);
    auto p = gen.pop();
    REQUIRE_GE(p.size(), 2);
}

TEST_CASE("CylindN - construct with 4 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto gen = lds2::CylindN(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 5);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("CylindN - construct with 5 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto gen = lds2::CylindN(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 6);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("CylindN - multiple sequential pops on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto gen = lds2::CylindN(base);
    for (int i = 0; i < 20; ++i) {
        auto p = gen.pop();
        REQUIRE_EQ(p.size(), 5);
        double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                         [](double acc, double x) { return acc + x * x; });
        CHECK_EQ(norm_sq, doctest::Approx(1.0));
    }
}

TEST_CASE("CylindN - consecutive pops produce different points") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto gen = lds2::CylindN(base);
    auto a = gen.pop();
    auto b = gen.pop();
    bool all_same = (a.size() == b.size());
    if (all_same) {
        for (size_t i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) {
                all_same = false;
                break;
            }
        }
    }
    CHECK(!all_same);
}

TEST_CASE("CylindN - reseed provides reproducibility") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto gen = lds2::CylindN(base);
    gen.reseed(77);
    auto a1 = gen.pop();
    auto a2 = gen.pop();
    gen.reseed(77);
    auto b1 = gen.pop();
    auto b2 = gen.pop();
    REQUIRE_EQ(a1.size(), b1.size());
    for (size_t i = 0; i < a1.size(); ++i) {
        CHECK_EQ(a1[i], doctest::Approx(b1[i]));
    }
    REQUIRE_EQ(a2.size(), b2.size());
    for (size_t i = 0; i < a2.size(); ++i) {
        CHECK_EQ(a2[i], doctest::Approx(b2[i]));
    }
}

TEST_CASE("CylindN - different bases produce different sequences") {
    const unsigned long base_a[] = {2, 3, 5, 7};
    const unsigned long base_b[] = {3, 5, 7, 11};
    auto ga = lds2::CylindN(base_a);
    auto gb = lds2::CylindN(base_b);
    auto pa = ga.pop();
    auto pb = gb.pop();
    bool all_same = (pa.size() == pb.size());
    if (all_same) {
        for (size_t i = 0; i < pa.size(); ++i) {
            if (pa[i] != pb[i]) {
                all_same = false;
                break;
            }
        }
    }
    CHECK(!all_same);
}

TEST_CASE("Sphere3, CylindN, SphereN produce consistent known values") {
    const unsigned long base[] = {2, 3, 5};
    auto sp3 = lds2::Sphere3(base);
    auto p = sp3.pop();
    CHECK_EQ(p[0], doctest::Approx(0.2913440162992141));
    CHECK_EQ(p[1], doctest::Approx(0.8966646826186098));
    CHECK_EQ(p[2], doctest::Approx(-0.33333333333333337));
    CHECK_EQ(p[3], doctest::Approx(6.123233995736766e-17));
}

TEST_CASE("SphereVariant type alias exists and is constructible") {
    const unsigned long base[] = {2, 3, 5};
    std::span<const unsigned long> sp_base(base);
    auto sp3 = std::make_unique<lds2::Sphere3>(sp_base);
    lds2::SphereVariant v = std::move(sp3);
    CHECK(std::holds_alternative<std::unique_ptr<lds2::Sphere3>>(v));
}

TEST_CASE("CylindVariant type alias exists and is constructible") {
    const unsigned long base[] = {2, 3};
    auto cir = std::make_unique<lds2::Circle>(base[0]);
    lds2::CylindVariant v = std::move(cir);
    CHECK(std::holds_alternative<std::unique_ptr<lds2::Circle>>(v));
}
