#include <semaphore.h>

typedef struct _RWLock {
    sem_t internal_lock;
    sem_t readers_lock;
    int current_readers;
}*RWLock;

// Crea e inicializa una estructura del tipo RWLock
RWLock rwl_init();

// Destruye una estructura RWLock. Si la destruccion fue exitosa, devuelve 0
// si hubo algun error, devuelve -1;
int rwl_destroy(RWLock rw);

// Lee concurrentemente la variable interna que lleva la cuenta de los lectores actuales
int rwl_readers(RWLock rw);

// Bloquea el semaforo interno "readers_lock"
int rwl_lock_writers(RWLock rw);

int rwl_lock_readers(RWLock rw);

int rwl_unlock_readers(RWLock rw);

int rwl_unlock_writers(RWLock rw);
