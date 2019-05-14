#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

VkInstance create_instance(SDL_Window *w);
void destroy_instance(VkInstance instance);

#endif 
