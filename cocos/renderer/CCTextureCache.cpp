/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2017 Chukong Technologies Inc.

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
#include "renderer/CCTextureCache.h"

#include <errno.h>

#include "renderer/CCTexture2D.h"
#include "base/ccUTF8.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "base/Async.h"
#include "platform/CCFileUtils.h"
#include "base/ccUtils.h"
#include "base/CCNinePatchImageParser.h"
#include "bimg/decode.h"


NS_CC_BEGIN


TextureCache::TextureCache()
{
}

TextureCache::~TextureCache()
{
}

std::string TextureCache::getDescription() const
{
    return StringUtils::format("<TextureCache | Number of textures = %d>", static_cast<int>(_textures.size()));
}

static void releaseImage(void* _ptr, void* _userData)
{
    bimg::ImageContainer* imageContainer = static_cast<bimg::ImageContainer*>(_userData);
    bimg::imageFree(imageContainer);
}

/**
 The addImageAsync logic follow the steps:
 - find the image has been add or not, if not add an AsyncStruct to _requestQueue  (GL thread)
 - get AsyncStruct from _requestQueue, load res and fill image data to AsyncStruct.image, then add AsyncStruct to _responseQueue (Load thread)
 - on schedule callback, get AsyncStruct from _responseQueue, convert image to texture, then delete AsyncStruct (GL thread)

 the Critical Area include these members:
 - _requestQueue: locked by _requestMutex
 - _responseQueue: locked by _responseMutex

 the object's life time:
 - AsyncStruct: construct and destruct in GL thread
 - image data: new in Load thread, delete in GL thread(by Image instance)

 Note:
 - all AsyncStruct referenced in _asyncStructQueue, for unbind function use.

 How to deal add image many times?
 - At first, this situation is abnormal, we only ensure the logic is correct.
 - If the image has been loaded, the after load image call will return immediately.
 - If the image request is in queue already, there will be more than one request in queue,
 - In addImageAsyncCallback, will deduplicate the request to ensure only create one texture.

 Does process all response in addImageAsyncCallback consume more time?
 - Convert image to texture faster than load image from disk, so this isn't a problem.
 */
void TextureCache::addImageAsync(String filename, const std::function<void(Texture2D*)>& callback)
{
    Texture2D *texture = nullptr;

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(filename);

    auto it = _textures.find(fullpath);
    if( it != _textures.end() )
        texture = it->second;

    if (texture != nullptr)
    {
        callback(texture);
        return;
    }

    // check if file exists
    if ( fullpath.empty() || ! FileUtils::getInstance()->isFileExist( fullpath ) ) {
        callback(nullptr);
        return;
    }
    std::string file(filename);
    FileUtils::getInstance()->loadFileAsyncUnsafe(fullpath, [this, file, callback](uint8_t* data, ssize_t size)
    {
        SharedAsyncThread.Loader.run([this, data, size]()
        {
            bimg::ImageContainer* imageContainer = bimg::imageParse(&allocator_, data, static_cast<uint32_t>(size));
            free(data);
            return TValues::create(imageContainer);
        }, [this, file, callback](TValues* result) 
        {
            bimg::ImageContainer* imageContainer;
            result->get(imageContainer);
            if (imageContainer)
            {
                uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
                const bgfx::Memory* mem = bgfx::makeRef(
                    imageContainer->m_data, imageContainer->m_size,
                    releaseImage, imageContainer);
                bgfx::TextureHandle handle = bgfx::createTexture2D(
                    static_cast<uint16_t>(imageContainer->m_width),
                    static_cast<uint16_t>(imageContainer->m_height),
                    imageContainer->m_numMips > 1,
                    imageContainer->m_numLayers,
                    static_cast<bgfx::TextureFormat::Enum>(imageContainer->m_format),
                    flags,
                    mem);
                bgfx::TextureInfo info;
                bgfx::calcTextureSize(info,
                    static_cast<uint16_t>(imageContainer->m_width),
                    static_cast<uint16_t>(imageContainer->m_height),
                    static_cast<uint16_t>(imageContainer->m_depth),
                    imageContainer->m_cubeMap,
                    imageContainer->m_numMips > 1,
                    imageContainer->m_numMips,
                    static_cast<bgfx::TextureFormat::Enum>(imageContainer->m_format));
                Texture2D* texture = Texture2D::create(handle, info, flags);
                std::string fullpath = FileUtils::getInstance()->fullPathForFilename(file);
                _textures[fullpath] = texture;
                callback(texture);
            }
            else
            {
                CCLOG("texture format %s is not supported for %s.", Slice(file).getFileExtension().c_str(), file.c_str());
                callback(nullptr);
            }
        });
    });
}

