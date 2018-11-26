#include "astcdecode.h"
#include "astc_codec_internals.h"

int print_diagnostics = 0;

void initAstcTables()
{
    static bool init = false;
    if (!init)
    {
        prepare_angular_tables();
        build_quantization_mode_table();
        init = true;
    }
}

unsigned char* decode_astc_buffer(const unsigned char* buffer, int xdim, int ydim, int zdim, int xsize, int ysize, int zsize, int xblocks, int yblocks, int zblocks, int* decodeSize)
{
    int bitness = 8;
    astc_decode_mode decode_mode = DECODE_LDR;
    swizzlepattern swz_decode = { 0, 1, 2, 3 };

    astc_codec_image *img = allocate_image(bitness, xsize, ysize, zsize, 0);
    initialize_image(img);

    int x, y, z;
    imageblock pb;
    for (z = 0; z < zblocks; z++)
    {
        for (y = 0; y < yblocks; y++)
        {
            for (x = 0; x < xblocks; x++)
            {
                int offset = (((z * yblocks + y) * xblocks) + x) * 16;
                const uint8_t *bp = buffer + offset;
                physical_compressed_block pcb = *(physical_compressed_block *)bp;
                symbolic_compressed_block scb;
                physical_to_symbolic(xdim, ydim, zdim, pcb, &scb);
                decompress_symbolic_block(decode_mode, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, &scb, &pb);
                write_imageblock(img, &pb, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, swz_decode);
            }
        }
    }
    int image_channels = determine_image_channels(img);
    int bytesperpixel = image_channels;

    uint8_t *row_pointers8 = NULL;
    uint16_t *row_pointers16 = NULL;
    if (bitness == 8)
    {
        row_pointers8 = (uint8_t*)malloc(xsize * ysize * bytesperpixel);
        *decodeSize = xsize * ysize * bytesperpixel;
        for (y = 0; y < ysize; ++y)
        {
            switch (bytesperpixel)
            {
            case 1:			// single-component, treated as Luminance
                for (x = 0; x < xsize; x++)
                {
                    row_pointers8[y * ysize * bytesperpixel + x] = img->imagedata8[0][ysize - 1 - y][4 * x];
                }
                break;
            case 2:			// two-component, treated as Luminance-Alpha
                for (x = 0; x < xsize; x++)
                {
                    row_pointers8[y * ysize * bytesperpixel + 2 * x] = img->imagedata8[0][ysize - 1 - y][4 * x];
                    row_pointers8[y * ysize * bytesperpixel + 2 * x + 1] = img->imagedata8[0][ysize - 1 - y][4 * x + 3];
                }
                break;
            case 3:			// three-component, treated as RGB
                for (x = 0; x < xsize; x++)
                {
                    row_pointers8[y * ysize * bytesperpixel + 3 * x + 2] = img->imagedata8[0][ysize - 1 - y][4 * x];
                    row_pointers8[y * ysize * bytesperpixel + 3 * x + 1] = img->imagedata8[0][ysize - 1 - y][4 * x + 1];
                    row_pointers8[y * ysize * bytesperpixel + 3 * x] = img->imagedata8[0][ysize - 1 - y][4 * x + 2];
                }
                break;
            case 4:			// three-component, treated as RGB
                for (x = 0; x < xsize; x++)
                {
                    //RGBA
                    row_pointers8[y * xsize * bytesperpixel + 4 * x] = img->imagedata8[0][ysize - 1 - y][4 * x];
                    row_pointers8[y * xsize * bytesperpixel + 4 * x + 1] = img->imagedata8[0][ysize - 1 - y][4 * x + 1];
                    row_pointers8[y * xsize * bytesperpixel + 4 * x + 2] = img->imagedata8[0][ysize - 1 - y][4 * x + 2];
                    row_pointers8[y * xsize * bytesperpixel + 4 * x + 3] = img->imagedata8[0][ysize - 1 - y][4 * x + 3];
                }
                break;
            }
        }
        return row_pointers8;
    }
    else						// if bitness == 16
    {

        row_pointers16 = (uint16_t*)malloc(xsize * ysize * bytesperpixel);

        for (y = 0; y < ysize; y++)
        {
            switch (bytesperpixel)
            {
            case 1:			// single-component, treated as Luminance
                for (x = 0; x < xsize; x++)
                {
                    row_pointers16[y * ysize * bytesperpixel + x] = img->imagedata16[0][ysize - 1 - y][4 * x];
                }
                break;
            case 2:			// two-component, treated as Luminance-Alpha
                for (x = 0; x < xsize; x++)
                {
                    row_pointers16[y * ysize * bytesperpixel + 2 * x] = img->imagedata16[0][ysize - 1 - y][4 * x];
                    row_pointers16[y * ysize * bytesperpixel + 2 * x + 1] = img->imagedata16[0][ysize - 1 - y][4 * x + 3];
                }
                break;
            case 3:			// three-component, treated as RGB
                for (x = 0; x < xsize; x++)
                {
                    row_pointers16[y * ysize * bytesperpixel + 3 * x + 2] = img->imagedata16[0][ysize - 1 - y][4 * x];
                    row_pointers16[y * ysize * bytesperpixel + 3 * x + 1] = img->imagedata16[0][ysize - 1 - y][4 * x + 1];
                    row_pointers16[y * ysize * bytesperpixel + 3 * x] = img->imagedata16[0][ysize - 1 - y][4 * x + 2];
                }
                break;
            case 4:			// three-component, treated as RGB
                for (x = 0; x < xsize; x++)
                {
                    row_pointers16[y * ysize * bytesperpixel + 4 * x] = img->imagedata16[0][ysize - 1 - y][4 * x];
                    row_pointers16[y * ysize * bytesperpixel + 4 * x + 1] = img->imagedata16[0][ysize - 1 - y][4 * x + 1];
                    row_pointers16[y * ysize * bytesperpixel + 4 * x + 2] = img->imagedata16[0][ysize - 1 - y][4 * x + 2];
                    row_pointers16[y * ysize * bytesperpixel + 4 * x + 3] = img->imagedata16[0][ysize - 1 - y][4 * x + 3];
                }
                break;
            }
        }
    }
    return NULL;
}