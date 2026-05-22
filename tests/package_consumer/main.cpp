#include "truffle/rhi/null_backend.hpp"

int main() {
  auto backend = truffle::rhi::create_null_backend();
  auto deviceResult = backend->create_device({});
  return deviceResult.ok() ? 0 : 1;
}
