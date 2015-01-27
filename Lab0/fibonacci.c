

#include<stdio.h>

void fibonacci(int a, int b, int max);

void fibonacci(int a, int b, int max){
  
  a = a + b;
  
  if (a<max){
    printf("%i\n",a);
    fibonacci(b,a,max);
  }
  
}

int main( void ) {

  int a=1, b=1;

  printf("%i\n%i\n",a,b);
  fibonacci(a,b,100);

  return 0;
}


