/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "ccHeader.h"
#include "platform/CCImage.h"


#include "base/CCData.h"
//#include "base/ccConfig.h" // CC_USE_JPEG, CC_USE_TIFF, CC_USE_WEBP

extern "C"
{
    // To resolve link error when building 32bits with Xcode 6.
    // More information please refer to the discussion in https://github.com/cocos2d/cocos2d-x/pull/6986
#if defined (__unix) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#ifndef __ENABLE_COMPATIBILITY_WITH_UNIX_2003__
#define __ENABLE_COMPATIBILITY_WITH_UNIX_2003__
#include <stdio.h>
    FILE *fopen$UNIX2003( const char *filename, const char *mode )
    {
        return fopen(filename, mode);
    }
    size_t fwrite$UNIX2003( const void *a, size_t b, size_t c, FILE *d )
    {
        return fwrite(a, b, c, d);
    }
    char *strerror$UNIX2003( int errnum )
    {
        return strerror(errnum);
    }
#endif
#endif

#if CC_USE_PNG
#include "png/png.h"
#endif //CC_USE_PNG

#if CC_USE_JPEG
#include "jpeg/jpeglib.h"
#endif // CC_USE_JPEG

}

#include "base/ccMacros.h"
#include "platform/CCCommon.h"
#include "platform/CCStdC.h"
#include "platform/CCFileUtils.h"
#include "base/CCConfiguration.h"
#include "base/ccUtils.h"
#include "base/ZipUtils.h"
#include "bimg/decode.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/CCFileUtils-android.h"
#endif

#define CC_GL_ATC_RGB_AMD                                          0x8C92
#define CC_GL_ATC_RGBA_EXPLICIT_ALPHA_AMD                          0x8C93
#define CC_GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD                      0x87EE

NS_CC_BEGIN


//////////////////////////////////////////////////////////////////////////
// Implement Image
//////////////////////////////////////////////Y////////////////////////////
bool Image::PNG_PREMULTIPLIED_ALPHA_ENABLED = false;

Image::Image()
: _unpack(false)
, _hasPremultipliedAlpha(true)
, imageContainer_(nullptr)
{

}

Image::~Image()
{
    if (imageContainer_)
    {
        bimg::imageFree(imageContainer_);
        imageContainer_ = nullptr;
    }
}

bool Image::initWithImageFile(const std::string& path)
{
    bool ret = false;
    _filePath = FileUtils::getInstance()->fullPathForFilename(path);

    Data data = FileUtils::getInstance()->getDataFromFile(_filePath);

    if (!data.isNull())
    {
        ret = initWithImageData(data.getBytes(), data.getSize());
    }

    return ret;
}

bool Image::initWithImageData(const uint8_t* data, ssize_t dataLen)
{
    bool ret = false;

    do
    {
        CC_BREAK_IF(! data || dataLen <= 0);

        uint8_t* unpackedData = nullptr;
        ssize_t unpackedLen = 0;

        //detect and unzip the compress file
        if (ZipUtils::isCCZBuffer(data, dataLen))
        {
            unpackedLen = ZipUtils::inflateCCZBuffer(data, dataLen, &unpackedData);
        }
        else if (ZipUtils::isGZipBuffer(data, dataLen))
        {
            unpackedLen = ZipUtils::inflateMemory(const_cast<uint8_t*>(data), dataLen, &unpackedData);
        }
        else
        {
            unpackedData = const_cast<uint8_t*>(data);
            unpackedLen = dataLen;
        }

        imageContainer_ = bimg::imageParse(&allocator_, unpackedData, static_cast<uint32_t>(unpackedLen));
        CCASSERT(imageContainer_, "initWithImageData parse failed.");

        if(unpackedData != data)
        {
            free(unpackedData);
        }
    } while (0);

    return ret;
}

int Image::getBitPerPixel()
{
    return imageContainer_ ? bimg::getBitsPerPixel(imageContainer_->m_format) : 0;
}

bool Image::hasAlpha()
{
    return imageContainer_ ? imageContainer_->m_hasAlpha : false;
}

bool Image::isCompressed()
{
    return imageContainer_ ? bimg::isCompressed(imageContainer_->m_format) : false;
}


