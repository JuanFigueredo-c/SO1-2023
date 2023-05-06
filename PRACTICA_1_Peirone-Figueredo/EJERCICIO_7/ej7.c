#include <signal.h>
#include <stdio.h>
#include <setjmp.h>

int denom = 0;

jmp_buf env;

void handler(int s) {
  printf("Ouch!\n");
  denom = 1;
  longjmp(env, 1);
}

int main() {
  int r;
  signal(SIGFPE, handler);
  (void) setjmp(env);
  /* El problema si no se usa longjmp es que denom se carga
   * en un registro, y si bien es modificado en memoria a 1,
   * en el registro aun conserva su valor 0, y por eso cuando
   * handler retorna, la division se ejecuta desde el valor de
   * denom cargado en el registro y se produce de nuevo una sigfpe.
   * El usar longjmp nos permite retornar previo a la instruccion de
   * cargado de denom a registro, evitando la excepcion.
   * */
  r = 1 / denom;
  printf("r = %d\n", r);
  return 0;
}

