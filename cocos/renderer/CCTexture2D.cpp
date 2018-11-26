/****************************************************************************
Copyright (c) 2008      Apple Inc. All Rights Reserved.
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



/*
* Support for RGBA_4_4_4_4 and RGBA_5_5_5_1 was copied from:
* https://devforums.apple.com/message/37855#37855 by a1studmuffin
*/

#include "ccHeader.h"
#include "renderer/CCTexture2D.h"

#include "platform/CCGL.h"
#include "platform/CCImage.h"
#include "base/ccUtils.h"
#include "platform/CCDevice.h"
#include "base/ccConfig.h"
#include "base/ccUTF8.h"
#include "base/CCConfiguration.h"
#include "platform/CCPlatformMacros.h"
#include "base/CCDirector.h"
#include "base/CCNinePatchImageParser.h"
#include "bimg/decode.h"

/* ASTC texture compression internal formats. */
#ifndef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR            (0x93B0)
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR            (0x93B1)
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR            (0x93B2)
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR            (0x93B3)
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR            (0x93B4)
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR            (0x93B5)
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR            (0x93B6)
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR            (0x93B7)
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR           (0x93B8)
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR           (0x93B9)
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR           (0x93BA)
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR          (0x93BB)
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR          (0x93BC)
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR          (0x93BD)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR    (0x93D0)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR    (0x93D1)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR    (0x93D2)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR    (0x93D3)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR    (0x93D4)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR    (0x93D5)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR    (0x93D6)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR    (0x93D7)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR   (0x93D8)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR   (0x93D9)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR   (0x93DA)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR  (0x93DB)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR  (0x93DC)
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR  (0x93DD)
#endif

#if CC_ENABLE_CACHE_TEXTURE_DATA
    #include "renderer/CCTextureCache.h"
#endif

#include <unordered_set>

NS_CC_BEGIN

namespace {
    typedef Texture2D::PixelFormatInfoMap::value_type PixelFormatInfoMapValue;
    static const PixelFormatInfoMapValue TexturePixelFormatInfoTablesValue[] =
    {
        PixelFormatInfoMapValue(Texture2D::PixelFormat::BGRA8888, Texture2D::PixelFormatInfo(GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE, 32, false, true)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::RGBA8888, Texture2D::PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 32, false, true)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::RGBA4444, Texture2D::PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, 16, false, true)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::RGB5A1, Texture2D::PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, 16, false, true)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::RGB565, Texture2D::PixelFormatInfo(GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 16, false, false)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::RGB888, Texture2D::PixelFormatInfo(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 24, false, false)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::A8, Texture2D::PixelFormatInfo(GL_RED, GL_RED, GL_UNSIGNED_BYTE, 8, false, false)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::I8, Texture2D::PixelFormatInfo(GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, 8, false, false)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::AI88, Texture2D::PixelFormatInfo(GL_RG, GL_RG, GL_UNSIGNED_BYTE, 16, false, true)),
		PixelFormatInfoMapValue(Texture2D::PixelFormat::ETC2_RGB, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGB8_ETC2, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
		PixelFormatInfoMapValue(Texture2D::PixelFormat::ETC2_RGBA, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA8_ETC2_EAC, 0xFFFFFFFF, 0xFFFFFFFF, 8, true, true)),
#ifdef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
        PixelFormatInfoMapValue(Texture2D::PixelFormat::PVRTC2, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 2, true, false)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::PVRTC2A, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 2, true, true)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::PVRTC4, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::PVRTC4A, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, true)),
#endif
#ifdef GL_KHR_texture_compression_astc_ldr
        PixelFormatInfoMapValue(Texture2D::PixelFormat::ASTC_RGBA_4x4, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, 0xFFFFFFFF, 0xFFFFFFFF, 8, true, true)),
        PixelFormatInfoMapValue(Texture2D::PixelFormat::ASTC_RGBA_8x8, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_ASTC_8x8_KHR, 0xFFFFFFFF, 0xFFFFFFFF, 2, true, true)),
