#pragma once

void initAstcTables();

unsigned char* decode_astc_buffer(const unsigned char* buffer, int xdim, int ydim, int zdim, int xsize, int ysize, int zsize, int xblocks, int yblocks, int zblocks, int* decodeSize);
