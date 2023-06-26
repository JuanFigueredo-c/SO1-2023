## Ejercicio 4
### **a)**
Para medir la performance en tiempo de cada implementación usamos
el programa `/bin/time`.

* Servidor C:
- N =   200: 0.00user 0.02system 0:01.08elapsed
- N =  2000: 0.03user 0.28system 0:06.92elapsed
- N = 20000: No funciona

* Servidor Erlang:
- N =   200: 0.00user 0.05system 0:04.22elapsed
- N =  2000: 0.02user 0.41system 0:37.60elapsed
- N = 20000: 0.40user 3.81system 4:51.54elapsed

En terminos de memoria observamos mediante el comando `htop` que el servidor en
Erlang consumen menos memoria que el servidor en C.

Nota: El servidor en C no logró funcionar para 20000 clientes. Si bien logra conectarse
a todos los clientes, no pudo dar respuesta a todos ellos, quedando siempre estancado
en un número fijo que depende del equipo donde se ejecute. Por ej, en el labdcc
respondió solo a los primeros 80 clientes, y en nuestras computadoras logró dar respuesta
a los primeros 6500 y 9000 aprox. clientes respectivamente. Debido a lo variado que son
estos numeros, y a que cada uno de estos se mantuvo fijo en cada equipo, creemos
que es por falta de recursos que el servidor no funciona correctamente, especialmente
por falta de memoria, lo cual es razonable debido a lo costoso en terminos de memoria
que son los hilos de POSIX.

### **b)**
El servidor en Erlang hace un mejor uso de memoria que el servidor en C. Esto se debe
a lo liviano que son los procesos y/o hilos en Erlang. Erlang no crea un hilo del
sistema para cada hilo de ejecución que requiere, ya que corre sobre una maquina
virtual. Esta maquina virtual es la que administra y hace el scheduling de cada
hilo, dando un uso de tiempo y memoria más eficientes.

### **c)**
El servidor en Erlang logra manejar 50.000 clientes de forma correcta, mientras
que el servidor en C no lo logró. Este último ocupó todos los recursos de la
computadora al llegar aproximadamente a los 30.000 clientes, y esta deja de responder
a partir de este punto. Este no es el mismo problema mencionado anteriormente,
ya que en este caso ni siquiera se pudieron conectar todos los clientes, mientras
que para 20.000 el problema era que el servidor no daba respuesta para varios clientes.