#endif
#ifdef GL_ETC1_RGB8_OES
        PixelFormatInfoMapValue(Texture2D::PixelFormat::ETC, Texture2D::PixelFormatInfo(GL_ETC1_RGB8_OES, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
        PixelFormatInfoMapValue(Texture2D::PixelFormat::S3TC_DXT1, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
        PixelFormatInfoMapValue(Texture2D::PixelFormat::S3TC_DXT3, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
        PixelFormatInfoMapValue(Texture2D::PixelFormat::S3TC_DXT5, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_ATC_RGB_AMD
        PixelFormatInfoMapValue(Texture2D::PixelFormat::ATC_RGB, Texture2D::PixelFormatInfo(GL_ATC_RGB_AMD,
            0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_ATC_RGBA_EXPLICIT_ALPHA_AMD
        PixelFormatInfoMapValue(Texture2D::PixelFormat::ATC_EXPLICIT_ALPHA, Texture2D::PixelFormatInfo(GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,
            0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD
        PixelFormatInfoMapValue(Texture2D::PixelFormat::ATC_INTERPOLATED_ALPHA, Texture2D::PixelFormatInfo(GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,
            0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif
    };
}

//CLASS IMPLEMENTATIONS:

//The PixpelFormat corresponding information
const Texture2D::PixelFormatInfoMap Texture2D::_pixelFormatInfoTables(TexturePixelFormatInfoTablesValue,
                                                                     TexturePixelFormatInfoTablesValue + sizeof(TexturePixelFormatInfoTablesValue) / sizeof(TexturePixelFormatInfoTablesValue[0]));

// If the image has alpha, you can create RGBA8 (32-bit) or RGBA4 (16-bit) or RGB5A1 (16-bit)
// Default is: RGBA8888 (32-bit textures)
static bgfx::TextureFormat::Enum g_defaultAlphaPixelFormat = bgfx::TextureFormat::Unknown;

//////////////////////////////////////////////////////////////////////////
static std::unordered_set<Texture2D*> s_allGLTexture2D;

void Texture2D::forceDeleteALLTexture2D()
{
    auto copyMap = s_allGLTexture2D;
    for (auto&& it: copyMap) {
        delete it;
    }
    s_allGLTexture2D.clear();
}

Texture2D::Texture2D()
: _pixelFormat(Texture2D::PixelFormat::DEFAULT)
, _pixelsWide(0)
, _pixelsHigh(0)
, _name(0)
, _maxS(0.0)
, _maxT(0.0)
, _hasPremultipliedAlpha(false)
, _hasMipmaps(false)
, _antialiasEnabled(true)
, _ninePatchInfo(nullptr)
, _valid(true)
, _alphaTexture(nullptr)
, handle_(BGFX_INVALID_HANDLE)
, flags_(0)
{
    /*s_allGLTexture2D.insert(this);*/
    _antialiasEnabled = SharedDirector.getOpenGLView()->isAntiAliasEnabled();
    memset(&info_, 0, sizeof(info_));
    info_.format = bgfx::TextureFormat::Unknown;
}

Texture2D::Texture2D(bgfx::TextureHandle handle, const bgfx::TextureInfo& info, uint64_t flags)
    :handle_(handle)
    ,info_(info)
    ,flags_(flags)
{

}

bool Texture2D::initOnPlace(bgfx::TextureHandle handle, const bgfx::TextureInfo& info, uint64_t flags)
{
    handle_ = handle;
    info_ = info;
    flags_ = flags;
    return true;
}

bgfx::TextureHandle Texture2D::getHandle() const 
{
    return handle_;
}

uint32_t Texture2D::getFlags() const
{
    return flags_;
}

Texture2D::~Texture2D()
{
    /*if (s_allGLTexture2D.find(this) != s_allGLTexture2D.end()) {
        s_allGLTexture2D.erase(this);
    }
#if CC_ENABLE_CACHE_TEXTURE_DATA
    VolatileTextureMgr::removeTexture(this);
#endif

    CCLOGINFO("deallocing Texture2D: %p - id=%u", this, _name);
    CC_SAFE_RELEASE(_shaderProgram);

    CC_SAFE_DELETE(_ninePatchInfo);

    if(_name)
    {
        GL::deleteTexture(_name);
    }*/
    if (bgfx::isValid(handle_))
    {
        bgfx::destroy(handle_);
        handle_ = BGFX_INVALID_HANDLE;
    }
}

void Texture2D::releaseGLTexture()
{
    _name = 0;
}

Texture2D::PixelFormat Texture2D::getPixelFormat() const
{
    return _pixelFormat;
}

int Texture2D::getPixelsWide() const
{
    return info_.width;
}

int Texture2D::getPixelsHigh() const
{
    return info_.height;
}

GLuint Texture2D::getName() const
{
    return _name;
}

GLuint Texture2D::getAlphaTextureName() const
{
    return _alphaTexture == nullptr ? 0 : _alphaTexture->getName();
}

Size Texture2D::getContentSize() const
{
    Size ret;
    ret.width = info_.width / CC_CONTENT_SCALE_FACTOR();
    ret.height = info_.height / CC_CONTENT_SCALE_FACTOR();

    return ret;
}

const Size& Texture2D::getContentSizeInPixels()
{
    return _contentSize;
}

GLfloat Texture2D::getMaxS() const
{
    return _maxS;
}

void Texture2D::setMaxS(GLfloat maxS)
{
    _maxS = maxS;
}

GLfloat Texture2D::getMaxT() const
{
    return _maxT;
}

void Texture2D::setMaxT(GLfloat maxT)
{
    _maxT = maxT;
}

bool Texture2D::hasPremultipliedAlpha() const
{
    return _hasPremultipliedAlpha;
}

static void releaseImage(void* ptr, void* userData)
{
    CC_UNUSED_PARAM(ptr);
    bimg::ImageContainer* imageContainer = static_cast<bimg::ImageContainer*>(userData);
    bimg::imageFree(imageContainer);
}

bool Texture2D::initWithData(const void *data, ssize_t dataLen, bgfx::TextureFormat::Enum textureFormat, int pixelsWide, int pixelsHigh, const Size& contentSize)
{
    CCASSERT(data && dataLen > 0, "Invalid data");
    //const bgfx::Memory* mem = bgfx::makeRef(data, dataLen);
    //no mem, should be mutable
    handle_ = bgfx::createTexture2D(
        pixelsWide, pixelsHigh,
        false, 1, textureFormat,
        flags_
    );
    bgfx::calcTextureSize(info_,
        pixelsWide, pixelsHigh,
        1, false, false, 1,
        textureFormat);

    return true;
    ////if data has no mipmaps, we will consider it has only one mipmap
    //MipmapInfo mipmap;
    //mipmap.address = (unsigned char*)data;
    //mipmap.len = static_cast<int>(dataLen);
    //return initWithMipmaps(&mipmap, 1, pixelFormat, pixelsWide, pixelsHigh);
}

bool Texture2D::initWithDataCopy(const void *data, ssize_t dataLen, bgfx::TextureFormat::Enum textureFormat, int pixelsWide, int pixelsHigh, const Size& contentSize)
{
    CCASSERT(data && dataLen > 0, "Invalid data");
    const bgfx::Memory* mem = bgfx::copy(data, dataLen);
    handle_ = bgfx::createTexture2D(
        pixelsWide, pixelsHigh,
        false, 1, textureFormat,
        flags_, mem
    );
    bgfx::calcTextureSize(info_,
        pixelsWide, pixelsHigh,
        1, false, false, 1,
        textureFormat);

    return true;
}

bool Texture2D::initWithMipmaps(MipmapInfo* mipmaps, int mipmapsNum, PixelFormat pixelFormat, int pixelsWide, int pixelsHigh)
{
    //the pixelFormat must be a certain value
    CCASSERT(pixelFormat != PixelFormat::NONE && pixelFormat != PixelFormat::AUTO, "the \"pixelFormat\" param must be a certain value!");
    CCASSERT(pixelsWide>0 && pixelsHigh>0, "Invalid size");

    if (mipmapsNum <= 0)
    {
        CCLOG("cocos2d: WARNING: mipmap number is less than 1");
        return false;
    }


    if(_pixelFormatInfoTables.find(pixelFormat) == _pixelFormatInfoTables.end())
    {
        CCLOG("cocos2d: WARNING: unsupported pixelformat: %lx", (unsigned long)pixelFormat );
        return false;
    }

    const PixelFormatInfo& info = _pixelFormatInfoTables.at(pixelFormat);

	if (info.compressed && !Configuration::getInstance()->supportsPVRTC()
		&& !Configuration::getInstance()->supportsETC()
		&& !Configuration::getInstance()->supportsETC2()
        && !Configuration::getInstance()->supportsASTC())
//                        && !Configuration::getInstance()->supportsS3TC()
//                        && !Configuration::getInstance()->supportsATITC())
    {
        CCLOG("cocos2d: WARNING: PVRTC/ETC/ETC2 images are not supported");
        return false;
    }

    //Set the row align only when mipmapsNum == 1 and the data is uncompressed
    return true;
}

bool Texture2D::updateWithData(const void *data,int offsetX,int offsetY,int width,int height)
{
    if (bgfx::isValid(handle_))
    {
        uint32_t bytes = info_.bitsPerPixel / 8;
        bgfx::updateTexture2D(handle_, 0, 0, offsetX, offsetY, width, height, bgfx::copy(data, width * height * bytes));
        return true;
    }
    return false;
}

std::string Texture2D::getDescription() const
{
    return StringUtils::format("<Texture2D | Name = %u | Dimensions = %ld x %ld | Coordinates = (%.2f, %.2f)>", _name, (long)_pixelsWide, (long)_pixelsHigh, _maxS, _maxT);
}

// implementation Texture2D (Image)
bool Texture2D::initWithImage(Image *image)
{
    return initWithImage(image, g_defaultAlphaPixelFormat);
}

bool Texture2D::initWithImage(Image *image, bgfx::TextureFormat::Enum format)
{
    CCASSERT(false, "initWithImage");
    return false;
}

// implementation Texture2D (Text)
bool Texture2D::initWithString(const std::string& text, const std::string& fontName, float fontSize, const Size& dimensions/* = Size(0, 0)*/, TextHAlignment hAlignment/* =  TextHAlignment::CENTER */, TextVAlignment vAlignment/* =  TextVAlignment::TOP */, bool enableWrap /* = false */, int overflow /* = 0 */)
{
    FontDefinition tempDef;

    tempDef._shadow._shadowEnabled = false;
    tempDef._stroke._strokeEnabled = false;


    tempDef._fontName      = fontName;
    tempDef._fontSize      = fontSize;
    tempDef._dimensions    = dimensions;
    tempDef._alignment     = hAlignment;
    tempDef._vertAlignment = vAlignment;
    tempDef._fontFillColor = Color3B::WHITE;
    tempDef._enableWrap    = enableWrap;
    tempDef._overflow      = overflow;

    return initWithString(text, tempDef);
}

bool Texture2D::initWithString(const std::string& text, const FontDefinition& textDefinition)
{
    if(text.empty())
    {
        return false;
    }

#if CC_ENABLE_CACHE_TEXTURE_DATA
    // cache the texture data
    VolatileTextureMgr::addStringTexture(this, text, textDefinition);
#endif

    bool ret = false;
    Device::TextAlign align;

    if (TextVAlignment::TOP == textDefinition._vertAlignment)
    {
        align = (TextHAlignment::CENTER == textDefinition._alignment) ? Device::TextAlign::TOP
        : (TextHAlignment::LEFT == textDefinition._alignment) ? Device::TextAlign::TOP_LEFT : Device::TextAlign::TOP_RIGHT;
    }
    else if (TextVAlignment::CENTER == textDefinition._vertAlignment)
    {
        align = (TextHAlignment::CENTER == textDefinition._alignment) ? Device::TextAlign::CENTER
        : (TextHAlignment::LEFT == textDefinition._alignment) ? Device::TextAlign::LEFT : Device::TextAlign::RIGHT;
    }
    else if (TextVAlignment::BOTTOM == textDefinition._vertAlignment)
    {
        align = (TextHAlignment::CENTER == textDefinition._alignment) ? Device::TextAlign::BOTTOM
        : (TextHAlignment::LEFT == textDefinition._alignment) ? Device::TextAlign::BOTTOM_LEFT : Device::TextAlign::BOTTOM_RIGHT;
    }
    else
    {
        CCASSERT(false, "Not supported alignment format!");
        return false;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    CCASSERT(textDefinition._stroke._strokeEnabled == false, "Currently stroke doesn't support win32!");
#endif

    bgfx::TextureFormat::Enum pixelFormat = g_defaultAlphaPixelFormat;
    unsigned char* outTempData = nullptr;
    ssize_t outTempDataLen = 0;

    int imageWidth;
    int imageHeight;
    auto textDef = textDefinition;
    auto contentScaleFactor = CC_CONTENT_SCALE_FACTOR();
    textDef._fontSize *= contentScaleFactor;
    textDef._dimensions.width *= contentScaleFactor;
    textDef._dimensions.height *= contentScaleFactor;
    textDef._stroke._strokeSize *= contentScaleFactor;
    textDef._shadow._shadowEnabled = false;

    bool hasPremultipliedAlpha;
    Data outData = Device::getTextureDataForText(text.c_str(), textDef, align, imageWidth, imageHeight, hasPremultipliedAlpha);
    if(outData.isNull())
    {
        return false;
    }

    Size  imageSize = Size((float)imageWidth, (float)imageHeight);
    //pixelFormat = convertDataToFormat(outData.getBytes(), imageWidth*imageHeight*4, PixelFormat::RGBA8888, pixelFormat, &outTempData, &outTempDataLen);
    outTempData = outData.getBytes();
    outTempDataLen = imageWidth * imageHeight * 4;
    ret = initWithDataCopy(outTempData, outTempDataLen, pixelFormat, imageWidth, imageHeight, imageSize);

    //if (outTempData != nullptr && outTempData != outData.getBytes())
    //{
    //    free(outTempData);
    //}
    _hasPremultipliedAlpha = hasPremultipliedAlpha;

    return ret;
}


// implementation Texture2D (Drawing)

void Texture2D::drawAtPoint(const Vec2& point)
{
    GLfloat    coordinates[] = {
        0.0f,    _maxT,
        _maxS,_maxT,
        0.0f,    0.0f,
        _maxS,0.0f };

    GLfloat    width = (GLfloat)_pixelsWide * _maxS,
        height = (GLfloat)_pixelsHigh * _maxT;

    GLfloat        vertices[] = {
        point.x,            point.y,
        width + point.x,    point.y,
        point.x,            height  + point.y,
        width + point.x,    height  + point.y };

    /*GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORD );
    _shaderProgram->use();
    _shaderProgram->setUniformsForBuiltins();

    GL::bindTexture2D( _name );

    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, coordinates);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/
}

void Texture2D::drawInRect(const Rect& rect)
{
    GLfloat    coordinates[] = {
        0.0f,    _maxT,
        _maxS,_maxT,
        0.0f,    0.0f,
        _maxS,0.0f };

    GLfloat    vertices[] = {    rect.origin.x,        rect.origin.y,                            /*0.0f,*/
        rect.origin.x + rect.size.width,        rect.origin.y,                            /*0.0f,*/
        rect.origin.x,                            rect.origin.y + rect.size.height,        /*0.0f,*/
        rect.origin.x + rect.size.width,        rect.origin.y + rect.size.height,        /*0.0f*/ };

    /*GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_TEX_COORD );
    _shaderProgram->use();
    _shaderProgram->setUniformsForBuiltins();

    GL::bindTexture2D( _name );

    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, coordinates);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/
}

//
// Use to apply MIN/MAG filter
//
// implementation Texture2D (GLFilter)

void Texture2D::generateMipmap()
{
    /*CCASSERT(_pixelsWide == ccNextPOT(_pixelsWide) && _pixelsHigh == ccNextPOT(_pixelsHigh), "Mipmap texture only works in POT textures");
    GL::bindTexture2D( _name );
    glGenerateMipmap(GL_TEXTURE_2D);
    _hasMipmaps = true;
#if CC_ENABLE_CACHE_TEXTURE_DATA
    VolatileTextureMgr::setHasMipmaps(this, _hasMipmaps);
#endif*/
}

bool Texture2D::hasMipmaps() const
{
    return _hasMipmaps;
}

void Texture2D::setTexParameters(const TexParams &texParams)
{
    flags_ |= texParams.minFilter | texParams.magFilter | texParams.wrapS | texParams.wrapT;

#if CC_ENABLE_CACHE_TEXTURE_DATA
    VolatileTextureMgr::setTexParameters(this, texParams);
#endif
}

void Texture2D::setAliasTexParameters()
{
    if (! _antialiasEnabled)
    {
        return;
    }

    _antialiasEnabled = false;

    if (_name == 0)
    {
        return;
    }

    /*GL::bindTexture2D( _name );

    if( ! _hasMipmaps )
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    }
    else
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
#if CC_ENABLE_CACHE_TEXTURE_DATA
    TexParams texParams = {(GLuint)(_hasMipmaps?GL_NEAREST_MIPMAP_NEAREST:GL_NEAREST),GL_NEAREST,GL_NONE,GL_NONE};
    VolatileTextureMgr::setTexParameters(this, texParams);
#endif*/
}

void Texture2D::setAntiAliasTexParameters()
{
    if ( _antialiasEnabled )
    {
        return;
    }

    _antialiasEnabled = true;

    if (_name == 0)
    {
        return;
    }

    /*GL::bindTexture2D( _name );

    if( ! _hasMipmaps )
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    }
    else
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
#if CC_ENABLE_CACHE_TEXTURE_DATA
    TexParams texParams = {(GLuint)(_hasMipmaps?GL_LINEAR_MIPMAP_NEAREST:GL_LINEAR),GL_LINEAR,GL_NONE,GL_NONE};
    VolatileTextureMgr::setTexParameters(this, texParams);
#endif*/
}

const char* Texture2D::getStringForFormat() const
{
    switch (_pixelFormat)
    {
        case Texture2D::PixelFormat::RGBA8888:
            return  "RGBA8888";

        case Texture2D::PixelFormat::RGB888:
            return  "RGB888";

        case Texture2D::PixelFormat::RGB565:
            return  "RGB565";

        case Texture2D::PixelFormat::RGBA4444:
            return  "RGBA4444";

        case Texture2D::PixelFormat::RGB5A1:
            return  "RGB5A1";

        case Texture2D::PixelFormat::AI88:
            return  "AI88";

        case Texture2D::PixelFormat::A8:
            return  "A8";

        case Texture2D::PixelFormat::I8:
            return  "I8";

        case Texture2D::PixelFormat::PVRTC4:
            return  "PVRTC4";

        case Texture2D::PixelFormat::PVRTC2:
            return  "PVRTC2";

		case Texture2D::PixelFormat::ETC2_RGB:
			return "ETC2_RGB";

		case Texture2D::PixelFormat::ETC2_RGBA:
			return "ETC2_RGBA";

        default:
            CCASSERT(false , "unrecognized pixel format");
            CCLOG("stringForFormat: %ld, cannot give useful result", (long)_pixelFormat);
            break;
    }

    return  nullptr;
}

//
// Texture options for images that contains alpha
//
// implementation Texture2D (PixelFormat)

void Texture2D::setDefaultAlphaPixelFormat(bgfx::TextureFormat::Enum format)
{
    g_defaultAlphaPixelFormat = format;
}

bgfx::TextureFormat::Enum Texture2D::getDefaultAlphaPixelFormat()
{
    return g_defaultAlphaPixelFormat;
}

unsigned int Texture2D::getBitsPerPixelForFormat(Texture2D::PixelFormat format) const
{
    if (format == PixelFormat::NONE || format == PixelFormat::DEFAULT)
    {
        return 0;
    }

    return _pixelFormatInfoTables.at(format).bpp;
}

unsigned int Texture2D::getBitsPerPixelForFormat() const
{
    return this->getBitsPerPixelForFormat(_pixelFormat);
}

const Texture2D::PixelFormatInfoMap& Texture2D::getPixelFormatInfoMap()
{
    return _pixelFormatInfoTables;
}

void Texture2D::addSpriteFrameCapInset(SpriteFrame* spritframe, const Rect& capInsets)
{
    if(nullptr == _ninePatchInfo)
    {
        _ninePatchInfo = new (std::nothrow) NinePatchInfo;
    }
    if(nullptr == spritframe)
    {
        _ninePatchInfo->capInsetSize = capInsets;
    }
    else
    {
        _ninePatchInfo->capInsetMap[spritframe] = capInsets;
    }
}

bool Texture2D::isContain9PatchInfo()const
{
    return nullptr != _ninePatchInfo;
}

const Rect& Texture2D::getSpriteFrameCapInset( cocos2d::SpriteFrame *spriteFrame )const
{
    CCASSERT(_ninePatchInfo != nullptr,
             "Can't get the sprite frame capInset when the texture contains no 9-patch info.");
    if(nullptr == spriteFrame)
    {
        return this->_ninePatchInfo->capInsetSize;
    }
    else
    {
        auto &capInsetMap = this->_ninePatchInfo->capInsetMap;
        if(capInsetMap.find(spriteFrame) != capInsetMap.end())
        {
            return capInsetMap.at(spriteFrame);
        }
        else
        {
            return this->_ninePatchInfo->capInsetSize;
        }
    }
}


void Texture2D::removeSpriteFrameCapInset(SpriteFrame* spriteFrame)
{
    if(nullptr != this->_ninePatchInfo)
    {
        auto capInsetMap = this->_ninePatchInfo->capInsetMap;
        if(capInsetMap.find(spriteFrame) != capInsetMap.end())
        {
            capInsetMap.erase(spriteFrame);
        }
    }
}

/// halx99 spec, ANDROID ETC1 ALPHA supports.
void Texture2D::setAlphaTexture(Texture2D* alphaTexture)
{
    if (alphaTexture != nullptr) {
        this->_alphaTexture = alphaTexture;
        this->_alphaTexture->retain();
        this->_hasPremultipliedAlpha = true; // PremultipliedAlpha shoud be true.
    }
}
NS_CC_END
