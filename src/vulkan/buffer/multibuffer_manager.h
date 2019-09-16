#ifndef VULKAN_MULTIBUFFER_MANAGER_H
#define VULKAN_MULTIBUFFER_MANAGER_H

#include <stdbool.h>
#include <stddef.h>

#include "../../collections/hash_string_map.h"
#include "./multibuffer.h"

#define DEFAULT_EXPECTED_NUMBER_OF_VK_MULTIBUFFERS 10

typedef struct vk_multibuffer_hash_map HASH_STRING_MAP(vk_multibuffer)
    vk_multibuffer_hash_map;

typedef struct vk_multibuffer_manager_create_info {
    size_t expected_number_of_multibuffers;
} vk_multibuffer_manager_create_info;

typedef struct vk_multibuffer_manager_record_info {
    const char* name;
    vk_multibuffer_create_info config;
} vk_multibuffer_manager_record_info;

typedef struct vk_multibuffer_manager_preload_info {
    const gpu_info* default_gpu;
    const vk_multibuffer_manager_record_info* multibuffers;
    size_t multibuffers_size;
} vk_multibuffer_manager_preload_info;

typedef struct vk_multibuffer_manager {
    vk_multibuffer_hash_map multibuffers;
} vk_multibuffer_manager;

void vk_multibuffer_manager_init_empty(vk_multibuffer_manager* mbm);
bool vk_multibuffer_manager_init(vk_multibuffer_manager* mbm, const
    vk_multibuffer_manager_create_info* mbm_info);

bool vk_multibuffer_manager_add_new(vk_multibuffer_manager* mbm, const char*
    mbuff_name, const vk_multibuffer_create_info* mbuff_info);
vk_multibuffer* vk_multibuffer_manager_get_reference(vk_multibuffer_manager*
    mbm, const char* mbuff_name);

bool vk_multibuffer_manager_has(vk_multibuffer_manager* mbm, const char*
    mbuff_name);
bool vk_multibuffer_manager_delete(vk_multibuffer_manager* mbm, const char*
    mbuff_name);

bool vk_multibuffer_manager_preload(vk_multibuffer_manager* mbm, const
    vk_multibuffer_manager_preload_info* preload_info);

void vk_multibuffer_manager_destroy(vk_multibuffer_manager* mbm);

void create_vk_multibuffer_manager(const vk_multibuffer_manager_create_info*
    mbm_info);
vk_multibuffer_manager* retrieve_vk_multibuffer_manager();
void destroy_vk_multibuffer_manager();

#endif
