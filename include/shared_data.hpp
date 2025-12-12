#pragma once

#include <semaphore.h>

struct SharedData {
    int number;
    int signal;
    char file[256];
    sem_t* sem_parent;
    sem_t* sem_child;
    sem_t* sem_file;
};