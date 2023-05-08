#ifndef __RWLOCK_H__
#define __RWLOCK_H__

typedef struct _RWLock* rwlock_t;

rwlock_t rwlock_init();

void rwlock_destroy(rwlock_t rwlock);

void read_lock(rwlock_t rwlock);

void read_unlock(rwlock_t rwlock);

void write_lock(rwlock_t rwlock);

void write_unlock(rwlock_t rwlock);

#endif // __RWLOCK_H__
