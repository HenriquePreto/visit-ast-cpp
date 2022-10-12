int GLOB = 0.3 + 3.4;

namespace hello_world {
  int foo(int aaa, int bbb) {
      int i = 0;
    int j = 4;
    int k = i + j;
    double x = 3.3;
    float y = 4.4;
    int z = x + y;
    double w = y + z;
    return 0;
  }

  namespace cool {
    int foo(int aaa, int bbb) {
      // empty
    }
  }
}

int main() {
  int i = 0;
  int j = 4;
  int k = i + j;
  double x = 3.3;
  float y = 4.4;
  int z = x + y;
  double w = y + z;
  hello_world::foo(10, 11);
  return 0;
}