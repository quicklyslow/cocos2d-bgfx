/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
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
#include "2d/CCLabel.h"

#include "2d/CCFont.h"
#include "2d/CCFontAtlasCache.h"
#include "2d/CCFontAtlas.h"
#include "2d/CCSprite.h"
#include "2d/CCSpriteBatchNode.h"
#include "2d/CCDrawNode.h"
#include "base/ccUTF8.h"
#include "platform/CCFileUtils.h"
#include "renderer/Renderer.h"
#include "renderer/Program.h"
#include "base/CCDirector.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventCustom.h"
#include "2d/CCFontFNT.h"
#include "2d/CCSpriteFrame.h"

NS_CC_BEGIN

/**
 * LabelLetter used to update the quad in texture atlas without SpriteBatchNode.
 */
class LabelLetter : public Sprite
{
public:
    LabelLetter()
    {
        _textureAtlas = nullptr;
        _letterVisible = true;
    }

    static LabelLetter* createWithTexture(Texture2D *texture, const Rect& rect, bool rotated = false)
    {
        auto letter = new (std::nothrow) LabelLetter();
        if (letter && letter->initWithTexture(texture, rect, rotated))
        {
            letter->Sprite::setVisible(false);
            letter->autorelease();
            return letter;
        }
        CC_SAFE_DELETE(letter);
        return nullptr;
    }

    CREATE_FUNC(LabelLetter);

    virtual void updateTransform() override
    {
        if (isDirty())
        {
            _transformToBatch = getNodeToParentTransform();
            Size &size = _rect.size;

            float x1 = _offsetPosition.x;
            float y1 = _offsetPosition.y;
            float x2 = x1 + size.width;
            float y2 = y1 + size.height;
            if (_flippedX)
            {
                std::swap(x1, x2);
            }
            if (_flippedY)
            {
                std::swap(y1, y2);
            }

            float x = _transformToBatch.m[12];
            float y = _transformToBatch.m[13];

            float cr = _transformToBatch.m[0];
            float sr = _transformToBatch.m[1];
            float cr2 = _transformToBatch.m[5];
            float sr2 = -_transformToBatch.m[4];
            float ax = x1 * cr - y1 * sr2 + x;
            float ay = x1 * sr + y1 * cr2 + y;

            float bx = x2 * cr - y1 * sr2 + x;
            float by = x2 * sr + y1 * cr2 + y;
            float cx = x2 * cr - y2 * sr2 + x;
            float cy = x2 * sr + y2 * cr2 + y;
            float dx = x1 * cr - y2 * sr2 + x;
            float dy = x1 * sr + y2 * cr2 + y;

            _quad.bl.vertices.set(SPRITE_RENDER_IN_SUBPIXEL(ax), SPRITE_RENDER_IN_SUBPIXEL(ay), _positionZ);
            _quad.br.vertices.set(SPRITE_RENDER_IN_SUBPIXEL(bx), SPRITE_RENDER_IN_SUBPIXEL(by), _positionZ);
            _quad.tl.vertices.set(SPRITE_RENDER_IN_SUBPIXEL(dx), SPRITE_RENDER_IN_SUBPIXEL(dy), _positionZ);
            _quad.tr.vertices.set(SPRITE_RENDER_IN_SUBPIXEL(cx), SPRITE_RENDER_IN_SUBPIXEL(cy), _positionZ);

            if (_textureAtlas)
            {
                _textureAtlas->updateQuad(&_quad, _atlasIndex);
            }

            _recursiveDirty = false;
            setDirty(false);
        }

        Node::updateTransform();
    }

    virtual void updateColor() override
    {
        if (_textureAtlas == nullptr)
        {
            return;
        }

        auto displayedOpacity = _displayedOpacity;
        if(!_letterVisible)
        {
            displayedOpacity = 0.0f;
        }
        Color4B color4(_displayedColor.r, _displayedColor.g, _displayedColor.b, displayedOpacity);
        // special opacity for premultiplied textures
        if (flags_.isOn(Sprite::OpacityModifyRGB))
        {
            color4.r *= displayedOpacity / 255.0f;
            color4.g *= displayedOpacity / 255.0f;
            color4.b *= displayedOpacity / 255.0f;
        }

        _quad.bl.colors = color4;
        _quad.br.colors = color4;
        _quad.tl.colors = color4;
        _quad.tr.colors = color4;

        _textureAtlas->updateQuad(&_quad, _atlasIndex);
    }

    void setVisible(bool visible) override
    {
        _letterVisible = visible;
        updateColor();
    }

    //LabelLetter doesn't need to draw directly.
    void draw(IRenderer *renderer, const Mat4 &transform, uint32_t flags) override
    {
    }

private:
    bool _letterVisible;
};


Label* Label::create()
{
    auto ret = new (std::nothrow) Label();

    if (ret)
    {
        ret->autorelease();
    }

    return ret;
}

