#ifndef RENDER_FUNCTION_H
#define RENDER_FUNCTION_H

#include <stdbool.h>

#include "./rendering_resource.h"

typedef bool (*render_function)(const rendering_resource* r);

#endif