void TextureCache::addImageAsyncOnTexture(String filename, Texture2D* texInput, const std::function<void(Texture2D*)>& callback)
{
    Texture2D *texture = nullptr;

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(filename);

    auto it = _textures.find(fullpath);
    if (it != _textures.end())
        texture = it->second;

    if (texture != nullptr && texture != texInput)
    {
        CCLOG("try to create a new texture, but the texture is already existed in texturecache: %s.", filename);
        callback(nullptr);
        return;
    }

    // check if file exists
    if (fullpath.empty() || !FileUtils::getInstance()->isFileExist(fullpath)) {
        callback(nullptr);
        return;
    }
    std::string file(filename);
    FileUtils::getInstance()->loadFileAsyncUnsafe(fullpath, [this, file, texInput, callback](uint8_t* data, ssize_t size)
    {
        SharedAsyncThread.Loader.run([this, data, size]()
        {
            bimg::ImageContainer* imageContainer = bimg::imageParse(&allocator_, data, static_cast<uint32_t>(size));
            free(data);
            return TValues::create(imageContainer);
        }, [this, file, texInput, callback](TValues* result)
        {
            bimg::ImageContainer* imageContainer;
            result->get(imageContainer);
            if (imageContainer)
            {
                uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
                const bgfx::Memory* mem = bgfx::makeRef(
                    imageContainer->m_data, imageContainer->m_size,
                    releaseImage, imageContainer);
                bgfx::TextureHandle handle = bgfx::createTexture2D(
                    static_cast<uint16_t>(imageContainer->m_width),
                    static_cast<uint16_t>(imageContainer->m_height),
                    imageContainer->m_numMips > 1,
                    imageContainer->m_numLayers,
                    static_cast<bgfx::TextureFormat::Enum>(imageContainer->m_format),
                    flags,
                    mem);
                bgfx::TextureInfo info;
                bgfx::calcTextureSize(info,
                    static_cast<uint16_t>(imageContainer->m_width),
                    static_cast<uint16_t>(imageContainer->m_height),
                    static_cast<uint16_t>(imageContainer->m_depth),
                    imageContainer->m_cubeMap,
                    imageContainer->m_numMips > 1,
                    imageContainer->m_numMips,
                    static_cast<bgfx::TextureFormat::Enum>(imageContainer->m_format));
                texInput->initOnPlace(handle, info, flags);
                std::string fullpath = FileUtils::getInstance()->fullPathForFilename(file);
                _textures[fullpath] = texInput;
                callback(texInput);
            }
            else
            {
                CCLOG("texture format %s is not supported for %s.", Slice(file).getFileExtension().c_str(), file.c_str());
                callback(nullptr);
            }
        });
    });
}

void TextureCache::unbindImageAsync(const std::string& filename)
{

}

void TextureCache::unbindAllImageAsync()
{

}

void TextureCache::loadImage()
{
   
}


