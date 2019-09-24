#ifndef DOMAIN_GEOM_REPOSITORY
#define DOMAIN_GEOM_REPOSITORY

#include <stdbool.h>
#include <stddef.h>

#include "../../collections/vector.h"
#include "../../geom/vertex.h"
#include "./geom_generator.h"

typedef struct geom_record {
    size_t vertex_offset;
    size_t vertex_count;
} geom_record;

typedef struct geom_record_vector VECTOR(geom_record) geom_record_vector;

typedef struct geom_repository_create_info {
    size_t expected_number_of_shapes;
    size_t max_vertex_size;
} geom_repository_create_info;

typedef struct geom_repository {
    geom_record_vector shapes;
    vertex_2f_rgba* vertices;
    size_t vertex_index;
} geom_repository;

void geom_repository_init_empty(geom_repository* repo);
bool geom_repository_init(geom_repository* repo, const
    geom_repository_create_info* repo_config);

bool geom_repository_shape_add(geom_repository* repo, const
    geom_shape_generate_info* shape_info);

#endif
