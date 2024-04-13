#include <cmath>
#include <iostream>
#include <vector>

const double PI = 3.14159265358979323846;
const double HALF_PI = PI / 2.0;

class Sphere3 {
  public:
    Sphere3(const std::vector<int>& base) : base(base), seed(0) {}

    void reseed(int seed) { this->seed = seed; }

    std::vector<double> pop() {
        double ti = HALF_PI * vdCorput.pop();
        double xi = interp(tp, X, ti);
        double cosxi = cos(xi);
        double sinxi = sin(xi);
        std::vector<double> point = sphere2.pop();
        point[0] *= sinxi;
        point[1] *= sinxi;
        point[2] *= sinxi;
        point.push_back(cosxi);
        return point;
    }

  private:
    double interp(const std::vector<double>& x, const std::vector<double>& y, double xi) {
        int n = x.size();
        int i = 0;
        while (i < n && x[i] < xi) {
            i++;
        }
        if (i == 0) {
            return y[0];
        } else if (i == n) {
            return y[n - 1];
        } else {
            double x0 = x[i - 1];
            double x1 = x[i];
            double y0 = y[i - 1];
            double y1 = y[i];
            return y0 + (y1 - y0) * (xi - x0) / (x1 - x0);
        }
    }

    std::vector<double> X = linspace(0.0, PI, 300);
    std::vector<double> tp = linspace(0.0, PI, 300);
    std::vector<double> neg_cosine = map(X, [](double x) { return -cos(x); });
    std::vector<double> sine = map(X, [](double x) { return sin(x); });
    std::vector<int> base;
    int seed;
    VdCorput vdCorput{base[0]};
    Sphere sphere2{{base[1], base[2]}};
};

class NSphere {
  public:
    NSphere(int n, const std::vector<int>& base) : n(n), base(base), seed(0) {}

    void reseed(int seed) { this->seed = seed; }

    std::vector<double> pop() {
        std::vector<double> point;
        if (n == 3) {
            point = sphere3.pop();
        } else {
            std::vector<int> subBase(base.begin() + 1, base.end());
            NSphere sMinus1(n - 1, subBase);
            std::vector<double> ssnMinus2 = sMinus1.getTpMinus1();
            NSphere s(n - 1, subBase);
            point = s.pop();
            for (int i = 0; i < point.size(); i++) {
                point[i] *= sin(ssnMinus2[i]);
            }
            point.push_back(cos(point[0]));
        }
        seed++;
        return point;
    }

    std::vector<double> getTpMinus1() {
        if (n == 3) {
            return sphere3.getTp();
        } else {
            NSphere sMinus1(n - 1, std::vector<int>(base.begin() + 1, base.end()));
            return sMinus1.getTp();
        }
    }

  private:
    int n;
    std::vector<int> base;
    int seed;
    Sphere3 sphere3{{base[1], base[2], base[3]}};
};

class CylinN {
  public:
    CylinN(int n, const std::vector<int>& base) : n(n), base(base), seed(0) {}

    void reseed(int seed) { this->seed = seed; }

    std::vector<double> pop() {
        double cosphi = 2.0 * vdCorput.pop() - 1.0;
        double sinphi = sqrt(1.0 - cosphi * cosphi);
        std::vector<double> point;
        if (n == 2) {
            point = circle.pop();
        } else {
            CylinN c(n - 1, std::vector<int>(base.begin() + 1, base.end()));
            point = c.pop();
        }
        for (int i = 0; i < point.size(); i++) {
            point[i] *= sinphi;
        }
        point.push_back(cosphi);
        return point;
    }

  private:
    int n;
    std::vector<int> base;
    int seed;
    VdCorput vdCorput{base[0]};
    Circle circle{base[1]};
};

std::vector<double> linspace(double start, double end, int num) {
    std::vector<double> result;
    double step = (end - start) / (num - 1);
    for (int i = 0; i < num; i++) {
        result.push_back(start + i * step);
    }
    return result;
}

std::vector<double> map(const std::vector<double>& arr, double (*func)(double)) {
    std::vector<double> result;
    for (double num : arr) {
        result.push_back(func(num));
    }
    return result;
}

int main() {
    std::vector<int> base = {2, 3, 5, 7, 11};
    NSphere sgen(4, base);
    sgen.reseed(0);
    for (int i = 0; i < 10; i++) {
        std::vector<double> point = sgen.pop();
        for (double coord : point) {
            std::cout << coord << " ";
        }
        std::cout << std::endl;
    }
    std::vector<double> res = sgen.pop();
    std::cout << res[0] << std::endl;

    return 0;
}
