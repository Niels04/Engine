#pragma once

#define RGBE_PIXEL_SIZE 4
#define RGB_PIXEL_SIZE 3

#define R 0
#define G 1
#define B 2
#define E 3

/* return codes for rgbe routines */
#define RGBE_RETURN_SUCCESS 0
#define RGBE_RETURN_FAILURE -1

namespace HDR_ImageLoader
{
	enum rgbe_error_codes {
		rgbe_read_error,
		rgbe_write_error,
		rgbe_format_error,
		rgbe_memory_error,
	};

	int rgbe_error(int rgbe_error_code, const char* msg);

	int RGBE_ReadPixels_RLE(FILE* fp, unsigned char** data, int scanline_width, int num_scanlines);

	bool load(const char* imagePath, unsigned char** data, uint32_t* out_width, uint32_t* out_height);
}