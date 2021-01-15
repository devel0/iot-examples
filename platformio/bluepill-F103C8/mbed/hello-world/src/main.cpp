#include <mbed.h>

Serial pc(PA_9, PA_10); // default: 9600

int main() {
  int a = 1;
  pc.printf("SETUP\n");
  int b = a*2;
  pc.printf("b=%d\n", b);

  while(1) {
    // put your main code here, to run repeatedly:
  }
}
