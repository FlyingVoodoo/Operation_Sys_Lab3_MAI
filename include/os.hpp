#pragma once

#include <semaphore.h>
#include <sys/types.h>
#include <cstddef>

#include "shared_data.hpp"

enum ProcessRole { IS_PARENT, IS_CHILD };

SharedData* CreateSharedMemory();
SharedData* OpenSharedMemory();
void DestroySharedMemory(SharedData* data);

ProcessRole ProcessCreate();
void ProcessExecute(const char* program, const char* arg);
pid_t ProcessWait(pid_t pid);

void SemaphorePost(sem_t* sem);
void SemaphoreWait(sem_t* sem);

char* MapFileContent(const char* filename, size_t size, int flags);