Texture2D * TextureCache::addImage(String path)
{
    Texture2D * texture = nullptr;
    Image* image = nullptr;
    // Split up directory and filename
    // MUTEX:
    // Needed since addImageAsync calls this method from a different thread

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(path);
    if (fullpath.empty())
    {
        return nullptr;
    }
    auto it = _textures.find(fullpath);
    if( it != _textures.end() )
        texture = it->second;

    if (! texture)
    {
        // all images are handled by UIImage except PVR extension that is handled by our own handler
        do
        {
            image = new (std::nothrow) Image();
            CC_BREAK_IF(nullptr == image);

            bool bRet = image->initWithImageFile(fullpath);
            CC_BREAK_IF(!bRet);

            texture = new (std::nothrow) Texture2D();

            if( texture && texture->initWithImage(image) )
            {
#if CC_ENABLE_CACHE_TEXTURE_DATA
                // cache the texture file name
                VolatileTextureMgr::addImageTexture(texture, fullpath);
#endif
                // texture already retained, no need to re-retain it
                _textures.insert( std::make_pair(fullpath, texture) );

                //parse 9-patch info
                this->parseNinePatchImage(image, texture, path);
            }
            else
            {
                CCLOG("cocos2d: Couldn't create texture for file:%s in TextureCache", path);
                CC_SAFE_RELEASE(texture);
                texture = nullptr;
            }
        } while (0);
    }

    CC_SAFE_RELEASE(image);

    return texture;
}

void TextureCache::parseNinePatchImage(cocos2d::Image *image, cocos2d::Texture2D *texture,const std::string& path)
{
    if(NinePatchImageParser::isNinePatchImage(path))
    {
        Rect frameRect = Rect(0,0,image->getWidth(), image->getHeight());
        NinePatchImageParser parser(image, frameRect, false);
        texture->addSpriteFrameCapInset(nullptr, parser.parseCapInset());
    }

}

Texture2D* TextureCache::addImage(Image *image, const std::string &key)
{
    CCASSERT(image != nullptr, "TextureCache: image MUST not be nil");

    Texture2D * texture = nullptr;

    do
    {
        auto it = _textures.find(key);
        if( it != _textures.end() ) {
            texture = it->second;
            break;
        }

        // prevents overloading the autorelease pool
        texture = new (std::nothrow) Texture2D();
        if(texture && texture->initWithImage(image))
        {
            _textures.insert( std::make_pair(key, texture) );
            texture->retain();

            texture->autorelease();
        }
        else
        {
            delete texture;
            texture = nullptr;
            CCLOG("cocos2d: Couldn't add UIImage in TextureCache");
        }

    } while (0);

#if CC_ENABLE_CACHE_TEXTURE_DATA
    if (texture)
    {
        VolatileTextureMgr::addImage(texture, image);
    }
#endif

    return texture;
}

Texture2D* TextureCache::addImage(String filename, const uint8_t* data, ssize_t size, Texture2D* passedTexture)
{
    auto it = _textures.find(filename);
    if (it != _textures.end()) 
    {
        if (passedTexture && passedTexture != it->second)
        {
            CCAssert(false, "passedTexture and texture is in texturecache");
        }
        return it->second;
    }
    CCAssert(data && size > 0, "add invalid data to texturecache.");
    bimg::ImageContainer* imageContainer = bimg::imageParse(&allocator_, data, static_cast<uint32_t>(size));
    if (imageContainer)
    {
        uint64_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
        const bgfx::Memory* mem = bgfx::makeRef(
            imageContainer->m_data, imageContainer->m_size,
            releaseImage, imageContainer);
        bgfx::TextureHandle handle = bgfx::createTexture2D(
            static_cast<uint16_t>(imageContainer->m_width),
            static_cast<uint16_t>(imageContainer->m_height),
            imageContainer->m_numMips > 1,
            imageContainer->m_numLayers,
            static_cast<bgfx::TextureFormat::Enum>(imageContainer->m_format),
            flags,
            mem);
        bgfx::TextureInfo info;
        bgfx::calcTextureSize(info,
            static_cast<uint16_t>(imageContainer->m_width),
            static_cast<uint16_t>(imageContainer->m_height),
            static_cast<uint16_t>(imageContainer->m_depth),
            imageContainer->m_cubeMap,
            imageContainer->m_numMips > 1,
            imageContainer->m_numMips,
            static_cast<bgfx::TextureFormat::Enum>(imageContainer->m_format));
        std::string fullpath = filename;
        if (passedTexture)
        {
            passedTexture->initOnPlace(handle, info, flags);
            _textures[fullpath] = passedTexture;
            return passedTexture;
        }
        else
        {
            Texture2D* texture = Texture2D::create(handle, info, flags);
            _textures[fullpath] = texture;
            return texture;
        }
    }
    CCLOG("texture is not loaed correctly for %s.", filename);
    return nullptr;
}

