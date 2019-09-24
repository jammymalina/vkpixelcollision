#include "./repository.h"

#include "../../logger/logger.h"

void geom_repository_init_empty(geom_repository* repo) {
    vector_init(&repo->shapes);
}

bool geom_repository_init(geom_repository* repo, const
    geom_repository_create_info* repo_config)
{
    geom_repository_init_empty(repo);
    ASSERT_LOG_ERROR(vector_reserve(&repo->shapes,
        repo_config->expected_number_of_shapes), "Unable to reserve capacity"
        " for shapes vector");

    return true;
}

bool geom_repository_shape_add(geom_repository* repo, const
    geom_shape_generate_info* shape_info)
{
    return true;
}
