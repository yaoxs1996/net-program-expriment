#include "Cond.h"

Cond::Cond() {
    pthread_cond_init(&m_cond_var, NULL);
}
Cond::~Cond() {
    pthread_cond_destroy(&m_cond_var);
}
void Cond::wait(pthread_mutex_t* mutex) {
    pthread_cond_wait(&m_cond_var, mutex);
}
void Cond::signal() {
    pthread_cond_signal(&m_cond_var);
}
void Cond::broadcast() {
    pthread_cond_broadcast(&m_cond_var);
}