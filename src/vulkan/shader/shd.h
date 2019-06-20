#ifndef SHD_H
#define SHD_H

#include "./shader_program/shader_types.h"
#include "./loader/shader_loader.h"
#include "./manager/shader_manager.h"

void create_shader_manager(const shader_manager_create_info* shm_info);
shader_manager* retrieve_shader_manager();
void destroy_shader_manager();

#endif
