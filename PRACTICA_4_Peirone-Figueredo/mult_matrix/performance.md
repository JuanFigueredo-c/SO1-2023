## Ejercicio *a*
Tiempo total de duración del programa:
Secuencial:
* N =  200:  0.06610s
* N =  500:  0.97927s
* N = 1000: 11.42864s

Paralelizado con OpenMP (4 hilos):
* N =  200: 0.07726s
* N =  500: 0.54753s
* N = 1000: 5.86300s

El speed up entonces para los tamaños probados será de:
* N =  200: 0.85555s
* N =  500: 1.78852s
* N = 1000: 1.94928s

La eficiencia es de:
* N =  200: 0.21389
* N =  500: 0.44713
* N = 1000: 0.48732

## Ejercicio *b*
Si se intercambian los for loops de las variables j y k se puede obtener una mejora
de performance muy significativa. Esto se debe a la reducción de fallos de cache
que proporciona esta mejora. 
Esto se debe a que `B[k][j]` implica ahora en el peor caso un solo fallo de cache
por cada N ciclos de la variable j, mientras que antes se podía tener los siguientes
fallos de cache: `B[0], B[1], ..., B[N-1]`. Osea, se podiía tener hasta N fallos en
cada ciclo for de la variable k

## Ejercicio *c*
Se puede mejorar el rendimiento. Dados A y B, la idea es hacer la operación:

```c
C[i][j] += A[i][k] * B[j][k];
```

Esto es equivalente a la operación $AB^T$, y además gana en rendimiento ya que
evita fallos de cache de forma similar a la optimización realizada en *b*.