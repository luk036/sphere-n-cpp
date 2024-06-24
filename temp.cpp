namespace lds2 {
    using gsl::span;
    using ldsgen::Circle;
    using ldsgen::Sphere;
    using ldsgen::VdCorput;
    using std::array;
    using std::vector;

    class Sphere3 {
        VdCorput vdc;
        Sphere sphere2;

      public:
        explicit Sphere3(span<const size_t> base);

        auto pop() -> array<double, 4>;
    };

    class SphereN;

    using SphereVariant = std::variant<std::unique_ptr<Sphere3>, std::unique_ptr<SphereN>>;

    class SphereN {
        size_t n;
        VdCorput vdc;
        SphereVariant s_gen;

      public:
        explicit SphereN(span<const size_t> base);

        auto pop() -> vector<double>;

        auto reseed(size_t seed) -> void;
    };

    using gsl::span;
    using std::array;
    using std::cos;
    using std::sin;
    using std::sqrt;
    using std::vector;

    Sphere3::Sphere3(span<const size_t> base) : vdc{base[0]}, sphere2{base[1], base[2]} {}

    /**
     * @brief
     *
     * @return array<double, 4>
     */
    auto Sphere3::pop() -> array<double, 4> {
        const auto ti = HALF_PI * this->vdc.pop();  // map to [0, pi/2];
        const auto &tp = GL.getTp(3);
        const auto xi = ::interp(GL.X, tp, ti);
        const auto cosxi = cos(xi);
        const auto sinxi = sin(xi);
        const auto [s0, s1, s2] = this->sphere2.pop();
        return {sinxi * s0, sinxi * s1, sinxi * s2, cosxi};
    }

    SphereN::SphereN(gsl::span<const size_t> base) : vdc{base[0]} {
        const auto m = base.size();
        assert(m >= 4);
        // Arr tp_minus2;
        if (m == 4) {
            // tp_minus2 = NEG_COSINE;
            // this->s_gen = std::make_unique<Sphere3>(base.subspan(1, 3));
            this->s_gen = std::make_unique<Sphere3>(base.subspan(1, 3));
        } else {
            auto s_minus1 = std::make_unique<SphereN>(base.last(m - 1));
            // tp_minus2 = s_minus1->get_tp_minus1();
            this->s_gen = std::move(s_minus1);
        }
        this->n = m - 1;
        // this->tp = ((n - 1.0) * tp_minus2 + NEG_COSINE * xt::pow(SINE, n - 1.0))
        // / n;
    }

    auto SphereN::pop() -> vector<double> {
        const auto vd = this->vdc.pop();
        const auto &tp = GL.getTp(this->n);
        const auto ti = tp[0] + (tp[tp.size() - 1] - tp[0]) * vd;  // map to [t0, tm-1];
        const auto xi = ::interp(GL.X, tp, ti);
        const auto sinphi = sin(xi);
        auto res = std::visit(
            [](auto &t) {
                using T = std::decay_t<decltype(*t)>;
                if constexpr (std::is_same_v<T, Sphere3>) {
                    auto arr = t->pop();
                    return vector<double>(arr.begin(), arr.end());
                } else if constexpr (std::is_same_v<T, SphereN>) {
                    return t->pop();
                } else {
                    return vector<double>{};
                }
            },
            this->s_gen);
        for (auto &elem : res) {
            elem *= sinphi;
        }
        res.emplace_back(cos(xi));
        return res;
    }
}
