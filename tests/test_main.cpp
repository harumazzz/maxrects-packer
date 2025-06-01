#include "simple_test.h"

auto main() -> int {
    return simple_test_runner::run_all_tests() ? 0 : 1;
}
