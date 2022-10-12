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

void switch_breaks(int c) {
  switch (c) {
  case 0:
    c++;
    break;
  case 1:
    c++;
    break;
  case 2:
    c++;
    break;
  case 3:
    break;
  case 4:
    c++;
    break;
  }
}

// void switch_nobreaks(char c) {
//   switch (c) {
//   case 'a':
//     c++;
//   case 'e':
//   case 'i':
//     c++;
//   case 'o':
//   case 'u':
//     break;
//   }
// }

int main() {
  int x = 3;
  int y = foo(5, "hello_world");
  checkEvenOrNot(y);
  int z = y + 5;
  return 0;
}