bool Image::initWithRawData(const uint8_t* data, ssize_t dataLen, int width, int height, int bitsPerComponent, bool preMulti)
{
    bool ret = false;
    do
    {
        CC_BREAK_IF(0 == width || 0 == height);

        //_height   = height;
        //_width    = width;
        //_hasPremultipliedAlpha = preMulti;
        //_renderFormat = Texture2D::PixelFormat::RGBA8888;

        //// only RGBA8888 supported
        //int bytesPerComponent = 4;
        //_dataLen = height * width * bytesPerComponent;
        //_data = static_cast<unsigned char*>(malloc(_dataLen * sizeof(unsigned char)));
        //CC_BREAK_IF(! _data);
        //memcpy(_data, data, _dataLen);
        //a RT is required
        ret = true;
    } while (0);

    return ret;
}
#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
bool Image::saveToFile(const std::string& filename, bool isToRGB)
{
    //only support for Texture2D::PixelFormat::RGB888 or Texture2D::PixelFormat::RGBA8888 uncompressed data
    bimg::TextureFormat::Enum renderFormat = getRenderFormat();
    if (isCompressed() || (renderFormat != bimg::TextureFormat::RGB8 && renderFormat != bimg::TextureFormat::RGBA8))
    {
        CCLOG("cocos2d: Image: saveToFile is only support for RGB8 or RGBA8 uncompressed data for now");
        return false;
    }

    std::string fileExtension = FileUtils::getInstance()->getFileExtension(filename);

    if (fileExtension == ".png")
    {
        return saveImageToPNG(filename, isToRGB);
    }
    else if (fileExtension == ".jpg")
    {
        return saveImageToJPG(filename);
    }
    else
    {
        CCLOG("cocos2d: Image: saveToFile no support file extension(only .png or .jpg) for file: %s", filename.c_str());
        return false;
    }
}
#endif // (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)


bool Image::saveImageToPNG(const std::string& filePath, bool isToRGB)
{
#if CC_USE_WIC
    return encodeWithWIC(filePath, isToRGB, GUID_ContainerFormatPng);
#elif CC_USE_PNG
    bool ret = false;
    uint8_t* data = static_cast<uint8_t*>(imageContainer_->m_data);
    uint32_t width = imageContainer_->m_width;
    uint32_t height = imageContainer_->m_height;
    do
    {
        FILE *fp;
        png_structp png_ptr;
        png_infop info_ptr;
        png_colorp palette;
        png_bytep *row_pointers;

        fp = fopen(FileUtils::getInstance()->getSuitableFOpen(filePath).c_str(), "wb");
        CC_BREAK_IF(nullptr == fp);

        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

        if (nullptr == png_ptr)
        {
            fclose(fp);
            break;
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (nullptr == info_ptr)
        {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, nullptr);
            break;
        }
#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA && CC_TARGET_PLATFORM != CC_PLATFORM_NACL)
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            break;
        }
#endif
        png_init_io(png_ptr, fp);

        if (!isToRGB && hasAlpha())
        {
            png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        }
        else
        {
            png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        }

        palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof (png_color));
        png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);

        png_write_info(png_ptr, info_ptr);

        png_set_packing(png_ptr);

        row_pointers = (png_bytep *)malloc(height * sizeof(png_bytep));
        if(row_pointers == nullptr)
        {
            fclose(fp);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            break;
        }

        if (!hasAlpha())
        {
            for (int i = 0; i < (int)height; i++)
            {
                row_pointers[i] = (png_bytep)data + i * width * 3;
            }

            png_write_image(png_ptr, row_pointers);

            free(row_pointers);
            row_pointers = nullptr;
        }
        else
        {
            if (isToRGB)
            {
                unsigned char *tempData = static_cast<unsigned char*>(malloc(width * height * 3 * sizeof(unsigned char)));
                if (nullptr == tempData)
                {
                    fclose(fp);
                    png_destroy_write_struct(&png_ptr, &info_ptr);

                    free(row_pointers);
                    row_pointers = nullptr;
                    break;
                }

                for (uint32_t i = 0; i < height; ++i)
                {
                    for (uint32_t j = 0; j < width; ++j)
                    {
                        tempData[(i * width + j) * 3] = data[(i * width + j) * 4];
                        tempData[(i * width + j) * 3 + 1] = data[(i * width + j) * 4 + 1];
                        tempData[(i * width + j) * 3 + 2] = data[(i * width + j) * 4 + 2];
                    }
                }

                for (int i = 0; i < (int)height; i++)
                {
                    row_pointers[i] = (png_bytep)tempData + i * width * 3;
                }

                png_write_image(png_ptr, row_pointers);

                free(row_pointers);
                row_pointers = nullptr;

                if (tempData != nullptr)
                {
                    free(tempData);
                }
            }
            else
            {
                for (int i = 0; i < (int)height; i++)
                {
                    row_pointers[i] = (png_bytep)data + i * width * 4;
                }

                png_write_image(png_ptr, row_pointers);

                free(row_pointers);
                row_pointers = nullptr;
            }
        }

        png_write_end(png_ptr, info_ptr);

        png_free(png_ptr, palette);
        palette = nullptr;

        png_destroy_write_struct(&png_ptr, &info_ptr);

        fclose(fp);

        ret = true;
    } while (0);
    return ret;
