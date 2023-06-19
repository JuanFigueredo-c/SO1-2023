#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Para compilar este programa utilizar la bandera -lm

int main(){
    double number;
    int divisors = 0;
    printf("Ingrese numero: \n");
    scanf("%lf",&number);
    double limit = floor(sqrt(number));
    #pragma omp parallel for reduction (+:divisors) // busqueda en paralelo de los divisores entre 2 y sqrt(N)
    for (double i = 2; i <= limit; i++) {            
        if (fmod(number, i) == 0) divisors = divisors + 1;
    }
    
    if (divisors == 0) printf("The number: %lf is a prime number \n",number);
    else {
        printf("The number %lf is not a prime number.\n", number);
    }   
    return 0;
}