Label* Label::createWithSystemFont(const std::string& text, const std::string& font, float fontSize, const Size& dimensions /* = Size::ZERO */, TextHAlignment hAlignment /* = TextHAlignment::LEFT */, TextVAlignment vAlignment /* = TextVAlignment::TOP */)
{
    auto ret = new (std::nothrow) Label();

    if (ret)
    {
        ret->_hAlignment = hAlignment;
        ret->_vAlignment = vAlignment;
        ret->setSystemFontName(font);
        ret->setSystemFontSize(fontSize);
        ret->setDimensions(dimensions.width, dimensions.height);
        ret->setString(text);

        ret->autorelease();

        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

Label* Label::createWithTTF(const std::string& text, const std::string& fontFile, float fontSize, const Size& dimensions /* = Size::ZERO */, TextHAlignment hAlignment /* = TextHAlignment::LEFT */, TextVAlignment vAlignment /* = TextVAlignment::TOP */)
{
    auto ret = new (std::nothrow) Label();

    if (ret && ret->initWithTTF(text, fontFile, fontSize, dimensions, hAlignment, vAlignment))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

Label* Label::createWithTTF(const TTFConfig& ttfConfig, const std::string& text, TextHAlignment hAlignment /* = TextHAlignment::CENTER */, int maxLineWidth /* = 0 */)
{
    auto ret = new (std::nothrow) Label();

    if (ret && ret->initWithTTF(ttfConfig, text, hAlignment, maxLineWidth))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

Label* Label::createWithBMFont(const std::string& fntDataString,
                               const std::string& text,
                               SpriteFrame* spriteFrame,
                               const TextHAlignment& hAlignment /* = TextHAlignment::LEFT */,
                               int maxLineWidth /* = 0 */,
                               const Vec2& imageOffset /* = Vec2::ZERO */)
{
    spriteFrame->autorelease(); //FIXME: Why to autorelease sprite frame here?

    auto ret = new (std::nothrow) Label();
    if (ret != nullptr)
    {
        ret->_hAlignment = hAlignment;

        if (ret->setBMFontFilePath(fntDataString, spriteFrame, imageOffset))
        {
            ret->setMaxLineWidth(maxLineWidth);
            ret->setString(text);
            ret->autorelease();
            return ret;
        }
    }

    delete ret;
    return nullptr;
}

Label* Label::createWithCharMap(const std::string& plistFile)
{
    auto ret = new (std::nothrow) Label();

    if (ret && ret->setCharMap(plistFile))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

Label* Label::createWithCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
    auto ret = new (std::nothrow) Label();

    if (ret && ret->setCharMap(texture,itemWidth,itemHeight,startCharMap))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

Label* Label::createWithCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
    auto ret = new (std::nothrow) Label();

    if (ret && ret->setCharMap(charMapFile,itemWidth,itemHeight,startCharMap))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool Label::setCharMap(const std::string& plistFile)
{
    auto newAtlas = FontAtlasCache::getFontAtlasCharMap(plistFile);

    if (!newAtlas)
    {
        reset();
        return false;
    }

    _currentLabelType = LabelType::CHARMAP;
    setFontAtlas(newAtlas);

    return true;
}

bool Label::initWithTTF(const std::string& text, const std::string& fontFilePath, float fontSize,
                        const Size& dimensions, TextHAlignment hAlignment, TextVAlignment vAlignment)
{
    _hAlignment = hAlignment;
    _vAlignment = vAlignment;
    if (FileUtils::getInstance()->isFileExist(fontFilePath))
    {
        TTFConfig ttfConfig(fontFilePath, fontSize, GlyphCollection::DYNAMIC);
        if (setTTFConfig(ttfConfig))
        {
            setDimensions(dimensions.width, dimensions.height);
            setString(text);
        }
        return true;
    }
    return false;
}

bool Label::initWithTTF(const TTFConfig& ttfConfig, const std::string& text, TextHAlignment hAlignment, int maxLineWidth)
{
    _hAlignment = hAlignment;
    if (FileUtils::getInstance()->isFileExist(ttfConfig.fontFilePath) && setTTFConfig(ttfConfig))
    {
        setMaxLineWidth(maxLineWidth);
        setString(text);
        return true;
    }
    return false;
}

bool Label::setCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
    auto newAtlas = FontAtlasCache::getFontAtlasCharMap(texture,itemWidth,itemHeight,startCharMap);

    if (!newAtlas)
    {
        reset();
        return false;
    }

    _currentLabelType = LabelType::CHARMAP;
    setFontAtlas(newAtlas);

    return true;
}

bool Label::setCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
    auto newAtlas = FontAtlasCache::getFontAtlasCharMap(charMapFile,itemWidth,itemHeight,startCharMap);

    if (!newAtlas)
    {
        reset();
        return false;
    }

    _currentLabelType = LabelType::CHARMAP;
    setFontAtlas(newAtlas);

    return true;
}

Label::Label()
: _systemFont("Helvetica")
, _textSprite(nullptr)
, _shadowNode(nullptr)
, _fontAtlas(nullptr)
, _horizontalKernings(nullptr)
, _purgeTextureListener(nullptr)
, _resetTextureListener(nullptr)
#if CC_LABEL_DEBUG_DRAW
, _debugDrawNode(nullptr)
#endif
, _fntSpriteFrame(nullptr)
, _underlineNode(nullptr)
, _reusedLetter(nullptr)
, _effectColorF(Color4F::BLACK)
, _textColor(Color4B::WHITE)
, _textColorF(Color4F::WHITE)
, _textGradientStartColor(Color4F::WHITE)
, _textGradientEndColor(Color4F::WHITE)
, _uniformEffectColor(0)
, _uniformTextColor(0)
, _shadowOpacity(0)
, _blendFunc(BlendFunc::ALPHA_PREMULTIPLIED)
, _numberOfLines(0)
, _lengthOfString(0)
, _outlineSize(0.0f)
, _systemFontSize(12.0f)
, _lineHeight(0.0f)
, _lineSpacing(0.0f)
, _additionalKerning(0.0f)
, _maxLineWidth(0.0f)
, _labelWidth(0.0f)
, _labelHeight(0.0f)
, _textDesiredHeight(0.0f)
, _letterOffsetY(0.0f)
, _tailoredTopY(0.0f)
, _tailoredBottomY(0.0f)
, _shadowBlurRadius(0.0f)
, _bmFontSize(-1.0f)
, _bmfontScale(1.0f)
, _originalFontSize(0.0f)
, _gradientAngle(0.0f)
, _currLabelEffect(LabelEffect::NORMAL)
, _currentLabelType(LabelType::STRING_TEXTURE)
, _overflow(Overflow::NONE)
, _hAlignment(TextHAlignment::LEFT)
, _vAlignment(TextVAlignment::TOP)
, _systemFontDirty(false)
, _contentDirty(false)
, _lineBreakWithoutSpaces(false)
, _strikethroughEnabled(false)
, _isOpacityModifyRGB(false)
, _enableWrap(true)
, _clipEnabled(false)
, _blendFuncDirty(false)
, _useDistanceField(false)
, _useA8Shader(false)
, _shadowDirty(false)
, program_(nullptr)
{
    setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    setColor(Color3B::WHITE);

#if CC_LABEL_DEBUG_DRAW
    _debugDrawNode = DrawNode::create();
    addChild(_debugDrawNode);
#endif

    _purgeTextureListener = EventListenerCustom::create(FontAtlas::CMD_PURGE_FONTATLAS, [this](EventCustom* event){
        if (_fontAtlas && _currentLabelType == LabelType::TTF && event->getUserData() == _fontAtlas)
        {
            for (auto&& it : _letters)
            {
                it.second->setTexture(nullptr);
            }
            _batchNodes.clear();

            if (_fontAtlas)
            {
                FontAtlasCache::releaseFontAtlas(_fontAtlas);
            }
        }
    });
    _eventDispatcher->addEventListenerWithFixedPriority(_purgeTextureListener, 1);

    _resetTextureListener = EventListenerCustom::create(FontAtlas::CMD_RESET_FONTATLAS, [this](EventCustom* event){
        if (_fontAtlas && _currentLabelType == LabelType::TTF && event->getUserData() == _fontAtlas)
        {
            _fontAtlas = nullptr;
            this->setTTFConfig(_fontConfig);
            for (auto&& it : _letters)
            {
                getLetter(it.first);
            }
        }
    });
    _eventDispatcher->addEventListenerWithFixedPriority(_resetTextureListener, 2);
}

Label::~Label()
{
    delete [] _horizontalKernings;
    _horizontalKernings = nullptr;

    if (_fontAtlas != nullptr)
    {
//        Node::removeAllChildrenWithCleanup(true);
        _batchNodes.clear();
        FontAtlasCache::releaseFontAtlas(_fontAtlas);
        _fontAtlas = nullptr;
    }
    _eventDispatcher->removeEventListener(_purgeTextureListener);
    _purgeTextureListener = nullptr;
    _eventDispatcher->removeEventListener(_resetTextureListener);
    _resetTextureListener = nullptr;
}

void Label::reset()
{
    _textSprite = nullptr;
    _shadowNode = nullptr;
    Node::removeAllChildrenWithCleanup(true);
    _reusedLetter = nullptr;

    _letters.clear();
    _batchNodes.clear();
    _lettersInfo.clear();
    if (_fontAtlas)
    {
        FontAtlasCache::releaseFontAtlas(_fontAtlas);
        _fontAtlas = nullptr;
    }

    _currentLabelType = LabelType::STRING_TEXTURE;
    _currLabelEffect.setValue(LabelEffect::NORMAL);
    _contentDirty = false;
    _numberOfLines = 0;
    _lengthOfString = 0;
    _utf16Text.clear();
    _utf8Text.clear();

    TTFConfig temp;
    _fontConfig = temp;
    _outlineSize = 0.f;
    _bmFontPath = "";
    _systemFontDirty = false;
    _systemFont = "Helvetica";
    _systemFontSize = 12;

    if (_horizontalKernings)
    {
        delete[] _horizontalKernings;
        _horizontalKernings = nullptr;
    }
    _additionalKerning = 0.f;
    _lineHeight = 0.f;
    _lineSpacing = 0.f;
    _maxLineWidth = 0.f;
    _labelDimensions.width = 0.f;
    _labelDimensions.height = 0.f;
    _labelWidth = 0.f;
    _labelHeight = 0.f;
    _lineBreakWithoutSpaces = false;
    _hAlignment = TextHAlignment::LEFT;
    _vAlignment = TextVAlignment::TOP;

    _effectColorF = Color4F::BLACK;
    _textColor = Color4B::WHITE;
    _textColorF = Color4F::WHITE;
    setColor(Color3B::WHITE);

    _shadowDirty = false;
    _shadowBlurRadius = 0.f;

    _useDistanceField = false;
    _useA8Shader = false;
    _clipEnabled = false;
    _blendFuncDirty = false;
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    _isOpacityModifyRGB = false;
    _enableWrap = true;
    _bmFontSize = -1;
    _bmfontScale = 1.0f;
    _overflow = Overflow::NONE;
    _originalFontSize = 0.0f;
    if (_underlineNode)
    {
        removeChild(_underlineNode);
        _underlineNode = nullptr;
    }
    _strikethroughEnabled = false;
    setRotationSkewX(0);        // reverse italics
}

//  ETC1 ALPHA supports, for LabelType::BMFONT & LabelType::CHARMAP
static Texture2D* _getTexture(Label* label)
{
    struct _FontAtlasPub : public FontAtlas
    {
        Texture2D* getTexture()
        {
            if (!_atlasTextures.empty())
                return _atlasTextures.begin()->second;
            return nullptr;
        }
    };

    auto fontAtlas = label->getFontAtlas();
    Texture2D* texture = nullptr;
    if (fontAtlas != nullptr)
        texture = ((_FontAtlasPub*)(fontAtlas))->getTexture();
    return texture;
}
void Label::updateShaderProgram()
{
	if (_currLabelEffect.isOn(LabelEffect::NORMAL))
	{
        if (_useDistanceField)
            program_ = SharedRenderer.getDistanceField();
        else if (_useA8Shader)
            program_ = SharedRenderer.getNormalProgram();
        else if (_currLabelEffect.isOn(LabelEffect::SHADOW))
            program_ = SharedRenderer.getDefaultProgram();
        else
            program_ = SharedRenderer.getDefaultProgramMVP();//is this could be used?
	}
	else
	{
		if (_currLabelEffect.isOn(LabelEffect::GRADIENT) && _currLabelEffect.isOn(LabelEffect::OUTLINE))
		{
            program_ = SharedRenderer.getGradientOutlineProgram();
		}
		else if (_currLabelEffect.isOn(LabelEffect::OUTLINE))
		{
            program_ = SharedRenderer.getOutlineProgram();
		}
		else if (_currLabelEffect.isOn(LabelEffect::GRADIENT))
		{
            program_ = SharedRenderer.getGradientProgram();
		} 
		else
		{
			if (_currLabelEffect.isOn(LabelEffect::GLOW))
			{
                program_ = SharedRenderer.getDistanceFieldGlowProgram();
			}
		}
	}
}

void Label::setFontAtlas(FontAtlas* atlas,bool distanceFieldEnabled /* = false */, bool useA8Shader /* = false */)
{
    if(atlas)
    {
        _systemFontDirty = false;
    }

    if (atlas == _fontAtlas)
    {
        FontAtlasCache::releaseFontAtlas(atlas);
        return;
    }

    if (_fontAtlas)
    {
        _batchNodes.clear();
        FontAtlasCache::releaseFontAtlas(_fontAtlas);
        _fontAtlas = nullptr;
    }

    _fontAtlas = atlas;
    if (_reusedLetter == nullptr)
    {
        _reusedLetter = Sprite::create();
        _reusedLetter->setOpacityModifyRGB(_isOpacityModifyRGB);
        _reusedLetter->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    }

    if (_fontAtlas)
    {
        _lineHeight = _fontAtlas->getLineHeight();
        _contentDirty = true;
        _systemFontDirty = false;
    }
    _useDistanceField = distanceFieldEnabled;
    _useA8Shader = useA8Shader;

    if (_currentLabelType != LabelType::TTF)
    {
        _currLabelEffect.setValue(LabelEffect::NORMAL);
        updateShaderProgram();
    }
}

bool Label::setTTFConfig(const TTFConfig& ttfConfig)
{
    _originalFontSize = ttfConfig.fontSize;
    return setTTFConfigInternal(ttfConfig);
}

bool Label::setBMFontFilePath(const std::string& fntDataString,
                              SpriteFrame* spriteFrame,
                              const Vec2& imageOffset,
                              float fontSize)
{
    FontAtlas *newAtlas = FontAtlasCache::getFontAtlasFNT(fntDataString,
                                                          spriteFrame,
                                                          imageOffset);

    if (!newAtlas)
    {
        reset();
        return false;
    }

    //assign the default fontSize
    if (std::abs(fontSize) < FLT_EPSILON) {
        FontFNT *bmFont = (FontFNT*)newAtlas->getFont();
        if (bmFont) {
            float originalFontSize = bmFont->getOriginalFontSize();
            _bmFontSize = originalFontSize / CC_CONTENT_SCALE_FACTOR();
        }
    }

    if(fontSize > 0.0f){
        _bmFontSize = fontSize;
    }

    _bmFontPath = fntDataString;
    _fntSpriteFrame = spriteFrame;

    _currentLabelType = LabelType::BMFONT;
    setFontAtlas(newAtlas);

    return true;
}

void Label::setString(const std::string& text)
{
    if (text.compare(_utf8Text))
    {
		std::u16string utf16String;
		if (StringUtils::UTF8ToUTF16(text, utf16String))
		{
			if (!colorText(utf16String))
			{
				_utf8Text = text;
				_utf16Text = utf16String;
			}
			else // colorText changed _utf8Text, so convert is need
			{
				StringUtils::UTF8ToUTF16(_utf8Text, _utf16Text);
			}
		}
        _contentDirty = true;
    }
}

bool Label::colorText(const std::u16string & utf16String)
{
	std::string::size_type size = utf16String.size();

	_colorSection.clear();
	_colorIndexNum.clear();
	_utf8Text.clear();
	_utf8Text.reserve(size);
	std::u16string outStr;

	std::string::size_type cursor = utf16String.find(u"<color=#");
	int count = 0;
	if (cursor != std::string::npos)
	{
		while (cursor != std::string::npos)
		{
			std::string::size_type endp = utf16String.find(u"</color>", cursor + 15);

			if (endp == std::string::npos)
			{
				_colorSection.push_back(Color3B(utf16String.substr(cursor + 8, 6)));

				break;
			}
			else
			{
				_colorSection.push_back(Color3B(utf16String.substr(cursor + 8, 6)));
				cursor = utf16String.find(u"<color=#", endp + 8);
			}
		}

		cursor = utf16String.find(u"<color=#");
		if (cursor != 0)
		{
			outStr.append(utf16String.substr(0, cursor));
		}
		while (cursor != std::string::npos)
		{
			std::string::size_type endp = utf16String.find(u"</color>", cursor + 15);
			if (endp == std::string::npos)
			{
				int fistIn = outStr.length();
				outStr.append(utf16String.substr(cursor + 15, utf16String.length() - (cursor + 15) + 1));
				_colorIndexNum.push_back({ fistIn, int(outStr.length()) });
				break;
			}
			else
			{
				_colorIndexNum.push_back({ int(outStr.length()), int(outStr.length() + endp - (cursor + 15) - 1) });
				outStr.append(utf16String.substr(cursor + 15, endp - (cursor + 15)));
				cursor = utf16String.find(u"<color=#", endp + 8);
				if (cursor != std::string::npos && cursor > endp + 8)
				{
					outStr.append(utf16String.substr(endp + 8, cursor - (endp + 8)));
				}
			}
			if (cursor == std::string::npos)
			{
				outStr.append(utf16String.substr(endp + 8, utf16String.length() - (endp + 8) + 1));
			}
		}
		StringUtils::UTF16ToUTF8(outStr, _utf8Text);
		return true;
	}
	else
	{
		return false;
	}

}

void Label::setAlignment(TextHAlignment hAlignment,TextVAlignment vAlignment)
{
    if (hAlignment != _hAlignment || vAlignment != _vAlignment)
    {
        _hAlignment = hAlignment;
        _vAlignment = vAlignment;

        _contentDirty = true;
    }
}

void Label::setMaxLineWidth(float maxLineWidth)
{
    if (_labelWidth == 0 && _maxLineWidth != maxLineWidth)
    {
        _maxLineWidth = maxLineWidth;
        _contentDirty = true;
    }
}

void Label::setDimensions(float width, float height)
{
    if(_overflow == Overflow::RESIZE_HEIGHT){
        height = 0;
    }

    if (height != _labelHeight || width != _labelWidth)
    {
        _labelWidth = width;
        _labelHeight = height;
        _labelDimensions.width = width;
        _labelDimensions.height = height;

        _maxLineWidth = width;
        _contentDirty = true;

        if(_overflow == Overflow::SHRINK){
            if (_originalFontSize > 0) {
                this->restoreFontSize();
            }
        }
    }
}

void Label::restoreFontSize()
{
    if(_currentLabelType == LabelType::TTF){
        auto ttfConfig = this->getTTFConfig();
        ttfConfig.fontSize = _originalFontSize;
        this->setTTFConfigInternal(ttfConfig);
    }else if(_currentLabelType == LabelType::BMFONT){
        this->setBMFontSizeInternal(_originalFontSize);
    }else if(_currentLabelType == LabelType::STRING_TEXTURE){
        this->setSystemFontSize(_originalFontSize);
    }
}

void Label::setLineBreakWithoutSpace(bool breakWithoutSpace)
{
    if (breakWithoutSpace != _lineBreakWithoutSpaces)
    {
        _lineBreakWithoutSpaces = breakWithoutSpace;
        _contentDirty = true;
    }
}

void Label::updateLabelLetters()
{
    if (!_letters.empty())
    {
        Rect uvRect;
        LabelLetter* letterSprite;
        int letterIndex;

        for (auto it = _letters.begin(); it != _letters.end();)
        {
            letterIndex = it->first;
            letterSprite = (LabelLetter*)it->second;

            if (letterIndex >= _lengthOfString)
            {
                Node::removeChild(letterSprite, true);
                it = _letters.erase(it);
            }
            else
            {
                auto& letterInfo = _lettersInfo[letterIndex];
                auto& letterDef = _fontAtlas->_letterDefinitions[letterInfo.utf16Char];
                uvRect.size.height = letterDef.height;
                uvRect.size.width = letterDef.width;
                uvRect.origin.x = letterDef.U;
                uvRect.origin.y = letterDef.V;

                auto batchNode = _batchNodes.at(letterDef.textureID);
                letterSprite->setTextureAtlas(batchNode->getTextureAtlas());
                letterSprite->setTexture(_fontAtlas->getTexture(letterDef.textureID));
                if (letterDef.width <= 0.f || letterDef.height <= 0.f)
                {
                    letterSprite->setTextureAtlas(nullptr);
                }
                else
                {
                    letterSprite->setTextureRect(uvRect, false, uvRect.size);
                    letterSprite->setTextureAtlas(_batchNodes.at(letterDef.textureID)->getTextureAtlas());
                    letterSprite->setAtlasIndex(_lettersInfo[letterIndex].atlasIndex);
                }

                auto px = letterInfo.positionX + letterDef.width / 2 + _linesOffsetX[letterInfo.lineIndex];
                auto py = letterInfo.positionY - letterDef.height / 2 + _letterOffsetY;
                letterSprite->setPosition(px, py);

                this->updateLetterSpriteScale(letterSprite);
                ++it;
            }
        }
    }
}

bool Label::alignText()
{
    if (_fontAtlas == nullptr || _utf16Text.empty())
    {
        if(_overflow == Overflow::NONE) {
            setContentSize(Size::ZERO);
        }
        return true;
    }

    bool ret = true;
    do {
        _fontAtlas->prepareLetterDefinitions(_utf16Text);
        auto& textures = _fontAtlas->getTextures();
        if (textures.size() > static_cast<size_t>(_batchNodes.size()))
        {
            for (auto index = static_cast<size_t>(_batchNodes.size()); index < textures.size(); ++index)
            {
                auto batchNode = SpriteBatchNode::createWithTexture(textures.at(index));
                if (batchNode)
                {
                    _isOpacityModifyRGB = batchNode->getTexture()->hasPremultipliedAlpha();
                    _blendFunc = batchNode->getBlendFunc();
                    batchNode->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
                    batchNode->setPosition(Vec2::ZERO);
                    _batchNodes.pushBack(batchNode);
                }
            }
        }
        if (_batchNodes.empty())
        {
            return true;
        }
        _reusedLetter->setBatchNode(_batchNodes.at(0));

        _lengthOfString = 0;
        _textDesiredHeight = 0.f;
        _linesWidth.clear();
        if (_maxLineWidth > 0.f && !_lineBreakWithoutSpaces)
        {
            multilineTextWrapByWord();
        }
        else
        {
            multilineTextWrapByChar();
        }
        computeAlignmentOffset();

        if(_overflow == Overflow::SHRINK){
            float fontSize = this->getRenderingFontSize();

            if(fontSize > 0 &&  isVerticalClamp()){
                this->shrinkLabelToContentSize(CC_CALLBACK_0(Label::isVerticalClamp, this));
            }
        }

        if(!updateQuads()){
            ret = false;
            if(_overflow == Overflow::SHRINK){
                this->shrinkLabelToContentSize(CC_CALLBACK_0(Label::isHorizontalClamp, this));
            }
            break;
        }

        if (!_colorSection.empty())
        {
            for (int index = 0; index < _lengthOfString; ++index)
            {
                auto& letterInfo = _lettersInfo[index];
                if (letterInfo.valid)
                {
                    auto iter = std::find_if(_colorIndexNum.begin(), _colorIndexNum.end(), [=](const auto& it) -> bool {
                        return index >= it.start && index <= it.end;
                    });
                    if (iter != _colorIndexNum.end())
                    {
                        letterInfo.colorIndex = iter - _colorIndexNum.begin();
                    }
                    else
                    {
                        letterInfo.colorIndex = -1;
                    }
                }
            }
        }

        updateLabelLetters();

        updateColor();
    }while (0);

    return ret;
}

bool Label::computeHorizontalKernings(const std::u16string& stringToRender)
{
    if (_horizontalKernings)
    {
        delete [] _horizontalKernings;
        _horizontalKernings = nullptr;
    }

    int letterCount = 0;
    _horizontalKernings = _fontAtlas->getFont()->getHorizontalKerningForTextUTF16(stringToRender, letterCount);

    if(!_horizontalKernings)
        return false;
    else
        return true;
}

bool Label::isHorizontalClamped(float letterPositionX, int lineIndex)
{
    auto wordWidth = this->_linesWidth[lineIndex];
    bool letterOverClamp = (letterPositionX > _contentSize.width || letterPositionX < 0);
    if (!_enableWrap) {
        return letterOverClamp;
    }else{
        return (wordWidth > this->_contentSize.width && letterOverClamp);
    }
}

bool Label::updateQuads()
{
    bool ret = true;
    for (auto&& batchNode : _batchNodes)
    {
        batchNode->getTextureAtlas()->removeAllQuads();
    }

    bool letterClamp = false;
    for (int ctr = 0; ctr < _lengthOfString; ++ctr)
    {
        if (_lettersInfo[ctr].valid)
        {
            auto& letterDef = _fontAtlas->_letterDefinitions[_lettersInfo[ctr].utf16Char];
            //_lettersInfo[ctr].atlasIndex = 0;//initialize for space letters

            _reusedRect.size.height = letterDef.height;
            _reusedRect.size.width  = letterDef.width;
            _reusedRect.origin.x    = letterDef.U;
            _reusedRect.origin.y    = letterDef.V;

            auto py = _lettersInfo[ctr].positionY + _letterOffsetY;
            if (_labelHeight > 0.f) {
                if (py > _tailoredTopY)
                {
                    auto clipTop = py - _tailoredTopY;
                    _reusedRect.origin.y += clipTop;
                    _reusedRect.size.height -= clipTop;
                    py -= clipTop;
                }
                if (py - letterDef.height * _bmfontScale < _tailoredBottomY)
                {
                    _reusedRect.size.height = (py < _tailoredBottomY) ? 0.f : (py - _tailoredBottomY);
                }
            }

            auto lineIndex = _lettersInfo[ctr].lineIndex;
            auto px = _lettersInfo[ctr].positionX + letterDef.width/2 * _bmfontScale + _linesOffsetX[lineIndex];

            if(_labelWidth > 0.f){
                if (this->isHorizontalClamped(px, lineIndex)) {
                    if(_overflow == Overflow::CLAMP){
                        _reusedRect.size.width = 0;
                    }else if(_overflow == Overflow::SHRINK){
                        if (_contentSize.width > letterDef.width) {
                            letterClamp = true;
                            ret = false;
                            break;
                        }else{
                            _reusedRect.size.width = 0;
                        }

                    }
                }
            }


            if (_reusedRect.size.height > 0.f && _reusedRect.size.width > 0.f)
            {
                if(_currentLabelType == Label::LabelType::BMFONT) {
                    auto isRotated = _fntSpriteFrame->isRotated();
                    auto spriteFrameRect = _fntSpriteFrame->getRect();
                    auto originalSize = _fntSpriteFrame->getOriginalSize();
                    auto offset = _fntSpriteFrame->getOffset();
                    auto trimmedLeft = offset.x + (originalSize.width - spriteFrameRect.size.width) / 2;
                    auto trimmedTop = offset.y - (originalSize.height - spriteFrameRect.size.height) / 2;

                    if (!isRotated) {
                        _reusedRect.origin.x += spriteFrameRect.origin.x - trimmedLeft;
                        _reusedRect.origin.y += spriteFrameRect.origin.y + trimmedTop;
                    } else {
                        auto originalX = _reusedRect.origin.x;
                        _reusedRect.origin.x = spriteFrameRect.origin.x + spriteFrameRect.size.height - _reusedRect.origin.y - _reusedRect.size.height - trimmedTop;
                        _reusedRect.origin.y = originalX + spriteFrameRect.origin.y - trimmedLeft;
                        if(_reusedRect.origin.y < 0) {
							_reusedRect.size.height += trimmedTop;
                        }
                    }


                    _reusedLetter->setTextureRect(_reusedRect, isRotated, _reusedRect.size);
                } else {
                    _reusedLetter->setTextureRect(_reusedRect, false, _reusedRect.size);

                }

                float letterPositionX = _lettersInfo[ctr].positionX + _linesOffsetX[_lettersInfo[ctr].lineIndex];
                _reusedLetter->setPosition(letterPositionX, py);
                auto index = static_cast<int>(_batchNodes.at(letterDef.textureID)->getTextureAtlas()->getTotalQuads());
                _lettersInfo[ctr].atlasIndex = index;

                this->updateLetterSpriteScale(_reusedLetter);

                _batchNodes.at(letterDef.textureID)->insertQuadFromSprite(_reusedLetter, index);
            }
        }
    }


    return ret;
}

bool Label::setTTFConfigInternal(const TTFConfig& ttfConfig)
{
    FontAtlas *newAtlas = FontAtlasCache::getFontAtlasTTF(&ttfConfig);

    if (!newAtlas)
    {
        reset();
        return false;
    }

    _currentLabelType = LabelType::TTF;
    setFontAtlas(newAtlas,ttfConfig.distanceFieldEnabled,true);

    _fontConfig = ttfConfig;

    if (_fontConfig.outlineSize > 0)
    {
        _fontConfig.distanceFieldEnabled = false;
        _useDistanceField = false;
        _useA8Shader = false;
        _currLabelEffect.setOff(LabelEffect::NORMAL);
        _currLabelEffect.setOn(LabelEffect::OUTLINE);
        updateShaderProgram();
    }
    else
    {
        _currLabelEffect.setValue(LabelEffect::NORMAL);
        updateShaderProgram();
    }

    if (_fontConfig.italics)
        this->enableItalics();
    if (_fontConfig.bold)
        this->enableBold();
    if (_fontConfig.underline)
        this->enableUnderline();
    if (_fontConfig.strikethrough)
        this->enableStrikethrough();

    return true;
}

void Label::setBMFontSizeInternal(float fontSize)
{
    if(_currentLabelType == LabelType::BMFONT){
        this->setBMFontFilePath(_bmFontPath,
                                _fntSpriteFrame,
                                Vec2::ZERO, fontSize);
        _contentDirty = true;
    }
}

void Label::scaleFontSizeDown(float fontSize)
{
    bool shouldUpdateContent = true;
    if(_currentLabelType == LabelType::TTF){
        auto ttfConfig = this->getTTFConfig();
        ttfConfig.fontSize = fontSize;
        this->setTTFConfigInternal(ttfConfig);
    }else if(_currentLabelType == LabelType::BMFONT){
        if (std::abs(fontSize) < FLT_EPSILON) {
            fontSize = 0.1f;
            shouldUpdateContent = false;
        }
        this->setBMFontSizeInternal(fontSize);
    }else if (_currentLabelType == LabelType::STRING_TEXTURE){
        this->setSystemFontSize(fontSize);
    }

    if (shouldUpdateContent) {
        this->updateContent();
    }
}

void Label::enableGlow(const Color4B& glowColor)
{
    if (_currentLabelType == LabelType::TTF)
    {
        if (_fontConfig.distanceFieldEnabled == false)
        {
            auto config = _fontConfig;
            config.outlineSize = 0;
            config.distanceFieldEnabled = true;
            setTTFConfig(config);
            _contentDirty = true;
        }
        _currLabelEffect.setOn(LabelEffect::GLOW);
        _effectColorF.r = glowColor.r / 255.0f;
        _effectColorF.g = glowColor.g / 255.0f;
        _effectColorF.b = glowColor.b / 255.0f;
        _effectColorF.a = glowColor.a / 255.0f;
        updateShaderProgram();
    }
}

void Label::enableGradient(const Color4B& startColor, const Color4B& endColor, float angle)
{
	if (_currentLabelType == LabelType::TTF)
	{
        _currLabelEffect.setOff(LabelEffect::NORMAL);
	    _currLabelEffect.setOn(LabelEffect::GRADIENT);
	    updateShaderProgram();
	}
}

void Label::enableOutline(const Color4B& outlineColor,int outlineSize /* = -1 */)
{
    CCASSERT(_currentLabelType == LabelType::STRING_TEXTURE
             || _currentLabelType == LabelType::TTF, "Only supported system font and TTF!");

    if (outlineSize > 0 || _currLabelEffect.isOn(LabelEffect::OUTLINE))
    {
        if (_currentLabelType == LabelType::TTF)
        {
            _effectColorF.r = outlineColor.r / 255.0f;
            _effectColorF.g = outlineColor.g / 255.0f;
            _effectColorF.b = outlineColor.b / 255.0f;
            _effectColorF.a = outlineColor.a / 255.0f;

            if (outlineSize > 0 && _fontConfig.outlineSize != outlineSize)
            {
                _fontConfig.outlineSize = outlineSize;
                setTTFConfig(_fontConfig);
            }
        }
        else if (_effectColorF != outlineColor || _outlineSize != outlineSize || _currLabelEffect.isOff(LabelEffect::OUTLINE))
        {
            _effectColorF.r = outlineColor.r / 255.f;
            _effectColorF.g = outlineColor.g / 255.f;
            _effectColorF.b = outlineColor.b / 255.f;
            _effectColorF.a = outlineColor.a / 255.f;
            _currLabelEffect.setOn(LabelEffect::OUTLINE);
            _contentDirty = true;
        }
        _outlineSize = outlineSize;
    }
}

void Label::enableShadow(const Color4B& shadowColor /* = Color4B::BLACK */,const Size &offset /* = Size(2 ,-2)*/, int blurRadius /* = 0 */)
{
    _currLabelEffect.setOn(LabelEffect::SHADOW);
    _shadowDirty = true;

    _shadowOffset.width = offset.width;
    _shadowOffset.height = offset.height;
    //TODO: support blur for shadow

    _shadowColor3B.r = shadowColor.r;
    _shadowColor3B.g = shadowColor.g;
    _shadowColor3B.b = shadowColor.b;
    _shadowOpacity = shadowColor.a;

    if (!_systemFontDirty && !_contentDirty && _textSprite)
    {
        auto fontDef = _getFontDefinition();
        if (_shadowNode)
        {
            if (shadowColor != _shadowColor4F)
            {
                _shadowNode = nullptr;
                createShadowSpriteForSystemFont(fontDef);
            }
            else
            {
                _shadowNode->setPosition(_shadowOffset.width, _shadowOffset.height);
            }
        }
        else
        {
            createShadowSpriteForSystemFont(fontDef);
        }
    }

    _shadowColor4F.r = shadowColor.r / 255.0f;
    _shadowColor4F.g = shadowColor.g / 255.0f;
    _shadowColor4F.b = shadowColor.b / 255.0f;
    _shadowColor4F.a = shadowColor.a / 255.0f;

    if (_currentLabelType == LabelType::BMFONT || _currentLabelType == LabelType::CHARMAP)
    {
        program_ = _currLabelEffect.isOn(LabelEffect::SHADOW) ? SharedRenderer.getDefaultProgramMVP() : SharedRenderer.getDefaultProgram();
    }
}

void Label::enableItalics()
{
    setRotationSkewX(12);
    _currLabelEffect.setOn(LabelEffect::ITALICS);
}

void Label::enableBold()
{
    if (!_currLabelEffect.isOn(LabelEffect::BOLD) && _currentLabelType != LabelType::STRING_TEXTURE)
    {
        // bold is implemented with outline
        enableShadow(Color4B::WHITE, Size(0.9f, 0), 0);
        // add one to kerning
        setAdditionalKerning(_additionalKerning+1);
    }
    _currLabelEffect.setOn(LabelEffect::BOLD);
}

void Label::enableUnderline()
{
    // remove it, just in case to prevent adding two or more
    if (!_underlineNode)
    {
        _underlineNode = DrawNode::create();
        addChild(_underlineNode, 100000);
        _contentDirty = true;
    }
}

void Label::enableStrikethrough()
{
    if (!_strikethroughEnabled)
    {
        enableUnderline();
        _strikethroughEnabled = true;
    }
}

void Label::disableEffect()
{
    disableEffect(LabelEffect::ALL);
}

void Label::disableEffect(LabelEffect effect)
{
	if (_currLabelEffect.isOn(effect))
	{
		Flag eflag(effect);
		if (eflag.isOn(LabelEffect::OUTLINE))
		{
			if (_currentLabelType == LabelType::TTF)
			{
				_fontConfig.outlineSize = 0;
				setTTFConfig(_fontConfig);
			}
			_currLabelEffect.setOff(LabelEffect::OUTLINE);
			_outlineSize = 0;
			_contentDirty = true;
		}

		if (eflag.isOn(LabelEffect::SHADOW))
		{
            _currLabelEffect.setOff(LabelEffect::SHADOW);
            _shadowNode = nullptr;
		}

		if (eflag.isOn(LabelEffect::GLOW))
		{
			_currLabelEffect.setOff(LabelEffect::GLOW);
		}

		if (eflag.isOn(LabelEffect::ITALICS))
		{
			setRotationSkewX(0);
            _currLabelEffect.setOff(LabelEffect::ITALICS);
		}

		if (eflag.isOn(LabelEffect::BOLD))
		{
            _currLabelEffect.setOff(LabelEffect::BOLD);
			_additionalKerning -= 1;
			
			if (_currLabelEffect.isOn(LabelEffect::SHADOW))
			{
                _currLabelEffect.setOff(LabelEffect::SHADOW);
				_shadowNode = nullptr;
			}
		}

		if (eflag.isOn(LabelEffect::UNDERLINE))
		{
			if (_underlineNode) {
				removeChild(_underlineNode);
				_underlineNode = nullptr;
			}
		}

		if (eflag.isOn(LabelEffect::STRIKETHROUGH))
		{
			_strikethroughEnabled = false;
			if (_underlineNode) {
				removeChild(_underlineNode);
				_underlineNode = nullptr;
			}
		}

		if (eflag.isOn(LabelEffect::GRADIENT))
		{
			_currLabelEffect.setOff(LabelEffect::GRADIENT);
			_gradientAngle = 0.0f;
		}

        if (_currLabelEffect.value() == 0)
        {
            _currLabelEffect.setOn(LabelEffect::NORMAL);
        }
		updateShaderProgram();
	}
}

void Label::createSpriteForSystemFont(const FontDefinition& fontDef)
{
    _currentLabelType = LabelType::STRING_TEXTURE;

    auto texture = new (std::nothrow) Texture2D;
    texture->initWithString(_utf8Text.c_str(), fontDef);

    _textSprite = Sprite::createWithTexture(texture);
    _textSprite->setGlobalZOrder(getGlobalZOrder());
    _textSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

    auto newSize = _textSprite->getContentSize();
    if(_overflow == Overflow::RESIZE_HEIGHT) {
        this->setContentSize(Size(_contentSize.width, newSize.height));
    } else {
        this->setContentSize(newSize);
    }

    texture->release();
    if (_blendFuncDirty)
    {
        _textSprite->setBlendFunc(_blendFunc);
    }

    _textSprite->updateDisplayedColor(_displayedColor);
    _textSprite->updateDisplayedOpacity(_displayedOpacity);
}

void Label::createShadowSpriteForSystemFont(const FontDefinition& fontDef)
{
    if (!fontDef._stroke._strokeEnabled && fontDef._fontFillColor == _shadowColor3B
        && (fontDef._fontAlpha == _shadowOpacity))
    {
        _shadowNode = Sprite::createWithTexture(_textSprite->getTexture());
    }
    else
    {
        FontDefinition shadowFontDefinition = fontDef;
        shadowFontDefinition._fontFillColor.r = _shadowColor3B.r;
        shadowFontDefinition._fontFillColor.g = _shadowColor3B.g;
        shadowFontDefinition._fontFillColor.b = _shadowColor3B.b;
        shadowFontDefinition._fontAlpha = _shadowOpacity;

        shadowFontDefinition._stroke._strokeColor = shadowFontDefinition._fontFillColor;
        shadowFontDefinition._stroke._strokeAlpha = shadowFontDefinition._fontAlpha;

        auto texture = new (std::nothrow) Texture2D;
        texture->initWithString(_utf8Text.c_str(), shadowFontDefinition);
        _shadowNode = Sprite::createWithTexture(texture);
        texture->release();
    }

    if (_shadowNode)
    {
        if (_blendFuncDirty)
        {
            _shadowNode->setBlendFunc(_blendFunc);
        }
        _shadowNode->setGlobalZOrder(getGlobalZOrder());
        _shadowNode->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        _shadowNode->setPosition(_shadowOffset.width, _shadowOffset.height);

        _shadowNode->updateDisplayedColor(_displayedColor);
        _shadowNode->updateDisplayedOpacity(_displayedOpacity);
    }
}

void Label::setFontDefinition(const FontDefinition& textDefinition)
{
    _systemFont = textDefinition._fontName;
    _systemFontSize = textDefinition._fontSize;
    _hAlignment = textDefinition._alignment;
    _vAlignment = textDefinition._vertAlignment;
    setDimensions(textDefinition._dimensions.width, textDefinition._dimensions.height);
    Color4B textColor = Color4B(textDefinition._fontFillColor);
    textColor.a = textDefinition._fontAlpha;
    setTextColor(textColor);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID) && (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
    if (textDefinition._stroke._strokeEnabled)
    {
        CCLOGERROR("Stroke Currently only supported on iOS and Android!");
    }
    _outlineSize = 0.f;
#else
    if (textDefinition._stroke._strokeEnabled && textDefinition._stroke._strokeSize > 0.f)
    {
        Color4B outlineColor = Color4B(textDefinition._stroke._strokeColor);
        outlineColor.a = textDefinition._stroke._strokeAlpha;
        enableOutline(outlineColor, textDefinition._stroke._strokeSize);
    }
#endif

    if (textDefinition._shadow._shadowEnabled)
    {
        enableShadow(Color4B(0, 0, 0, 255 * textDefinition._shadow._shadowOpacity),
            textDefinition._shadow._shadowOffset, textDefinition._shadow._shadowBlur);
    }
}

void Label::updateContent()
{
    if (_systemFontDirty)
    {
        if (_fontAtlas)
        {
            _batchNodes.clear();

            FontAtlasCache::releaseFontAtlas(_fontAtlas);
            _fontAtlas = nullptr;
        }

        _systemFontDirty = false;
    }

    _textSprite = nullptr;
    _shadowNode = nullptr;
    bool updateFinished = true;

    if (_fontAtlas)
    {
        std::u16string utf16String;
        if (StringUtils::UTF8ToUTF16(_utf8Text, utf16String))
        {
            _utf16Text = utf16String;
        }

        computeHorizontalKernings(_utf16Text);
        updateFinished = alignText();
    }
    else
    {
        auto fontDef = _getFontDefinition();
        createSpriteForSystemFont(fontDef);
        if (_currLabelEffect.isOn(LabelEffect::SHADOW))
        {
            createShadowSpriteForSystemFont(fontDef);
        }
    }

    if (_underlineNode)
    {
        _underlineNode->clear();

        if(_currentLabelType == Label::LabelType::STRING_TEXTURE) {
            // system font
            const auto spriteSize = _textSprite->getContentSize();

            this->computeStringNumLines();
            float startY = spriteSize.height / 2 - _lineHeight * (_numberOfLines - 1) / 2 - _systemFontSize / 2;
            if (_numberOfLines > 0) {
                // atlas font
                _underlineNode->setLineWidth(_systemFontSize / 8);
                for (int i=0; i<_numberOfLines; ++i)
                {
                    if (_strikethroughEnabled)
                        startY += _systemFontSize / 2;
                    // FIXME: Might not work with different vertical alignments
                    _underlineNode->drawLine(Vec2(0, startY - 1),
                                             Vec2(spriteSize.width, startY - 1), _textColorF);
                }
            }

        }

        else if(_numberOfLines)
        {
            const float charheight = (_textDesiredHeight / _numberOfLines);
            _underlineNode->setLineWidth(charheight/6);

            // atlas font
            for (int i=0; i<_numberOfLines; ++i)
            {
                float offsety = 0;
                if (_strikethroughEnabled)
                    offsety += charheight / 2;
                // FIXME: Might not work with different vertical alignments
                float y = (_numberOfLines - i - 1) * charheight + offsety;
                _underlineNode->drawLine(Vec2(_linesOffsetX[i],y), Vec2(_linesWidth[i] + _linesOffsetX[i],y), _textColorF);
            }
        }
    }

    if(updateFinished){
        _contentDirty = false;
    }

    if (_currLabelEffect.isOn(LabelEffect::ITALICS)) {
        _contentSize.width += this->_originalFontSize * tan(6 * 0.0174532925);
    }

#if CC_LABEL_DEBUG_DRAW
    _debugDrawNode->clear();
    Vec2 vertices[4] =
    {
        Vec2::ZERO,
        Vec2(_contentSize.width, 0),
        Vec2(_contentSize.width, _contentSize.height),
        Vec2(0, _contentSize.height)
    };
    _debugDrawNode->drawPoly(vertices, 4, true, Color4F::WHITE);
#endif
}

void Label::setBMFontSize(float fontSize)
{
    this->setBMFontSizeInternal(fontSize);
    _originalFontSize = fontSize;
}

float Label::getBMFontSize()const
{
    return _bmFontSize;
}

void Label::setGradientColorStart(const Color4B& startColor)
{
	_textGradientStartColor.r = startColor.r / 255.0f;
	_textGradientStartColor.g = startColor.g / 255.0f;
	_textGradientStartColor.b = startColor.b / 255.0f;
	_textGradientStartColor.a = startColor.a / 255.0f;
}

void Label::setGradientColorEnd(const Color4B& endColor)
{
	_textGradientEndColor.r = endColor.r / 255.0f;
	_textGradientEndColor.g = endColor.g / 255.0f;
	_textGradientEndColor.b = endColor.b / 255.0f;
	_textGradientEndColor.a = endColor.a / 255.0f;
}

void Label::onDrawShadow(GLProgram* glProgram, const Color4F& shadowColor)
{
	
    if (_currentLabelType == LabelType::TTF)
    {
        program_->set("u_textColor"_slice, shadowColor.r, shadowColor.g, shadowColor.b, shadowColor.a);
        if (_currLabelEffect.isOn(LabelEffect::OUTLINE) || _currLabelEffect.isOn(LabelEffect::GLOW) || _currLabelEffect.isOn(LabelEffect::GRADIENT))
        {
            program_->set("u_startColor"_slice, shadowColor.r, shadowColor.g, shadowColor.b, shadowColor.a);
            program_->set("u_endColor"_slice, shadowColor.r, shadowColor.g, shadowColor.b, shadowColor.a);
        }

        uint64_t state = (
            BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
            BGFX_STATE_MSAA | _blendFunc.toValue());
        for (auto&& it : _letters)
        {
            it.second->updateTransform();
        }
        for (auto&& batchNode : _batchNodes)
        {
            batchNode->getTextureAtlas()->drawQuads(program_, state, &_shadowTransform);
        }
    }
    else
    {
        Color3B oldColor = _realColor;
        GLubyte oldOPacity = _displayedOpacity;
        _displayedOpacity = shadowColor.a * 255;
        setColor(Color3B(shadowColor));

        uint64_t state = (
            BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
            BGFX_STATE_MSAA | _blendFunc.toValue());
        for (auto&& it : _letters)
        {
            it.second->updateTransform();
        }
        for (auto&& batchNode : _batchNodes)
        {
            batchNode->getTextureAtlas()->drawQuads(program_, state, &_shadowTransform);
        }

        _displayedOpacity = oldOPacity;
        setColor(oldColor);
    }
}

void Label::onDraw(const Mat4& transform, bool transformUpdated)
{
    if (_currLabelEffect.isOn(LabelEffect::SHADOW))
    {
        if (_currLabelEffect.isOn(LabelEffect::BOLD))
            onDrawShadow(nullptr, _textColorF);
        else
            onDrawShadow(nullptr, _shadowColor4F);
    }

    for (auto&& it : _letters)
    {
        it.second->updateTransform();
    }

    uint64_t state = (
        BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
        BGFX_STATE_MSAA | _blendFunc.toValue());

    if (_currentLabelType == LabelType::TTF)
    {
		if (_currLabelEffect.isOn(LabelEffect::GRADIENT) && _currLabelEffect.isOn(LabelEffect::OUTLINE))
		{
			//draw text with outline
			program_->set("u_textColor"_slice,
				_textColorF.r, _textColorF.g, _textColorF.b, _textColorF.a);
            program_->set("u_effectColor"_slice ,
				_effectColorF.r, _effectColorF.g, _effectColorF.b, _effectColorF.a);

            program_->set("u_startColor"_slice, _textGradientStartColor.r, _textGradientStartColor.g, _textGradientStartColor.b, _textGradientStartColor.a);
            program_->set("u_endColor"_slice, _textGradientEndColor.r, _textGradientEndColor.g, _textGradientEndColor.b, _textGradientEndColor.a);
            program_->set("u_labelWidth"_slice, _contentSize.width);
            program_->set("u_labelHeight"_slice, _contentSize.height);
            program_->set("u_angle"_slice, _gradientAngle);

			for (auto&& batchNode : _batchNodes)
			{
				batchNode->getTextureAtlas()->drawQuads(program_, state, &transform);
			}

			//draw text without outline
            program_->set("u_effectColor"_slice, _effectColorF.r, _effectColorF.g, _effectColorF.b, 0.f);
		}
		else if (_currLabelEffect.isOn(LabelEffect::OUTLINE))
		{
			//draw text with outline
            program_->set("u_textColor"_slice, _textColorF.r, _textColorF.g, _textColorF.b, _textColorF.a);
            program_->set("u_effectColor"_slice, _effectColorF.r, _effectColorF.g, _effectColorF.b, _effectColorF.a);

			for (auto&& batchNode : _batchNodes)
			{
				batchNode->getTextureAtlas()->drawQuads(program_, state, &transform);
			}

			//draw text without outline
            program_->set("u_effectColor"_slice, _effectColorF.r, _effectColorF.g, _effectColorF.b, 0.f);
		}
		else if (_currLabelEffect.isOn(LabelEffect::GRADIENT))
		{
            program_->set("u_textColor"_slice, _textColorF.r, _textColorF.g, _textColorF.b, _textColorF.a);
            program_->set("u_startColor"_slice, _textGradientStartColor.r, _textGradientStartColor.g, _textGradientStartColor.b, _textGradientStartColor.a);
            program_->set("u_endColor"_slice, _textGradientEndColor.r, _textGradientEndColor.g, _textGradientEndColor.b, _textGradientEndColor.a);
            program_->set("u_labelWidth"_slice, _contentSize.width);
            program_->set("u_labelHeight"_slice, _contentSize.height);
            program_->set("u_angle"_slice, _gradientAngle);
		}
		else
		{
			if (_currLabelEffect.isOn(LabelEffect::GLOW))
			{
                program_->set("u_effectColor"_slice, _effectColorF.r, _effectColorF.g, _effectColorF.b, _effectColorF.a);
			}
			if (_currLabelEffect.isOn(LabelEffect::NORMAL))
			{
                program_->set("u_textColor"_slice, _textColorF.r, _textColorF.g, _textColorF.b, _textColorF.a);
			}
		}
    }

	if(_colorIndexNum.size() > 0)
	{
        TextureAtlas* textureAtlas = nullptr;
        for (int ctr = 0; ctr < _lengthOfString; ++ctr)
        {
            auto& letterInfo = _lettersInfo[ctr];
            if (letterInfo.valid && letterInfo.atlasIndex >= 0)
            {
                auto& letterDef = _fontAtlas->_letterDefinitions[letterInfo.utf16Char];

                auto batchNode = _batchNodes.at(letterDef.textureID);
                textureAtlas = batchNode->getTextureAtlas();
                if (letterInfo.colorIndex >= 0)
                {
                    ssize_t count = textureAtlas->getTotalQuads();
                    if (letterInfo.atlasIndex < count)
                    {
                        program_->set("u_textColor"_slice, 1.0f, 1.0f, 1.0f, _textColorF.a);
                        textureAtlas->drawNumberOfQuads(1, letterInfo.atlasIndex, program_, state, &transform);
                    }
                }
                else
                {
                    program_->set("u_textColor"_slice, _textColorF.r, _textColorF.g, _textColorF.b, _textColorF.a);
                    textureAtlas->drawNumberOfQuads(1, letterInfo.atlasIndex, program_, state, &transform);
                }
            }
        }
	}
	else
	{
		for (auto&& batchNode : _batchNodes)
		{
			batchNode->getTextureAtlas()->drawQuads(program_, state, &transform);
		}
	}
}

void Label::draw(IRenderer *renderer, const Mat4 &transform, uint32_t flags)
{
    if (_batchNodes.empty() || _lengthOfString <= 0)
    {
        return;
    }

    if (!_currLabelEffect.isOn(LabelEffect::SHADOW) && (_currentLabelType == LabelType::BMFONT || _currentLabelType == LabelType::CHARMAP))
    {
        for (auto&& it : _letters)
        {
            it.second->updateTransform();
        }
        // ETC1 ALPHA supports for BMFONT & CHARMAP
        auto textureAtlas = _batchNodes.at(0)->getTextureAtlas();
        auto texture = textureAtlas->getTexture();

        /*_quadCommand.init(_globalZOrder, texture, getGLProgramState(),
            _blendFunc, textureAtlas->getQuads(), textureAtlas->getTotalQuads(), transform, flags);
        renderer->addCommand(&_quadCommand);*/
        //SharedRenderer.push(textureAtlas->getQuads(), textureAtlas->getTotalQuads(), )
        CCAssertIf(true, "bmfont or charmap no shadow");
    }
    else
    {
        onDraw(transform, flags & FLAGS_TRANSFORM_DIRTY);
    }
}

void Label::visit(IRenderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if (flags_.isOff(Node::Visible) || (_utf8Text.empty() && _children.empty()) )
    {
        return;
    }

    if (_systemFontDirty || _contentDirty)
    {
        updateContent();
    }

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    if (!_utf8Text.empty() && _currLabelEffect.isOn(LabelEffect::SHADOW) && (_shadowDirty || (flags & FLAGS_DIRTY_MASK)))
    {
        _position.x += _shadowOffset.width;
        _position.y += _shadowOffset.height;
        flags_.setOn(Node::TransformDirty);

        _shadowTransform = transform(parentTransform);

        _position.x -= _shadowOffset.width;
        _position.y -= _shadowOffset.height;
        flags_.setOn(Node::TransformDirty);

        _shadowDirty = false;
    }


    if (!_children.empty())
    {
        sortAllChildren();

        int i = 0;
        // draw children zOrder < 0
        for (; i < _children.size(); i++)
        {
            auto node = _children.at(i);

            if (node && node->getLocalZOrder() < 0)
                node->visit(renderer, _modelViewTransform, flags);
            else
                break;
        }

        this->drawSelf(renderer, flags);

        for (auto it = _children.cbegin() + i; it != _children.cend(); ++it)
        {
            (*it)->visit(renderer, _modelViewTransform, flags);
        }
    }
    else
    {
        this->drawSelf(renderer, flags);
    }
}

void Label::drawSelf(IRenderer* renderer, uint32_t flags)
{
    if (_textSprite)
    {
        if (_shadowNode)
        {
            _shadowNode->visit(renderer, _modelViewTransform, flags);
        }
        _textSprite->visit(renderer, _modelViewTransform, flags);
    }
    else if (!_utf8Text.empty())
    {
        draw(renderer, _modelViewTransform, flags);
    }
}

void Label::setSystemFontName(const std::string& systemFont)
{
    if (systemFont != _systemFont)
    {
        _systemFont = systemFont;
        _currentLabelType = LabelType::STRING_TEXTURE;
        _systemFontDirty = true;
    }
}

void Label::setSystemFontSize(float fontSize)
{
    if (_systemFontSize != fontSize)
    {
        _systemFontSize = fontSize;
        _originalFontSize = fontSize;
        _currentLabelType = LabelType::STRING_TEXTURE;
        _systemFontDirty = true;
    }
}

///// PROTOCOL STUFF
Sprite* Label::getLetter(int letterIndex)
{
    Sprite* letter = nullptr;
    do
    {
        if (_systemFontDirty || _currentLabelType == LabelType::STRING_TEXTURE)
        {
            break;
        }

        auto contentDirty = _contentDirty;
        if (contentDirty)
        {
            updateContent();
        }

        if (_textSprite == nullptr && letterIndex < _lengthOfString)
        {
            const auto &letterInfo = _lettersInfo[letterIndex];
            if (!letterInfo.valid)
            {
                break;
            }

            if (_letters.find(letterIndex) != _letters.end())
            {
                letter = _letters[letterIndex];
            }

            if (letter == nullptr)
            {
                auto& letterDef = _fontAtlas->_letterDefinitions[letterInfo.utf16Char];
                auto textureID = letterDef.textureID;
                Rect uvRect;
                uvRect.size.height = letterDef.height;
                uvRect.size.width = letterDef.width;
                uvRect.origin.x = letterDef.U;
                uvRect.origin.y = letterDef.V;

                if (letterDef.width <= 0.f || letterDef.height <= 0.f)
                {
                    letter = LabelLetter::create();
                }
                else
                {
                    letter = LabelLetter::createWithTexture(_fontAtlas->getTexture(textureID), uvRect);
                    letter->setTextureAtlas(_batchNodes.at(textureID)->getTextureAtlas());
                    letter->setAtlasIndex(letterInfo.atlasIndex);
                    auto px = letterInfo.positionX + uvRect.size.width / 2 + _linesOffsetX[letterInfo.lineIndex];
                    auto py = letterInfo.positionY - uvRect.size.height / 2 + _letterOffsetY;
                    letter->setPosition(px,py);
                    letter->setOpacity(_realOpacity);
                }

                addChild(letter);
                _letters[letterIndex] = letter;
            }
        }
    } while (false);

    return letter;
}

void Label::setLineHeight(float height)
{
    CCASSERT(_currentLabelType != LabelType::STRING_TEXTURE, "Not supported system font!");

    if (_lineHeight != height)
    {
        _lineHeight = height;
        _contentDirty = true;
    }
}

float Label::getLineHeight() const
{
    CCASSERT(_currentLabelType != LabelType::STRING_TEXTURE, "Not supported system font!");
    return _textSprite ? 0.0f : _lineHeight * _bmfontScale;
}

void Label::setLineSpacing(float height)
{
    if (_lineSpacing != height)
    {
        _lineSpacing = height;
        _contentDirty = true;
    }
}

float Label::getLineSpacing() const
{
    return _lineSpacing;
}

void Label::setAdditionalKerning(float space)
{

    if (_currentLabelType != LabelType::STRING_TEXTURE)
    {
        if (_additionalKerning != space)
        {
            _additionalKerning = space;
            _contentDirty = true;
        }
    }
    else
        CCLOG("Label::setAdditionalKerning not supported on LabelType::STRING_TEXTURE");
}

float Label::getAdditionalKerning() const
{
    CCASSERT(_currentLabelType != LabelType::STRING_TEXTURE, "Not supported system font!");

    return _additionalKerning;
}

void Label::computeStringNumLines()
{
    int quantityOfLines = 1;

    if (_utf16Text.empty())
    {
        _numberOfLines = 0;
        return;
    }

    // count number of lines
    size_t stringLen = _utf16Text.length();
    for (size_t i = 0; i < stringLen - 1; ++i)
    {
        if (_utf16Text[i] == (char16_t)TextFormatter::NewLine)
        {
            quantityOfLines++;
        }
    }

    _numberOfLines = quantityOfLines;
}

int Label::getStringNumLines()
{
    if (_contentDirty)
    {
        updateContent();
    }

    if (_currentLabelType == LabelType::STRING_TEXTURE)
    {
        computeStringNumLines();
    }

    return _numberOfLines;
}

int Label::getStringLength()
{
    _lengthOfString = static_cast<int>(_utf16Text.length());
    return _lengthOfString;
}

// RGBA protocol
void Label::setOpacityModifyRGB(bool isOpacityModifyRGB)
{
    if (isOpacityModifyRGB != _isOpacityModifyRGB)
    {
        _isOpacityModifyRGB = isOpacityModifyRGB;
        updateColor();
    }
}

void Label::updateDisplayedColor(const Color3B& parentColor)
{
    Node::updateDisplayedColor(parentColor);

    if (_textSprite)
    {
        _textSprite->updateDisplayedColor(_displayedColor);
    }

    if (_shadowNode)
    {
        _shadowNode->updateDisplayedColor(_displayedColor);
    }

    if (_underlineNode)
    {
        _contentDirty = true;
    }

    for (auto&& it : _letters)
    {
        it.second->updateDisplayedColor(_displayedColor);;
    }
}

void Label::updateDisplayedOpacity(GLubyte parentOpacity)
{
    Node::updateDisplayedOpacity(parentOpacity);

    if (_textSprite)
    {
        _textSprite->updateDisplayedOpacity(_displayedOpacity);
        if (_shadowNode)
        {
            _shadowNode->updateDisplayedOpacity(_displayedOpacity);
        }
    }

    for (auto&& it : _letters)
    {
        it.second->updateDisplayedOpacity(_displayedOpacity);;
    }
    if (_underlineNode) {
        _underlineNode->updateDisplayedOpacity(_displayedOpacity);
    }
}

void Label::setTextColor(const Color4B &color)
{
    CCASSERT(_currentLabelType == LabelType::TTF || _currentLabelType == LabelType::STRING_TEXTURE, "Only supported system font and ttf!");

    if (_currentLabelType == LabelType::STRING_TEXTURE && _textColor != color)
    {
        _contentDirty = true;
    }
    _textColor = color;
    _textColorF.r = _textColor.r / 255.0f;
    _textColorF.g = _textColor.g / 255.0f;
    _textColorF.b = _textColor.b / 255.0f;
    _textColorF.a = _textColor.a / 255.0f;
}

void Label::updateColor()
{
    if (_batchNodes.empty())
    {
        return;
    }

    Color4B color4( _displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity );

    // special opacity for premultiplied textures
    if (_isOpacityModifyRGB)
    {
        color4.r *= _displayedOpacity/255.0f;
        color4.g *= _displayedOpacity/255.0f;
        color4.b *= _displayedOpacity/255.0f;
    }
	_effectColorF.a = _displayedOpacity / 255.0f;
    cocos2d::TextureAtlas* textureAtlas = nullptr;
    V3F_C4B_T2F_Quad *quads = nullptr;
    for(int ctr = 0; ctr < _lengthOfString; ++ctr)
    {
        auto& letterInfo = _lettersInfo[ctr];
        if (letterInfo.valid)
        {
            if (_colorSection.size() > 0 && letterInfo.colorIndex >= 0) 
            {
                color4 = Color4B(_colorSection[letterInfo.colorIndex]);
                color4.a = _displayedOpacity;
                if (_isOpacityModifyRGB)
                {
                    color4.r *= _displayedOpacity / 255.0f;
                    color4.g *= _displayedOpacity / 255.0f;
                    color4.b *= _displayedOpacity / 255.0f;
                }
            }
            else
            {
                color4 = Color4B(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);
            }

            auto& letterDef = _fontAtlas->_letterDefinitions[letterInfo.utf16Char];

            auto batchNode = _batchNodes.at(letterDef.textureID);
            textureAtlas = batchNode->getTextureAtlas();
            quads = textureAtlas->getQuads();

            ssize_t count = textureAtlas->getTotalQuads();
            if (letterInfo.atlasIndex >= 0 && letterInfo.atlasIndex < count)
            {
                auto& quad = quads[letterInfo.atlasIndex];
                quad.bl.colors = color4;
                quad.br.colors = color4;
                quad.tl.colors = color4;
                quad.tr.colors = color4;
                textureAtlas->updateQuad(&quad, letterInfo.atlasIndex);
            }
        }
    }

    /*V3F_C4B_T2F_Quad *quads;
    for (auto&& batchNode:_batchNodes)
    {
        textureAtlas = batchNode->getTextureAtlas();
        quads = textureAtlas->getQuads();
        auto count = textureAtlas->getTotalQuads();

        if (_colorSection.size() > 0)
        {
            for (int index = 0; index < count; ++index)
            {
                bool isColored = false;
                int colorIndex = 0;
                auto iter = std::find_if(_colorIndexNum.begin(), _colorIndexNum.end(), [=](const auto& it) -> bool {
                    return index >= it.start && index <= it.end;
                });
                if (iter != _colorIndexNum.end())
                {
                    colorIndex = iter - _colorIndexNum.begin();
                    isColored = true;
                }
                if (isColored)
                {
                    Color4B color4 = Color4B(_colorSection[colorIndex]);
                    color4.a = _displayedOpacity;
                    if (_isOpacityModifyRGB) {
                        color4.r *= _displayedOpacity / 255.0f;
                        color4.g *= _displayedOpacity / 255.0f;
                        color4.b *= _displayedOpacity / 255.0f;
                    }

                    quads[index].bl.colors = color4;
                    quads[index].br.colors = color4;
                    quads[index].tl.colors = color4;
                    quads[index].tr.colors = color4;
                    textureAtlas->updateQuad(&quads[index], index);
                }
                else
                {
                    quads[index].bl.colors = color4;
                    quads[index].br.colors = color4;
                    quads[index].tl.colors = color4;
                    quads[index].tr.colors = color4;
                    textureAtlas->updateQuad(&quads[index], index);
                }
            }
        }
		else
		{
            for (int index = 0; index < count; ++index)
            {
			    quads[index].bl.colors = color4;
			    quads[index].br.colors = color4;
			    quads[index].tl.colors = color4;
			    quads[index].tr.colors = color4;
			    textureAtlas->updateQuad(&quads[index], index);
		    }
        }
    }*/
}

std::string Label::getDescription() const
{
    char tmp[50];
    sprintf(tmp, "<Label | Tag = %d, Label = >", _tag);
    std::string ret = tmp;
    ret += _utf8Text;

    return ret;
}

const Size& Label::getContentSize() const
{
    if (_systemFontDirty || _contentDirty)
    {
        const_cast<Label*>(this)->updateContent();
    }
    return _contentSize;
}

Rect Label::getBoundingBox() const
{
    const_cast<Label*>(this)->getContentSize();

    return Node::getBoundingBox();
}

void Label::setBlendFunc(const BlendFunc &blendFunc)
{
    _blendFunc = blendFunc;
    _blendFuncDirty = true;
    if (_textSprite)
    {
        _textSprite->setBlendFunc(blendFunc);
        if (_shadowNode)
        {
            _shadowNode->setBlendFunc(blendFunc);
        }
    }
}

void Label::removeAllChildrenWithCleanup(bool cleanup)
{
    Node::removeAllChildrenWithCleanup(cleanup);
    _letters.clear();
}

void Label::removeChild(Node* child, bool cleanup /* = true */)
{
    Node::removeChild(child, cleanup);
    for (auto&& it : _letters)
    {
        if (it.second == child)
        {
            _letters.erase(it.first);
            break;
        }
    }
}

FontDefinition Label::_getFontDefinition() const
{
    FontDefinition systemFontDef;
    systemFontDef._fontName = _systemFont;
    systemFontDef._fontSize = _systemFontSize;
    systemFontDef._alignment = _hAlignment;
    systemFontDef._vertAlignment = _vAlignment;
    systemFontDef._dimensions.width = _labelWidth;
    systemFontDef._dimensions.height = _labelHeight;
    systemFontDef._fontFillColor.r = _textColor.r;
    systemFontDef._fontFillColor.g = _textColor.g;
    systemFontDef._fontFillColor.b = _textColor.b;
    systemFontDef._fontAlpha = _textColor.a;
    systemFontDef._shadow._shadowEnabled = false;
    systemFontDef._enableWrap = _enableWrap;
    systemFontDef._overflow = (int)_overflow;
    systemFontDef._enableBold = _currLabelEffect.isOn(LabelEffect::BOLD);

    if (_currLabelEffect.isOn(LabelEffect::OUTLINE) && _outlineSize > 0.f)
    {
        systemFontDef._stroke._strokeEnabled = true;
        systemFontDef._stroke._strokeSize = _outlineSize;
        systemFontDef._stroke._strokeColor.r = _effectColorF.r * 255;
        systemFontDef._stroke._strokeColor.g = _effectColorF.g * 255;
        systemFontDef._stroke._strokeColor.b = _effectColorF.b * 255;
        systemFontDef._stroke._strokeAlpha = _effectColorF.a * 255;
    }
    else
    {
        systemFontDef._stroke._strokeEnabled = false;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    if (systemFontDef._stroke._strokeEnabled)
    {
        CCLOGERROR("Currently stroke doesn't support win32!");
    }
    systemFontDef._stroke._strokeEnabled = false;
#endif

    return systemFontDef;
}

void Label::setGlobalZOrder(float globalZOrder)
{
    Node::setGlobalZOrder(globalZOrder);
    if (_textSprite)
    {
        _textSprite->setGlobalZOrder(globalZOrder);
        if (_shadowNode)
        {
            _shadowNode->setGlobalZOrder(globalZOrder);
        }
    }
}

float Label::getRenderingFontSize()const
{
    float fontSize;
    if (_currentLabelType == LabelType::BMFONT) {
        fontSize = _bmFontSize;
    }else if(_currentLabelType == LabelType::TTF){
        fontSize = this->getTTFConfig().fontSize;
    }else if(_currentLabelType == LabelType::STRING_TEXTURE){
        fontSize = _systemFontSize;
    }else{ //FIXME: find a way to calculate char map font size
        fontSize = this->getLineHeight();
    }
    return fontSize;
}

void Label::enableWrap(bool enable)
{
    if(enable == _enableWrap || _overflow == Overflow::RESIZE_HEIGHT){
        return;
    }

    this->_enableWrap = enable;

    this->rescaleWithOriginalFontSize();

    _contentDirty = true;
}

bool Label::isWrapEnabled()const
{
    return this->_enableWrap;
}

void Label::setOverflow(Overflow overflow)
{
    if(_overflow == overflow){
        return;
    }

    if (_currentLabelType == LabelType::CHARMAP) {
        if (overflow == Overflow::SHRINK) {
            return;
        }
    }

    if(overflow == Overflow::RESIZE_HEIGHT){
        this->setDimensions(_labelDimensions.width,0);
        this->enableWrap(true);
    }
    _overflow = overflow;

    this->rescaleWithOriginalFontSize();

    _contentDirty = true;
}

void Label::rescaleWithOriginalFontSize()
{
    auto renderingFontSize = this->getRenderingFontSize();
    if (_originalFontSize - renderingFontSize >= 1) {
        this->scaleFontSizeDown(_originalFontSize);
    }
}

Label::Overflow Label::getOverflow()const
{
    return _overflow;
}

void Label::updateLetterSpriteScale(Sprite* sprite)
{
    if (_currentLabelType == LabelType::BMFONT && _bmFontSize > 0)
    {
        sprite->setScale(_bmfontScale);
    }
    else
    {
        if (std::abs(_bmFontSize) < FLT_EPSILON)
        {
            sprite->setScale(0);
        }
        else
        {
            sprite->setScale(1.0);
        }
    }
}

NS_CC_END
