#ifndef __READ_WRITE_LOCK_H__
#define __READ_WRITE_LOCK_H__

#include <semaphore.h>

typedef struct _RWLock *RWLock;

// Crea e inicializa una estructura del tipo RWLock
RWLock rwl_init();

// Destruye una estructura RWLock.
void rwl_destroy(RWLock rw);

// Lee concurrentemente la variable interna de la estructura.
int rwl_readers(RWLock rw);

// Implementacion para los lock de los lectores (leer implementaciones para read y write prefering)
void rwl_lock_readers(RWLock rw);

// Implementacion para los unlock de los lectores (leer implementaciones para read y write prefering)
void rwl_unlock_readers(RWLock rw);

// Implementacion para los lock de los escritores (leer implementaciones para read y write prefering)
void rwl_lock_writers(RWLock rw);

// Implementacion para los lock de los escritores (leer implementaciones para read y write prefering)
void rwl_unlock_writers(RWLock rw);

#endif
