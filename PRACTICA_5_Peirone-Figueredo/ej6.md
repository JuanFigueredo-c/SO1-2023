## Ejercicio 6
# a)

Entre las diferencias de las implementaciones entre MPI y Erlang notamos lo siguiente:
* La implementación en Erlang usa recursión para las multiples etapas de la suma, mientras
  que en MPI hacemos iteraciones con un ciclo ´for´.
* En MPI, podemos identificar a cada proceso mediante un numero, su rango, lo que hace
  sencillo el saber quien es el siguiente proceso en el anillo. En Erlang no sucede esto,
  y para solucionarlo lo que hicimos fue enviarle a cada proceso el PID correspondiente del
  proceso que le sigue.

Entre las similitudes destacamos:
* Tanto MPI como Erlang son bloqueantes por defecto para recibir, esto es una ventaja porque
  nos evita tener que hacer busy waiting o algun otro mecanismo de espera aparte
* Ambos garantizan que el mensaje se leerá entero, aunque pueda ser enviado en partes. Esto
  nos evitará tener que parsear los datos que llegan.

# b)
En cuanto a performance notamos que la implementación en Erlang es mas rapida
que la versión implementada con MPI. Para medir el tiempo en Erlang, se usó la
función `timer:tc`.

Consideramos que la versión en MPI es mas robusta, ya que al probar, hemos visto
que cuando un proceso muere en MPI, el sistema completo se detiene avisandonos
de que un proceso ha fallado. Por el otro lado, en Erlang, si uno de los procesos
del anillo muere, los demás procesos no se enteran y quedarán a la espera de su
respuesta, lo cual termina bloqueando el anillo de procesos entero.