bool TextureCache::reloadTexture(const std::string& fileName)
{
    Texture2D * texture = nullptr;
    Image * image = nullptr;

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(fileName);
    if (fullpath.empty())
    {
        return false;
    }

    auto it = _textures.find(fullpath);
    if (it != _textures.end()) {
        texture = it->second;
    }

    bool ret = false;
    if (! texture) {
        texture = this->addImage(fullpath);
        ret = (texture != nullptr);
    }
    else
    {
        do {
            image = new (std::nothrow) Image();
            CC_BREAK_IF(nullptr == image);

            bool bRet = image->initWithImageFile(fullpath);
            CC_BREAK_IF(!bRet);

            ret = texture->initWithImage(image);
        } while (0);
    }

    CC_SAFE_RELEASE(image);

    return ret;
}

cocos2d::Vector<Texture2D*> TextureCache::getAllTextures() const
{
    cocos2d::Vector<Texture2D*> result;
    for( auto it=_textures.begin(); it!=_textures.end(); ++it ) {
        result.pushBack(it->second);
    }
    return result;
}

// TextureCache - Remove

void TextureCache::removeAllTextures()
{
    _textures.clear();
}

void TextureCache::removeUnusedTextures()
{
    for( auto it=_textures.cbegin(); it!=_textures.cend(); /* nothing */) {
        Texture2D *tex = it->second;
        if( tex->getReferenceCount() == 1 ) {
            CCLOG("cocos2d: TextureCache: removing unused texture: %s", it->first.c_str());

            tex->release();
            it = _textures.erase(it);
        }
        else {
            ++it;
        }

    }
}

void TextureCache::removeTexture(Texture2D* texture)
{
    if( ! texture )
    {
        return;
    }

    for( auto it=_textures.cbegin(); it!=_textures.cend(); /* nothing */ ) {
        if( it->second == texture ) {

            it->second->release();
            it = _textures.erase(it);
            break;
        }
        else
            ++it;
    }
}

void TextureCache::removeTextureForKey(const std::string &textureKeyName)
{
    std::string key = textureKeyName;
    auto it = _textures.find(key);

    if( it == _textures.end() ) {
        key = FileUtils::getInstance()->fullPathForFilename(textureKeyName);
        it = _textures.find(key);
    }

    if( it != _textures.end() ) {
        (it->second)->release();
        _textures.erase(it);
    }
}

Texture2D* TextureCache::getTextureForKey(const std::string &textureKeyName) const
{
    std::string key = textureKeyName;
    auto it = _textures.find(key);

    if( it == _textures.end() ) {
        key = FileUtils::getInstance()->fullPathForFilename(textureKeyName);
        if (key.empty()) {
            return nullptr;
        }
        it = _textures.find(key);
    }

    if( it != _textures.end() )
        return it->second;
    return nullptr;
}

//void TextureCache::reloadAllTextures()
//{
////will do nothing
//// #if CC_ENABLE_CACHE_TEXTURE_DATA
////     VolatileTextureMgr::reloadAllTextures();
//// #endif
//}

std::string TextureCache::getTextureFilePath( cocos2d::Texture2D *texture )const
{
    for(auto& item : _textures)
    {
        if(item.second == texture)
        {
            return item.first;
            break;
        }
    }
    return "";
}


