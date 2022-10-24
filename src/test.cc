#include <stdio.h>

int GLOB = 0.3 + 3.4;

namespace hello_world {
  int bar();

  void foo(int aaa, int bbb) {
    int i = 0;
    int j = 4;
    int k = i + j;
    int p = bar(); // 1
    double x = 3.3;
    float y = 4.4f;
    int z = x + y; // 2
    double w = y + z; // 2
    printf("%d", x + y); // 1
  }

  int bar() {
    return 0.0;
  }

  namespace cool {
    int foo(int aaa, int bbb) {
      return 0;
    }
  }
}

int main() {
  int i = 0;
  int j = 4;
  int k = i + j;
  double x = 3.3;
  float y = 4.4f;
  int z = x + y; // 2
  double w = y + z; // 2
  hello_world::foo(10, 11); // 1
  printf("%f", x + y); // 1
  return 0; // 1
}