#include <iostream>
#include <optional>
// #include <span>
// #include <vector>
// #include <optional>
// #include "tsl/literal_string.hpp"
// #include "tsl/macros.hpp"
// #include "tsl/linux/subprocess.hpp"
// #include "tsl/attributes.hpp"
#include "tsl/types/contracts.hpp"
#include "tsl/maybe.hpp"

using namespace std;
using namespace tsl;

// consteval void f(bool x) {
//     TSL_ASSERT(x && "triste");
// }

int main() {
    maybe<non_negative<int>> m = 10;
    m = 34;

    cout << bool(m) << "\n";
    return 0;
}
