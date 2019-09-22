#ifndef DOMAIN_GEOM_REPOSITORY
#define DOMAIN_GEOM_REPOSITORY

#include "../../collections/vector.h"

typedef struct geom_record {
    size_t vertex_offset;
    size_t vertex_count;
} geom_record;

typedef struct geom_record_vector VECTOR(geom_record) geom_record_vector;

typedef struct geom_repository {
    geom_record_vector shapes;
} geom_repository;

#endif
