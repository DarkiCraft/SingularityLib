Just a small Linear Algebra Header-Only Template Library I am making in my free time to learn C++ Metaprogramming.
All help and suggestions will be appreciated!

Example usage:

```cpp
#include "Singularity/Matrix.hpp"

int main() {
  Sglty::RowVector<int, 3> m1{1, 2, 3};
  Sglty::ColVector<int, 3> m2{1, 2, 3};

  auto m = m2 * m1 * Sglty::Matrix<int, 3, 3>::Identity();

  m.Print();
}

// main.cpp
```
To compile and run:

`g++ -std=c++23 main.cpp -o main`
`./main`

Written and tested in C++23