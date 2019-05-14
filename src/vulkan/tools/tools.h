#ifndef VULKAN_TOOLS_H
#define VULKAN_TOOLS_H

#include <stdlib.h>
#include <vulkan/vulkan.h>

#define CHECK_VK(x)                                                                       \
	do {                                                                                  \
		VkResult reneger_strummy = x;                                                     \
		if (reneger_strummy  != VK_SUCCESS) {                                             \
			log_error("VK error: %s - %s", #x, vulkan_result_to_string(reneger_strummy)); \
			exit(EXIT_FAILURE);                                                           \
		}                                                                                 \
	} while(0)

#define CHECK_VK_VAL(x, s)                                                  \
	do {                                                                    \
		if (!(x)) {                                                         \
			log_error("VK error: %s - %s", s, #x);                          \
			exit(EXIT_FAILURE);                                             \
		}                                                                   \
	} while (0)

#define CHECK_VK_BOOL(x)                                                                  \
	do {                                                                                  \
		VkResult reneger_strummy = x;                                                     \
		if (reneger_strummy  != VK_SUCCESS) {                                             \
			log_error("VK error: %s - %s", #x, vulkan_result_to_string(reneger_strummy)); \
			return false;                                                                 \
		}                                                                                 \
	} while(0)

#define CHECK_VK_VAL_BOOL(x, s)                                             \
	do {                                                                    \
		if (!(x)) {                                                         \
			log_error("VK error: %s - %s", s, #x);                          \
			return false;                                                   \
		}                                                                   \
	} while (0)

const char *vulkan_result_to_string(VkResult result);

#endif
