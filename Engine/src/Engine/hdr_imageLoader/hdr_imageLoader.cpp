#include "Engpch.hpp"
#include "hdr_imageLoader.hpp"

namespace HDR_ImageLoader
{
	int rgbe_error(int rgbe_error_code, const char* msg)
	{
		switch (rgbe_error_code) {
		case rgbe_read_error:
			perror("RGBE read error");
			break;
		case rgbe_write_error:
			perror("RGBE write error");
			break;
		case rgbe_format_error:
			fprintf(stderr, "RGBE bad file format: %s\n", msg);
			break;
		default:
		case rgbe_memory_error:
			fprintf(stderr, "RGBE error: %s\n", msg);
		}
		return RGBE_RETURN_FAILURE;
	}

	int RGBE_ReadPixels_RLE(FILE* fp, unsigned char** data, int scanline_width, int num_scanlines)
	{
		unsigned char rgbe[4], * scanline_buffer, * ptr, * ptr_end;
		int i, count;
		unsigned char buf[2];
		int height = num_scanlines;
		*data = new unsigned char[scanline_width * height * RGBE_PIXEL_SIZE];//allocate a buffer that will store the whole image in RGBE format

		if ((scanline_width < 8) || (scanline_width > 0x7fff))
			/* run length encoding is not allowed so output error*/
			return rgbe_error(rgbe_format_error, "Texture too small or too big.");
		scanline_buffer = NULL;
		/* read in each successive scanline */
		while (num_scanlines > 0) {
			if (fread(rgbe, sizeof(rgbe), 1, fp) < 1) {
				free(scanline_buffer);
				return rgbe_error(rgbe_read_error, NULL);
			}
			if ((rgbe[0] != 2) || (rgbe[1] != 2) || (rgbe[2] & 0x80)) {
				/* this file is not run length encoded so output error*/
				return rgbe_error(rgbe_format_error, "The file has to be run length encoded.");
			}
			int testval = ((int)rgbe[2]) << 8 | rgbe[3];
			if ((((int)rgbe[2]) << 8 | rgbe[3]) != scanline_width) {
				free(scanline_buffer);
				return rgbe_error(rgbe_format_error, "wrong scanline width");
			}
			if (scanline_buffer == NULL)
				scanline_buffer = (unsigned char*)
				malloc(sizeof(unsigned char) * 4 * scanline_width);
			if (scanline_buffer == NULL)
				return rgbe_error(rgbe_memory_error, "unable to allocate buffer space");

			ptr = &scanline_buffer[0];
			/* read each of the four channels for the scanline into the buffer */
			for (i = 0; i < 4; i++) {
				ptr_end = &scanline_buffer[(i + 1) * scanline_width];
				while (ptr < ptr_end) {
					if (fread(buf, sizeof(buf[0]) * 2, 1, fp) < 1) {
						free(scanline_buffer);
						return rgbe_error(rgbe_read_error, NULL);
					}
					if (buf[0] > 128) {
						/* a run of the same value */
						count = buf[0] - 128;
						if ((count == 0) || (count > ptr_end - ptr)) {
							free(scanline_buffer);
							return rgbe_error(rgbe_format_error, "bad scanline data");
						}
						while (count-- > 0)
							*ptr++ = buf[1];
					}
					else {
						/* a non-run */
						count = buf[0];
						if ((count == 0) || (count > ptr_end - ptr)) {
							free(scanline_buffer);
							return rgbe_error(rgbe_format_error, "bad scanline data");
						}
						*ptr++ = buf[1];
						if (--count > 0) {
							if (fread(ptr, sizeof(*ptr) * count, 1, fp) < 1) {
								free(scanline_buffer);
								return rgbe_error(rgbe_read_error, NULL);
							}
							ptr += count;
						}
					}
				}
			}
			//now write everything to the data-buffer
			for (i = 0; i < scanline_width; i++) {
				uint8_t pixel[4];
				pixel[0] = scanline_buffer[i];
				pixel[1] = scanline_buffer[i + scanline_width];
				pixel[2] = scanline_buffer[i + 2 * scanline_width];
				pixel[3] = scanline_buffer[i + 3 * scanline_width];
				uint8_t* pixel_out = *data + i * RGBE_PIXEL_SIZE + RGBE_PIXEL_SIZE * scanline_width * (height - num_scanlines);
				pixel_out[R] = pixel[R];
				pixel_out[G] = pixel[G];
				pixel_out[B] = pixel[B];
				pixel_out[E] = pixel[E];
			}
			num_scanlines--;
		}
		free(scanline_buffer);
		return RGBE_RETURN_SUCCESS;
	}

	bool load(const char* imagePath, unsigned char** data, uint32_t* out_width, uint32_t* out_height)
	{
		std::ifstream input;
		input.open(imagePath, std::ios::binary);
		_ASSERT(input.is_open());

		std::string line;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t pixelCount;
		while (std::getline(input, line))
		{
			if (!memcmp(line.c_str(), "-Y", 2))//returns 0 if the two bytes of memory are equal
			{
				uint32_t begin = 3;
				uint32_t end = line.find(' ', begin);
				width = std::stoi(line.substr(begin, end - begin));

				begin = end + 4;
				end = line.size();
				height = std::stoi(line.substr(begin, end - begin));
				pixelCount = width * height;
				break;
			}
		}
		if (!(width & height))//if eighter width or height are 0
			return false;
		*out_width = width; *out_height = height;
		uint32_t pos = input.tellg();
		input.close();

		FILE* file;
		fopen_s(&file, imagePath, "rb");
		if (!file)
			return false;
		fseek(file, pos, SEEK_SET);//set the stream to the position, where it can start reading pixels
		if (RGBE_ReadPixels_RLE(file, data, width, height) == RGBE_RETURN_FAILURE)
			return false;
		else
			return true;
	}
}