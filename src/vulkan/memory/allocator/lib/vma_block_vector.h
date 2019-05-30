#ifndef VMA_BLOCK_VECTOR_H
#define VMA_BLOCK_VECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "./vma_types.h"
#include "./vma_vector.h"

typedef struct vma_block_vector_create_info {
    vma_allocator* handle_allocator;
    vma_pool* handle_parent_pool;

    uint32_t memory_type_index;
    VkDeviceSize preferred_block_size;
    size_t min_block_count;
    size_t max_block_count;
    VkDeviceSize buffer_image_granularity;

    uint32_t frame_in_use_count;
    bool is_custom_pool;
    bool explicit_block_size;
    uint32_t algorithm;
} vma_block_vector_create_info;

typedef struct vma_block_vector_alloc_create_info {
    uint32_t current_frame_index;
    VkDeviceSize size;
    VkDeviceSize alignment;
    vma_allocation_create_info* alloc_create_info;
    vma_suballocation_type* suballoc_type;
    size_t allocation_count;
    vma_allocation* allocations;
} vma_block_vector_alloc_create_info;

typedef struct vma_block_vector {
    vma_allocator* handle_allocator;
    vma_pool* handle_pool;
    uint32_t memory_type_index;
    VkDeviceSize preferred_block_size;
    size_t min_block_count;
    size_t max_block_count;
    VkDeviceSize buffer_image_granularity;
    uint32_t frame_in_use_count;
    bool is_custom_pool;
    bool explicit_block_size;
    uint32_t algorithm;

    bool has_empty_block;
    VmaVector< VmaDeviceMemoryBlock*, VmaStlAllocator<VmaDeviceMemoryBlock*> > m_Blocks;
    uint32_t next_block_id;
} vma_block_vector;

VkResult vma_block_vector_create_min_blocks(const vma_block_vector* bv);
bool vma_block_vector_is_empty(const vma_block_vector* bv);
bool vma_block_vector_get_block_count(const vma_block_vector* bv);
vma_device_memory_block* vma_block_vector_get_block(const vma_block_vector* bv);

bool vma_block_vector_is_corruption_detection_enabled(const vma_block_vector* bv);

VkResult vma_block_vector_allocate(vma_block_vector* bv, const vma_block_vector_alloc_create_info* alloc_create_info);
void vma_block_vector_free(vma_allocation* handle_allocation);

void vma_block_vector_add_stats(vma_block_vector* bv, vma_stats* stats);
void vma_block_vector_make_pool_alloc_lost(vma_block_vector* bv, uint32_t current_frame_index,
    size_t* lost_alloc_count);
VkResult vma_block_vector_check_corruption(vma_block_vector* bv);

    // Saves results in pCtx->res.
    void Defragment(
        class VmaBlockVectorDefragmentationContext* pCtx,
        VmaDefragmentationStats* pStats,
        VkDeviceSize& maxCpuBytesToMove, uint32_t& maxCpuAllocationsToMove,
        VkDeviceSize& maxGpuBytesToMove, uint32_t& maxGpuAllocationsToMove,
        VkCommandBuffer commandBuffer);
    void DefragmentationEnd(
        class VmaBlockVectorDefragmentationContext* pCtx,
        VmaDefragmentationStats* pStats);

    ////////////////////////////////////////////////////////////////////////////////
    // To be used only while the m_Mutex is locked. Used during defragmentation.

    VmaDeviceMemoryBlock* GetBlock(size_t index) const { return m_Blocks[index]; }
    size_t CalcAllocationCount() const;
    bool IsBufferImageGranularityConflictPossible() const;

private:
    friend class VmaDefragmentationAlgorithm_Generic;

    

    VkDeviceSize CalcMaxBlockSize() const;

    // Finds and removes given block from vector.
    void Remove(VmaDeviceMemoryBlock* pBlock);

    // Performs single step in sorting m_Blocks. They may not be fully sorted
    // after this call.
    void IncrementallySortBlocks();

    VkResult AllocatePage(
        uint32_t currentFrameIndex,
        VkDeviceSize size,
        VkDeviceSize alignment,
        const VmaAllocationCreateInfo& createInfo,
        VmaSuballocationType suballocType,
        VmaAllocation* pAllocation);

    // To be used only without CAN_MAKE_OTHER_LOST flag.
    VkResult AllocateFromBlock(
        VmaDeviceMemoryBlock* pBlock,
        uint32_t currentFrameIndex,
        VkDeviceSize size,
        VkDeviceSize alignment,
        VmaAllocationCreateFlags allocFlags,
        void* pUserData,
        VmaSuballocationType suballocType,
        uint32_t strategy,
        VmaAllocation* pAllocation);

    VkResult CreateBlock(VkDeviceSize blockSize, size_t* pNewBlockIndex);

    // Saves result to pCtx->res.
    void ApplyDefragmentationMovesCpu(
        class VmaBlockVectorDefragmentationContext* pDefragCtx,
        const VmaVector< VmaDefragmentationMove, VmaStlAllocator<VmaDefragmentationMove> >& moves);
    // Saves result to pCtx->res.
    void ApplyDefragmentationMovesGpu(
        class VmaBlockVectorDefragmentationContext* pDefragCtx,
        const VmaVector< VmaDefragmentationMove, VmaStlAllocator<VmaDefragmentationMove> >& moves,
        VkCommandBuffer commandBuffer);

    /*
    Used during defragmentation. pDefragmentationStats is optional. It's in/out
    - updated with new data.
    */
    void FreeEmptyBlocks(VmaDefragmentationStats* pDefragmentationStats);
};

#endif