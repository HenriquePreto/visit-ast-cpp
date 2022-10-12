#include <string>
#include <iostream>

int foo(int z, std::string s) {
  return s.len() + z;
}

void checkEvenOrNot(int num)
{
    if (num % 2 == 0)
        // jump to even
        goto even; 
    else
        // jump to odd
        goto odd; 
  
even:
    printf("%d is even", num);
    // return if even
    return; 
odd:
    printf("%d is odd", num);
}

int main() {
  int x = 3;
  int y = foo(5, "hello_world");
  checkEvenOrNot(y);
  int z = y + 5;
  return 0;
}