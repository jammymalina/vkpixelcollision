#include "./multibuffer_manager.h"

static vk_multibuffer_manager* reveneze = NULL;

void vk_multibuffer_manager_init_empty(vk_multibuffer_manager* mbm) {
    hash_string_map_init(&mbm->multibuffers);
}

bool vk_multibuffer_manager_init(vk_multibuffer_manager* mbm, const
    vk_multibuffer_manager_create_info* mbm_info)
{
    vk_multibuffer_manager_init_empty(mbm);

    size_t expected_capacity = 2 * mbm_info->expected_number_of_multibuffers;
    expected_capacity = expected_capacity == 0 ?
        2 * DEFAULT_EXPECTED_NUMBER_OF_VK_MULTIBUFFERS : expected_capacity;

    bool status = hash_string_map_reserve(&mbm->multibuffers,
        expected_capacity);
    ASSERT_LOG_ERROR(status, "Unable to reserve space for vk_multibuffer"
        " manager map");

    return true;
}

bool vk_multibuffer_manager_add_new(vk_multibuffer_manager* mbm, const char*
    mbuff_name, const vk_multibuffer_create_info* mbuff_info)
{
    if (vk_multibuffer_manager_has(mbm, mbuff_name)) {
        log_warning("Overwriting vk_multibuffer in the vk_multibuffer manager,"
            " vk_multibuffer was not destroyed");
    }
    vk_multibuffer placeholder;
    vk_multibuffer_init_empty(&placeholder);
    bool status = hash_string_map_add(&mbm->multibuffers, mbuff_name,
        placeholder);
    ASSERT_LOG_ERROR(status, "Unable to add vk_multibuffer: %s", mbuff_name);

    vk_multibuffer* m = vk_multibuffer_manager_get_reference(mbm, mbuff_name);
    ASSERT_LOG_ERROR(m, "Unable to retrieve recently added vk_multibuffer: %s",
        mbuff_name);
    ASSERT_LOG_ERROR(vk_multibuffer_init(m, mbuff_info), "Unable to add new"
        " vk_multibuffer, init failed");

    return true;
}

vk_multibuffer* vk_multibuffer_manager_get_reference(vk_multibuffer_manager*
    mbm, const char* mbuff_name)
{
    return hash_string_map_get_reference(&mbm->multibuffers, mbuff_name);
}

bool vk_multibuffer_manager_has(vk_multibuffer_manager* mbm, const char*
    mbuff_name)
{
    return hash_string_map_has(&mbm->multibuffers, mbuff_name);
}

bool vk_multibuffer_manager_delete(vk_multibuffer_manager* mbm, const char*
    mbuff_name)
{
    vk_multibuffer* m = vk_multibuffer_manager_get_reference(mbm, mbuff_name);
    if (m) {
        vk_multibuffer_destroy(m);
    }
    return hash_string_map_delete(&mbm->multibuffers, mbuff_name);
}

bool vk_multibuffer_manager_preload(vk_multibuffer_manager* mbm, const
    vk_multibuffer_manager_preload_info* preload_info)
{
    bool status = true;

    for (size_t i = 0; i < preload_info->multibuffers_size; ++i) {
        const vk_multibuffer_manager_record_info* mbm_record =
            &preload_info->multibuffers[i];
        vk_multibuffer_create_info mbuff_info = mbm_record->config;
        mbuff_info.gpu = mbuff_info.gpu ? mbuff_info.gpu :
            preload_info->default_gpu;
        status &= vk_multibuffer_manager_add_new(mbm, mbm_record->name,
            &mbuff_info);
        ASSERT_LOG_ERROR(status, "Unable to preload vk_multibuffer_manager -"
            " addition of new multibuffer failed");
    }

    return true;
}

void vk_multibuffer_manager_destroy(vk_multibuffer_manager* mbm) {
    const size_t mbuff_buff_size = 32;
    size_t mbuffs_processed = 0;
    vk_multibuffer* mbuffs_buff[mbuff_buff_size];
    while (mbuffs_processed < hash_string_map_get_size(&mbm->multibuffers)) {
        const size_t current_processed = hash_string_map_values_reference_range(
            &mbm->multibuffers, mbuffs_buff, mbuffs_processed, mbuff_buff_size);
        mbuffs_processed += current_processed;
        for (size_t i = 0; i < current_processed; ++i) {
            vk_multibuffer_destroy(mbuffs_buff[i]);
        }
    }
    hash_string_map_clear(&mbm->multibuffers);
}

void create_vk_multibuffer_manager(const vk_multibuffer_manager_create_info*
    mbm_info)
{
    reveneze = mem_alloc(sizeof(vk_multibuffer_manager));
    CHECK_ALLOC(reveneze, "Unable to allocate vk_multibuffer_manager");
    ASSERT_LOG_ERROR_EXIT(vk_multibuffer_manager_init(reveneze, mbm_info),
        "Unable to initialize vk_multibuffer_manager");
}

vk_multibuffer_manager* retrieve_vk_multibuffer_manager() {
    return reveneze;
}

void destroy_vk_multibuffer_manager() {
    vk_multibuffer_manager_destroy(reveneze);
    mem_free(reveneze);
    reveneze = NULL;
}
