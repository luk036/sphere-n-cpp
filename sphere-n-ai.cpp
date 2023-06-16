#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

class VdCorput {
  int base;
  int seed;

public:
  VdCorput(int base) : base(base), seed(0) {}
  double pop() {
    double r = 0.0;
    double base_inv = 1.0 / base;
    double f = base_inv;
    int n = seed;
    while (n > 0) {
      d = n % base;
      r += d * f;
      f *= base_inv;
      n /= base;
    }
    seed++;
    return r;
  }
  void reseed(int seed) { this->seed = seed; }
};

class Circle {
  VdCorput vdc;
  std::vector<double> c_gen;

public:
  Circle(int base) : vdc(base), c_gen() {}
  std::vector<double> pop() {
    double cosphi = 2.0 * vdc.pop() - 1.0;
    double sinphi = std::sqrt(1.0 - cosphi * cosphi);
    std::vector<double> c = c_gen.pop();
    std::vector<double> result(c.size() + 1);
    std::transform(c.begin(), c.end(), result.begin(),
                   [&](double xi) { return xi * sinphi; });
    result.back() = cosphi;
    return result;
  }
  void reseed(int seed) {
    vdc.reseed(seed);
    c_gen.reseed(seed);
  }
};

class CylinN : public Circle {
  int n;

public:
  CylinN(int n, std::vector<int> base) : Circle(base[1]), n(n) {
    assert(n >= 1);
    vdc = VdCorput(base[0]);
    if (n > 1) {
      c_gen = std::vector<double>(n - 1);
      CylinN(n - 1, std::vector<int>(base.begin() + 1, base.end())).swap(c_gen);
    }
  }
  std::vector<double> pop() {
    std::vector<double> result = Circle::pop();
    std::vector<double> c = c_gen.pop();
    result.insert(result.end(), c.begin(), c.end());
    return result;
  }
};

std::vector<double> linspace(double start, double end, int num) {
  std::vector<double> result(num);
  double step = (end - start) / (num - 1);
  std::generate(result.begin(), result.end(), [&]() {
    double val = start;
    start += step;
    return val;
  });
  return result;
}

std::vector<double> neg_cosine = []() {
  std::vector<double> result = linspace(0.0, M_PI, 300);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](double x) { return -std::cos(x); });
  return result;
}();

std::vector<double> sine = []() {
  std::vector<double> result = linspace(0.0, M_PI, 300);
  std::transform(result.begin(), result.end(), result.begin(),
                 [](double x) { return std::sin(x); });
  return result;
}();

std::vector<double> get_tp(int n) {
  if (n == 0) {
    return linspace(0.0, M_PI, 300);
  }
  if (n == 1) {
    return neg_cosine;
  }
  std::vector<double> tp_minus2 = get_tp(n - 2);
  std::vector<double> result(tp_minus2.size());
  std::transform(tp_minus2.begin(), tp_minus2.end(), sine.begin() + n - 1,
                 result.begin(), [&](double tp, double sin_n_minus_1) {
                   return ((n - 1) * tp + neg_cosine[n - 1] * sin_n_minus_1) /
                          n;
                 });
  return result;
}

class Sphere {
  VdCorput vdc;
  std::vector<double> s_gen;

public:
  Sphere(std::vector<int> base) : vdc(base[0]), s_gen(base[1]) {}
  std::vector<double> pop() {
    double ti = M_PI_2 * vdc.pop();
    double xi = std::interp(ti, get_tp(2), linspace(0.0, M_PI, 300));
    double cosxi = std::cos(xi);
    double sinxi = std::sin(xi);
    std::vector<double> s = s_gen.pop();
    std::vector<double> result(s.size() + 1);
    std::transform(s.begin(), s.end(), result.begin(),
                   [&](double s_i) { return sinxi * s_i; });
    result.back() = cosxi;
    return result;
  }
  void reseed(int seed) {
    vdc.reseed(seed);
    s_gen.reseed(seed);
  }
};

class SphereN : public Sphere {
  int n;
  double range;

public:
  SphereN(int n, std::vector<int> base)
      : Sphere(std::vector<int>(base.begin() + 1, base.begin() + 3)), n(n) {
    assert(n >= 2);
    vdc = VdCorput(base[0]);
    if (n > 2) {
      s_gen = std::vector<double>(n - 2);
      SphereN(n - 1, std::vector<int>(base.begin() + 1, base.end()))
          .swap(s_gen);
    }
    std::vector<double> tp = get_tp(n);
    range = tp.back() - tp.front();
  }
  std::vector<double> pop() {
    double vd = vdc.pop();
    std::vector<double> s = s_gen.pop();
    std::vector<double> tp = get_tp(n);
    double ti = tp.front() + range * vd;
    double xi = std::interp(ti, tp, linspace(0.0, M_PI, 300));
    double sinphi = std::sin(xi);
    std::vector<double> result(s.size() + 1);
    std::transform(s.begin(), s.end(), result.begin(),
                   [&](double s_i) { return xi * sinphi * s_i; });
    result.back() = std::cos(xi);
    return result;
  }
  void reseed(int seed) {
    vdc.reseed(seed);
    s_gen.reseed(seed);
  }
};

double discrep_2(const std::vector<std::array<int, 3>> &K,
                 const std::vector<std::vector<double>> &X) {
  int nsimplex = K.size();
  int n = K[0].size();
  double maxq = 0.0;
  double minq = 1000.0;
  for (int k = 0; k < nsimplex; k++) {
    std::vector<std::vector<double>> p(n);
    for (int i = 0; i < n; i++) {
      p[i] = X[K[k][i]];
    }
    for (int i = 0; i < n - 1; i++) {
      for (int j = i + 1; j < n; j++) {
        double dot =
            std::inner_product(p[i].begin(), p[i].end(), p[j].begin(), 0.0);
        double q = 1.0 - dot * dot;
        maxq = std::max(maxq, q);
        minq = std::min(minq, q);
      }
    }
  }
  double dis = std::asin(std::sqrt(maxq)) - std::asin(std::sqrt(minq));
  return dis;
}

double run_lds(SphereN &spgen) {
  int npoints = 600;
  std::vector<std::vector<double>> Triples(npoints);
  std::generate(Triples.begin(), Triples.end(), [&]() { return spgen.pop(); });
  std::vector<std::array<int, 3>> hull = ConvexHull(Triples).simplices;
  return discrep_2(hull, Triples);
}

double run_lds(CylinN &cygen) {
  int npoints = 600;
  std::vector<std::vector<double>> Triples(npoints);
  std::generate(Triples.begin(), Triples.end(), [&]() { return cygen.pop(); });
  std::vector<std::array<int, 3>> hull = ConvexHull(Triples).simplices;
  return discrep_2(hull, Triples);
}

int main() {
  SphereN spgen(3, {2, 3, 5, 7});
  double measure = run_lds(spgen);
  assert(measure == 0.9125914);
  CylinN cygen(3, {2, 3, 5, 7});
  measure = run_lds(cygen);
  assert(measure == 1.0505837105828988);
  return 0;
}
