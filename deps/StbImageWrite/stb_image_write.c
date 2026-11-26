#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#define STBI_WRITE_NO_STDIO
#include <stb_image_write.h>

unsigned char* stb_image_write_png(const unsigned char* data, int width, int height, int* length) {
	return stbi_write_png_to_mem(data, 0, width, height, 4, length);
}

void stb_image_write_free(void* data) {
	STBIW_FREE(data);
}