std::string TextureCache::getCachedTextureInfo() const
{
    std::string buffer;
    char buftmp[4096];

    unsigned int count = 0;
    unsigned int totalBytes = 0;

    for( auto it = _textures.begin(); it != _textures.end(); ++it ) {

        memset(buftmp,0,sizeof(buftmp));


        Texture2D* tex = it->second;
        unsigned int bpp = tex->getBitsPerPixelForFormat();
        // Each texture takes up width * height * bytesPerPixel bytes.
        auto bytes = tex->getPixelsWide() * tex->getPixelsHigh() * bpp / 8;
        totalBytes += bytes;
        count++;
        snprintf(buftmp,sizeof(buftmp)-1,"\"%s\" rc=%lu id=%lu %lu x %lu @ %ld bpp => %lu KB\n",
               it->first.c_str(),
               (long)tex->getReferenceCount(),
               (long)tex->getName(),
               (long)tex->getPixelsWide(),
               (long)tex->getPixelsHigh(),
               (long)bpp,
               (long)bytes / 1024);

        buffer += buftmp;
    }

    snprintf(buftmp, sizeof(buftmp)-1, "TextureCache dumpDebugInfo: %ld textures, for %lu KB (%.2f MB)\n", (long)count, (long)totalBytes / 1024, totalBytes / (1024.0f*1024.0f));
    buffer += buftmp;

    return buffer;
}

void TextureCache::renameTextureWithKey(const std::string& srcName, const std::string& dstName)
{
    std::string key = srcName;
    auto it = _textures.find(key);

    if (it == _textures.end()) {
        key = FileUtils::getInstance()->fullPathForFilename(srcName);
        it = _textures.find(key);
    }

    if (it != _textures.end()) {
        std::string fullpath = FileUtils::getInstance()->fullPathForFilename(dstName);
        Texture2D* tex = it->second;

        Image* image = new (std::nothrow) Image();
        if (image)
        {
            bool ret = image->initWithImageFile(dstName);
            if (ret)
            {
                tex->initWithImage(image);
                _textures.insert(std::make_pair(fullpath, tex));
                _textures.erase(it);
            }
            CC_SAFE_RELEASE(image);
        }
    }
}


#if CC_ENABLE_CACHE_TEXTURE_DATA

std::list<VolatileTexture*> VolatileTextureMgr::_textures;
bool VolatileTextureMgr::_isReloading = false;

VolatileTexture::VolatileTexture(Texture2D *t)
: _texture(t)
, _cashedImageType(kInvalid)
, _textureData(nullptr)
, _pixelFormat(Texture2D::PixelFormat::RGBA8888)
, _fileName("")
, _text("")
, _uiImage(nullptr)
, _hasMipmaps(false)
{
    _texParams.minFilter = GL_LINEAR;
    _texParams.magFilter = GL_LINEAR;
    _texParams.wrapS = GL_CLAMP_TO_EDGE;
    _texParams.wrapT = GL_CLAMP_TO_EDGE;
}

VolatileTexture::~VolatileTexture()
{
    CC_SAFE_RELEASE(_uiImage);
}

void VolatileTextureMgr::addImageTexture(Texture2D *tt, const std::string& imageFileName)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kImageFile;
    vt->_fileName = imageFileName;
    vt->_pixelFormat = tt->getPixelFormat();
}

void VolatileTextureMgr::addImage(Texture2D *tt, Image *image)
{
    VolatileTexture *vt = findVolotileTexture(tt);
    image->retain();
    vt->_uiImage = image;
    vt->_cashedImageType = VolatileTexture::kImage;
}

VolatileTexture* VolatileTextureMgr::findVolotileTexture(Texture2D *tt)
{
    VolatileTexture *vt = nullptr;
    auto i = _textures.begin();
    while (i != _textures.end())
    {
        VolatileTexture *v = *i++;
        if (v->_texture == tt)
        {
            vt = v;
            break;
        }
    }

    if (! vt)
    {
        vt = new (std::nothrow) VolatileTexture(tt);
        _textures.push_back(vt);
    }

    return vt;
}

