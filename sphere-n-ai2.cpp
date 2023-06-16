#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <numeric>
#include <vector>

static const auto PI = 3.1315926;

class SphereTrait {
public:
  virtual std::vector<double> pop() = 0;
  virtual ~SphereTrait() {}
};

class Sphere {
public:
  Sphere(const std::vector<int> &base) {}
  std::vector<double> pop() { return {0.0, 0.0}; }
};

std::vector<double> get_tp(int n) {
  std::vector<double> x(300);
  std::iota(x.begin(), x.end(), 0.0);
  std::transform(x.begin(), x.end(), x.begin(),
                 [](double xi) { return xi * PI / 299.0; });
  if (n == 2) {
    return {0.0, PI};
  }
  auto tp_minus2 = get_tp(n - 2);
  std::vector<double> sine_n_minus_1(x.size());
  std::transform(x.begin(), x.end(), sine_n_minus_1.begin(),
                 [&](double xi) { return std::pow(std::sin(xi), n - 1); });
  std::vector<double> numerator(x.size());
  std::transform(tp_minus2.begin(), tp_minus2.end(), sine_n_minus_1.begin(),
                 numerator.begin(), [&](double tp, double sine_n_minus_1) {
                   return (n - 1) * tp + -std::cos(xi) * sine_n_minus_1;
                 });
  return std::vector<double>(numerator.begin(), numerator.end());
}

class VdCorput {
public:
  VdCorput(int base) {}
  double pop() { return 0.0; }
};

class SphereN : public SphereTrait {
public:
  SphereN(int n, const std::vector<int> &base) {
    assert(n >= 2);
    vdc = VdCorput(base[0]);
    if (n == 2) {
      s_gen = std::make_unique<Sphere>(Sphere({base[1], base[2]}));
    } else {
      s_gen = std::make_unique<SphereN>(
          SphereN(n - 1, std::vector<int>(base.begin() + 1, base.end())));
    }
    this->n = n;
    auto tp = get_tp(n);
    range = tp.back() - tp.front();
  }
  std::vector<double> pop() {
    auto vd = vdc.pop();
    auto tp = get_tp(n);
    auto ti = tp.front() + range * vd;
    auto xi = std::lower_bound(tp.begin(), tp.end(), ti) - tp.begin();
    auto sinphi = std::sin(xi);
    auto s_gen_pop = s_gen->pop();
    std::vector<double> res(s_gen_pop.size() + 1);
    std::transform(s_gen_pop.begin(), s_gen_pop.end(), res.begin(),
                   [&](double x) { return x * sinphi; });
    res.back() = std::cos(xi);
    return res;
  }

private:
  VdCorput vdc;
  std::unique_ptr<SphereTrait> s_gen;
  int n;
  double range;
};
