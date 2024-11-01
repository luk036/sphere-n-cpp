#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullVertexSet.h"

using namespace std;

typedef vector<vector<double>> Matrix;
typedef vector<vector<int>> IntMatrix;
typedef vector<double> Vector;

double dot(const Vector& a, const Vector& b) {
    double sum = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

double norm(const Vector& v) { return sqrt(dot(v, v)); }

void normalize(Vector& v) {
    double n = norm(v);
    for (double& x : v) {
        x /= n;
    }
}

double discrep_2(const IntMatrix& K, const Matrix& X) {
    int nsimplex = K.size();
    int n = K[0].size();
    double maxq = 0;
    double minq = 1000;
    for (int k = 0; k < nsimplex; ++k) {
        vector<Vector> p(n);
        for (int i = 0; i < n; ++i) {
            p[i] = X[K[k][i]];
        }
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                double dot_product = dot(p[i], p[j]);
                double q = 1.0 - dot_product * dot_product;
                maxq = max(maxq, q);
                minq = min(minq, q);
            }
        }
    }
    double dis = asin(sqrt(maxq)) - asin(sqrt(minq));
    return dis;
}

Matrix sample_spherical(int npoints, int ndim = 3) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> d(0, 1);

    Matrix vec(npoints, Vector(ndim));
    for (int i = 0; i < npoints; ++i) {
        for (int j = 0; j < ndim; ++j) {
            vec[i][j] = d(gen);
        }
        normalize(vec[i]);
    }
    return vec;
}

double dispersion(const Matrix& Triples) {
    int npoints = Triples.size();
    int ndim = Triples[0].size();
    vector<double> points;
    for (const auto& row : Triples) {
        points.insert(points.end(), row.begin(), row.end());
    }

    orgQhull::Qhull qhull;
    qhull.runQhull("", ndim, npoints, points.data(), "Qt");

    vector<vector<int>> triangles;
    for (const auto& facet : qhull.facetList()) {
        vector<int> triangle;
        for (const auto& vertex : facet.vertices()) {
            triangle.push_back(vertex.point().id());
        }
        triangles.push_back(triangle);
    }

    return discrep_2(triangles, Triples);
}

class Sphere3Hopf {
  public:
    Sphere3Hopf(const vector<int>& params) {
        // Initialize Sphere3Hopf
    }

    Vector pop() {
        // Implement pop method
        return Vector(4, 0.0);
    }
};

class Sphere3 {
  public:
    Sphere3(const vector<int>& params) {
        // Initialize Sphere3
    }

    Vector pop() {
        // Implement pop method
        return Vector(4, 0.0);
    }
};

int main() {
    int npoints = 2001;
    int ndim = 4;
    Matrix Triples_r = sample_spherical(npoints, ndim);

    Sphere3Hopf sphopfgen({2, 3, 5});
    Sphere3 spgen({2, 3, 5});

    Matrix Triples_h(npoints, Vector(ndim));
    Matrix Triples_s(npoints, Vector(ndim));
    for (int i = 0; i < npoints; ++i) {
        Triples_h[i] = sphopfgen.pop();
        Triples_s[i] = spgen.pop();
    }

    vector<int> x;
    for (int i = 100; i < npoints; i += 100) {
        x.push_back(i);
    }

    vector<double> res_r, res_h, res_s;

    for (int i : x) {
        Matrix subset_r(Triples_r.begin(), Triples_r.begin() + i);
        Matrix subset_h(Triples_h.begin(), Triples_h.begin() + i);
        Matrix subset_s(Triples_s.begin(), Triples_s.begin() + i);

        res_r.push_back(dispersion(subset_r));
        res_h.push_back(dispersion(subset_h));
        res_s.push_back(dispersion(subset_s));
    }

    // Print results (plotting not implemented)
    cout << "x: ";
    for (int i : x) cout << i << " ";
    cout << endl;

    cout << "res_r: ";
    for (double r : res_r) cout << r << " ";
    cout << endl;

    cout << "res_h: ";
    for (double r : res_h) cout << r << " ";
    cout << endl;

    cout << "res_s: ";
    for (double r : res_s) cout << r << " ";
    cout << endl;

    return 0;
}
