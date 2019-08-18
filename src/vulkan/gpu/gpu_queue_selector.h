#ifndef GPU_QUEUE_SELECTOR_H
#define GPU_QUEUE_SELECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../queue/vk_queue.h"
#include "./gpu.h"

typedef enum gpu_queue_select_query_status {
    GPU_SELECTOR_OK,
    GPU_SELECTOR_NOT_INIT_ERROR,
    GPU_SELECTOR_NOT_ENOUGH_QUEUES_ERROR,
    GPU_SELECTOR_SUITABLE_QUEUE_NOT_FOUND_ERROR,
    GPU_SELECTOR_UNABLE_TO_RETRIEVE_QUEUE_ERROR
}  gpu_queue_select_query_status;

typedef struct gpu_queue_selector_create_info {
    const gpu_info* gpu;
} gpu_queue_selector_create_info;

typedef struct gpu_queue_family_record {
    size_t queues_used;
    size_t next_available_queue;
} gpu_queue_family_record;

typedef struct gpu_queue_select_query {
    VkQueueFlags usage;
    bool support_present;
    VkSurfaceKHR surface;
    uint32_t queue_count;
} gpu_queue_select_query;

typedef struct gpu_queue_selector {
    const gpu_info* gpu;
    gpu_queue_family_record* records;
    size_t records_size;
} gpu_queue_selector;

void gpu_queue_selector_init_empty(gpu_queue_selector* selector);
bool gpu_queue_selector_init(gpu_queue_selector* selector, const
    gpu_queue_selector_create_info* selector_info);

gpu_queue_select_query_status gpu_selector_select(gpu_queue_selector* selector,
    const gpu_queue_select_query* query, vk_queue* queues);

void gpu_queue_selector_destroy(gpu_queue_selector* selector);

void create_gpu_queue_selector(const gpu_queue_selector_create_info*
    selector_info);
gpu_queue_selector* retrieve_gpu_queue_selector();
void destroy_gpu_queue_selector();

#endif