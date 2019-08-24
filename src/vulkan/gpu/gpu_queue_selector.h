#ifndef GPU_QUEUE_SELECTOR_H
#define GPU_QUEUE_SELECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../queue/vk_queue.h"
#include "./gpu.h"

#define GPU_QUEUE_MAX_GROUP_NAME_LENGTH 32

typedef enum gpu_queue_select_query_status {
    GPU_SELECTOR_OK,
    GPU_SELECTOR_NOT_INIT_ERROR,
    GPU_SELECTOR_NOT_ENOUGH_QUEUES_ERROR,
    GPU_SELECTOR_SUITABLE_QUEUE_NOT_FOUND_ERROR,
    GPU_SELECTOR_UNABLE_TO_RETRIEVE_QUEUE_ERROR
} gpu_queue_select_query_status;

typedef struct gpu_queue_selector_create_info {
    const gpu_info* gpu;
} gpu_queue_selector_create_info;

typedef struct gpu_queue_record {
    char group_name[GPU_QUEUE_MAX_GROUP_NAME_LENGTH];
    float priority;
} gpu_queue_record;

typedef struct gpu_queue_family_record {
    size_t queues_used;
    gpu_queue_record* queue_records;
} gpu_queue_family_record;

typedef struct gpu_queue_select_query {
    VkQueueFlags usage;
    const char queue_group_name[GPU_QUEUE_MAX_GROUP_NAME_LENGTH];
    bool support_present;
    VkSurfaceKHR surface;
    uint32_t queue_count;
    float priority;
} gpu_queue_select_query;

typedef struct gpu_queue_select_response {
    uint32_t queue_family;
    uint32_t queue_count;
    float priority;
} gpu_queue_select_response;

typedef struct gpu_queue_selector {
    const gpu_info* gpu;
    gpu_queue_family_record* records;
    size_t records_size;
} gpu_queue_selector;

void gpu_queue_selector_init_empty(gpu_queue_selector* selector);
bool gpu_queue_selector_init(gpu_queue_selector* selector, const
    gpu_queue_selector_create_info* selector_info);

uint32_t gpu_selector_get_used_queue_family_count(const gpu_queue_selector*
    selector);

bool gpu_selector_get_device_queue_create_info(const gpu_queue_selector*
    selector, VkDeviceQueueCreateInfo* queues_info);
void gpu_selector_destroy_device_queue_create_info(VkDeviceQueueCreateInfo*
    queues_info, size_t queues_info_size);

gpu_queue_select_query_status gpu_selector_select(gpu_queue_selector* selector,
    const gpu_queue_select_query* query, gpu_queue_select_response* response);
size_t gpu_selector_retrieve_queue_group(const gpu_queue_selector* selector,
    const char group_name[GPU_QUEUE_MAX_GROUP_NAME_LENGTH], vk_queue* queues);

void gpu_queue_selector_destroy(gpu_queue_selector* selector);

void create_gpu_queue_selector(const gpu_queue_selector_create_info*
    selector_info);
gpu_queue_selector* retrieve_gpu_queue_selector();
void destroy_gpu_queue_selector();

#endif