void VolatileTextureMgr::addDataTexture(Texture2D *tt, void* data, int dataLen, Texture2D::PixelFormat pixelFormat, const Size& contentSize)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kImageData;
    vt->_textureData = data;
    vt->_dataLen = dataLen;
    vt->_pixelFormat = pixelFormat;
    vt->_textureSize = contentSize;
}

void VolatileTextureMgr::addStringTexture(Texture2D *tt, const std::string& text, const FontDefinition& fontDefinition)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kString;
    vt->_text     = text;
    vt->_fontDefinition = fontDefinition;
}

void VolatileTextureMgr::setHasMipmaps(Texture2D *t, bool hasMipmaps)
{
    VolatileTexture *vt = findVolotileTexture(t);
    vt->_hasMipmaps = hasMipmaps;
}

void VolatileTextureMgr::setTexParameters(Texture2D *t, const Texture2D::TexParams &texParams)
{
    VolatileTexture *vt = findVolotileTexture(t);

    if (texParams.minFilter != GL_NONE)
        vt->_texParams.minFilter = texParams.minFilter;
    if (texParams.magFilter != GL_NONE)
        vt->_texParams.magFilter = texParams.magFilter;
    if (texParams.wrapS != GL_NONE)
        vt->_texParams.wrapS = texParams.wrapS;
    if (texParams.wrapT != GL_NONE)
        vt->_texParams.wrapT = texParams.wrapT;
}

void VolatileTextureMgr::removeTexture(Texture2D *t)
{
    auto i = _textures.begin();
    while (i != _textures.end())
    {
        VolatileTexture *vt = *i++;
        if (vt->_texture == t)
        {
            _textures.remove(vt);
            delete vt;
            break;
        }
    }
}

void VolatileTextureMgr::reloadAllTextures()
{
    _isReloading = true;

    // we need to release all of the glTextures to avoid collisions of texture id's when reloading the textures onto the GPU
    for(auto iter = _textures.begin(); iter != _textures.end(); ++iter)
    {
        (*iter)->_texture->releaseGLTexture();
    }

    CCLOG("reload all texture");
    auto iter = _textures.begin();

    while (iter != _textures.end())
    {
        VolatileTexture *vt = *iter++;

        switch (vt->_cashedImageType)
        {
        case VolatileTexture::kImageFile:
            {
                Image* image = new (std::nothrow) Image();

                Data data = FileUtils::getInstance()->getDataFromFile(vt->_fileName);

                if (image && image->initWithImageData(data.getBytes(), data.getSize()))
                {
                    Texture2D::PixelFormat oldPixelFormat = Texture2D::getDefaultAlphaPixelFormat();
                    Texture2D::setDefaultAlphaPixelFormat(vt->_pixelFormat);
                    vt->_texture->initWithImage(image);
                    Texture2D::setDefaultAlphaPixelFormat(oldPixelFormat);
                }

                CC_SAFE_RELEASE(image);
            }
            break;
        case VolatileTexture::kImageData:
            {
                vt->_texture->initWithData(vt->_textureData,
                                           vt->_dataLen,
                                          vt->_pixelFormat,
                                          vt->_textureSize.width,
                                          vt->_textureSize.height,
                                          vt->_textureSize);
            }
            break;
        case VolatileTexture::kString:
            {
                vt->_texture->initWithString(vt->_text, vt->_fontDefinition);
            }
            break;
        case VolatileTexture::kImage:
            {
                vt->_texture->initWithImage(vt->_uiImage);
            }
            break;
        default:
            break;
        }
        if (vt->_hasMipmaps) {
            vt->_texture->generateMipmap();
        }
        vt->_texture->setTexParameters(vt->_texParams);
    }

    _isReloading = false;
}

#endif // CC_ENABLE_CACHE_TEXTURE_DATA

NS_CC_END

