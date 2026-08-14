#include <sphere_n/version.h>

#include <cassert>
#include <sphere_n/sphere_n.hpp>

auto main() -> int {
    assert(SPHERE_N_VERSION_MAJOR == 1);

    ldsgen::VdCorput vdc(2);
    const auto first = vdc.pop();
    assert(first == 0.5);

    return 0;
}
