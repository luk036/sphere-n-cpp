#include <doctest/doctest.h>

#include <array>
#include <cmath>
#include <memory>
#include <numeric>
#include <span>
#include <ldsgen/sphere_n.hpp>
#include <sphere_n/cylind_n.hpp>
#include <vector>

TEST_CASE("ldsgen namespace constants") { CHECK_EQ(ldsgen::TABLE_SIZE, 300); }

TEST_CASE("Sphere3 - pop returns 4 elements on unit sphere") {
    const unsigned long base[] = {2, 3, 5};
    auto gen = ldsgen::Sphere3(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 4);
    double norm_sq = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3];
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("Sphere3 - multiple sequential pops remain on unit sphere") {
    const unsigned long base[] = {2, 3, 5};
    auto gen = ldsgen::Sphere3(base);
    for (int i = 0; i < 20; ++i) {
        auto p = gen.pop();
        REQUIRE_EQ(p.size(), 4);
        double norm_sq = p[0] * p[0] + p[1] * p[1] + p[2] * p[2] + p[3] * p[3];
        CHECK_EQ(norm_sq, doctest::Approx(1.0));
    }
}

TEST_CASE("Sphere3 - consecutive pops produce different points") {
    const unsigned long base[] = {2, 3, 5};
    auto gen = ldsgen::Sphere3(base);
    auto a = gen.pop();
    auto b = gen.pop();
    bool all_same = (a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3]);
    CHECK(!all_same);
}

TEST_CASE("Sphere3 - reseed provides reproducibility") {
    const unsigned long base[] = {2, 3, 5};
    auto gen = ldsgen::Sphere3(base);
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
    auto ga = ldsgen::Sphere3(base_a);
    auto gb = ldsgen::Sphere3(base_b);
    auto pa = ga.pop();
    auto pb = gb.pop();
    bool all_same = (pa[0] == pb[0] && pa[1] == pb[1] && pa[2] == pb[2] && pa[3] == pb[3]);
    CHECK(!all_same);
}

TEST_CASE("SphereN - construct with minimal 4 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto gen = ldsgen::SphereN(base);
    auto p = gen.pop();
    REQUIRE_GE(p.size(), 4);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("SphereN - construct with 5 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto gen = ldsgen::SphereN(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 6);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("SphereN - construct with 6 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7, 11, 13};
    auto gen = ldsgen::SphereN(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 7);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("SphereN - multiple sequential pops remain on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto gen = ldsgen::SphereN(base);
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
    auto gen = ldsgen::SphereN(base);
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
    auto gen = ldsgen::SphereN(base);
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
    auto ga = ldsgen::SphereN(base_a);
    auto gb = ldsgen::SphereN(base_b);
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
    auto gen = ldsgen::CylindN(base);
    auto p = gen.pop();
    REQUIRE_GE(p.size(), 2);
}

TEST_CASE("CylindN - construct with 4 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto gen = ldsgen::CylindN(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 5);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("CylindN - construct with 5 bases and pop on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    auto gen = ldsgen::CylindN(base);
    auto p = gen.pop();
    REQUIRE_EQ(p.size(), 6);
    double norm_sq = std::accumulate(p.begin(), p.end(), 0.0,
                                     [](double acc, double x) { return acc + x * x; });
    CHECK_EQ(norm_sq, doctest::Approx(1.0));
}

TEST_CASE("CylindN - multiple sequential pops on unit sphere") {
    const unsigned long base[] = {2, 3, 5, 7};
    auto gen = ldsgen::CylindN(base);
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
    auto gen = ldsgen::CylindN(base);
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
    auto gen = ldsgen::CylindN(base);
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
    auto ga = ldsgen::CylindN(base_a);
    auto gb = ldsgen::CylindN(base_b);
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
    auto sp3 = ldsgen::Sphere3(base);
    auto p = sp3.pop();
    CHECK_EQ(p[0], doctest::Approx(0.2913440162992141));
    CHECK_EQ(p[1], doctest::Approx(0.8966646826186098));
    CHECK_EQ(p[2], doctest::Approx(-0.33333333333333337));
    CHECK_EQ(p[3], doctest::Approx(6.123233995736766e-17));
}

TEST_CASE("SphereGen polymorphic dispatch with SphereWrapper") {
    const unsigned long base[] = {2, 3};
    std::span<const unsigned long> sp_base(base);
    std::unique_ptr<ldsgen::SphereGen> gen = std::make_unique<ldsgen::SphereWrapper>(sp_base);
    auto p = gen->pop();
    REQUIRE_EQ(p.size(), 3);
}

TEST_CASE("SphereGen polymorphic dispatch with SphereN") {
    const unsigned long base[] = {2, 3, 5, 7, 11};
    std::span<const unsigned long> sp_base(base);
    std::unique_ptr<ldsgen::SphereGen> gen = std::make_unique<ldsgen::SphereN>(sp_base);
    auto p = gen->pop();
    REQUIRE_EQ(p.size(), 6);
}

TEST_CASE("CylindGen polymorphic dispatch with CylindN") {
    const unsigned long base[] = {2, 3, 5, 7};
    std::span<const unsigned long> sp_base(base);
    std::unique_ptr<ldsgen::CylindGen> gen = std::make_unique<ldsgen::CylindN>(sp_base);
    auto p = gen->pop();
    REQUIRE_EQ(p.size(), 5);
}

TEST_CASE("CylindGen polymorphic dispatch with CircleWrapper") {
    std::unique_ptr<ldsgen::CylindGen> gen = std::make_unique<ldsgen::CircleWrapper>(7);
    auto p = gen->pop();
    REQUIRE_EQ(p.size(), 2);
}
