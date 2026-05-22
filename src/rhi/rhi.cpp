#include "truffle/rhi/rhi.hpp"

namespace truffle::rhi {
// Backend-neutral RHI contracts intentionally keep implementation in backends.
namespace detail {
void link_anchor() noexcept {}
}
}