#else
    CCLOG("png is not enabled, please enable it in ccConfig.h");
    return false;
#endif // CC_USE_PNG
}

bool Image::saveImageToJPG(const std::string& filePath)
{
#if CC_USE_WIC
    return encodeWithWIC(filePath, false, GUID_ContainerFormatJpeg);
#elif CC_USE_JPEG
    bool ret = false;
    do
    {
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;
        FILE * outfile;                 /* target file */
        JSAMPROW row_pointer[1];        /* pointer to JSAMPLE row[s] */
        int     row_stride;          /* physical row width in image buffer */

        cinfo.err = jpeg_std_error(&jerr);
        /* Now we can initialize the JPEG compression object. */
        jpeg_create_compress(&cinfo);

        CC_BREAK_IF((outfile = fopen(FileUtils::getInstance()->getSuitableFOpen(filePath).c_str(), "wb")) == nullptr);

        jpeg_stdio_dest(&cinfo, outfile);

        uint32_t width = imageContainer_->m_width;
        uint32_t height = imageContainer_->m_height;
        uint8_t* data = static_cast<uint8_t*>(imageContainer_->m_data);

        cinfo.image_width = width;    /* image width and height, in pixels */
        cinfo.image_height = height;
        cinfo.input_components = 3;       /* # of color components per pixel */
        cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */

        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, 90, TRUE);

        jpeg_start_compress(&cinfo, TRUE);

        row_stride = width * 3; /* JSAMPLEs per row in image_buffer */

        if (hasAlpha())
        {
            uint8_t*tempData = static_cast<uint8_t*>(malloc(width * height * 3 * sizeof(unsigned char)));
            if (nullptr == tempData)
            {
                jpeg_finish_compress(&cinfo);
                jpeg_destroy_compress(&cinfo);
                fclose(outfile);
                break;
            }

            for (uint32_t i = 0; i < height; ++i)
            {
                for (uint32_t j = 0; j < width; ++j)

                {
                    tempData[(i * width + j) * 3] = data[(i * width + j) * 4];
                    tempData[(i * width + j) * 3 + 1] = data[(i * width + j) * 4 + 1];
                    tempData[(i * width + j) * 3 + 2] = data[(i * width + j) * 4 + 2];
                }
            }

            while (cinfo.next_scanline < cinfo.image_height)
            {
                row_pointer[0] = & tempData[cinfo.next_scanline * row_stride];
                (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }

            if (tempData != nullptr)
            {
                free(tempData);
            }
        }
        else
        {
            while (cinfo.next_scanline < cinfo.image_height) {
                row_pointer[0] = & data[cinfo.next_scanline * row_stride];
                (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }
        }

        jpeg_finish_compress(&cinfo);
        fclose(outfile);
        jpeg_destroy_compress(&cinfo);

        ret = true;
    } while (0);
    return ret;
#else
    CCLOG("jpeg is not enabled, please enable it in ccConfig.h");
    return false;
#endif // CC_USE_JPEG
}

void Image::premultipliedAlpha()
{
    if (PNG_PREMULTIPLIED_ALPHA_ENABLED && getRenderFormat() == bimg::TextureFormat::RGBA8)
    {
        //this maybe dangerous, modify the imageContainer data directly
        unsigned int* fourBytes = (unsigned int*)imageContainer_->m_data;
        for(uint32_t i = 0; i < imageContainer_->m_width * imageContainer_->m_height; i++)
        {
            uint8_t* p = static_cast<uint8_t*>(imageContainer_->m_data) + i * 4;
            fourBytes[i] = CC_RGB_PREMULTIPLY_ALPHA(p[0], p[1], p[2], p[3]);
        }

        _hasPremultipliedAlpha = true;
    }
    else
    {
        _hasPremultipliedAlpha = false;
        return;
    }
}


void Image::setPVRImagesHavePremultipliedAlpha(bool haveAlphaPremultiplied)
{
    
}

NS_CC_END

