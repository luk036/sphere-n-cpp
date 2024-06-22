#include <CGAL/Convex_hull_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

// Include SphereN and CylinN headers assuming they are defined elsewhere
#include "CylinN.h"
#include "SphereN.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef K::Point_3 Point_3;

// Define a type alias for a 3D point represented as a std::vector<double>
using Vector3d = std::vector<double>;

// Adjust the discrep_2 function to work with the new Vector3d type
double discrep_2(const std::vector<std::vector<int>>& Ks, const std::vector<Vector3d>& X) {
    double maxq = 0.0;
    double minq = 1000.0;
    for (const auto& face : Ks) {
        for (size_t i = 0; i < face.size() - 1; ++i) {
            for (size_t j = i + 1; j < face.size(); ++j) {
                double dot = X[face[i]][0] * X[face[j]][0] + X[face[i]][1] * X[face[j]][1]
                             + X[face[i]][2] * X[face[j]][2];
                double q = 1.0 - dot * dot;
                maxq = std::max(maxq, q);
                minq = std::min(minq, q);
            }
        }
    }
    return std::acos(std::sqrt(maxq)) - std::acos(std::sqrt(minq));
}

std::vector<Point_3> toCGALPoints(const std::vector<Vector3d>& points) {
    std::vector<Point_3> cgalPoints;
    for (const auto& point : points) {
        cgalPoints.emplace_back(point[0], point[1], point[2]);
    }
    return cgalPoints;
}

double run_lds(SphereGenerator& spgen) {
    const int npoints = 600;
    std::vector<Vector3d> triples(npoints);
    for (int i = 0; i < npoints; ++i) {
        Vector3d point = spgen.pop();  // Ensure spgen.pop() returns a Vector3d
        triples[i] = point;
    }
    std::vector<Point_3> points = toCGALPoints(triples);
    CGAL::Convex_hull_3_points_traits_3<K> ch_traits;
    CGAL::convex_hull_3(points.begin(), points.end(), ch_traits);
    std::vector<std::vector<int>> triangles;  // Still a placeholder
    return discrep_2(triangles, triples);
}

// Ensure SphereN and CylinN classes' pop() methods return Vector3d
void test_sphere_n() {
    SphereN spgen({2, 3, 5, 7});
    double measure = run_lds(spgen);
    assert(std::abs(measure - 0.9125914) < 1e-6);
}

void test_cylin_n() {
    CylinN cygen({2, 3, 5, 7});
    double measure = run_lds(cygen);
    assert(std::abs(measure - 1.0505837105828988) < 1e-6);
}

int main() {
    test_sphere_n();
    test_cylin_n();
    std::cout << "All tests passed." << std::endl;
    return 0;
}
