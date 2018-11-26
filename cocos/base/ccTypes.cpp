/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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
#include "base/ccTypes.h"

NS_CC_BEGIN

const std::string STD_STRING_EMPTY("");
const ssize_t CC_INVALID_INDEX = -1;

int cocosType = 1;

/**
 * Color3B
 */

Color3B::Color3B()
: r(0)
, g(0)
, b(0)
{}

Color3B::Color3B(GLubyte _r, GLubyte _g, GLubyte _b)
: r(_r)
, g(_g)
, b(_b)
{}

Color3B::Color3B(const Color4B& color)
: r(color.r)
, g(color.g)
, b(color.b)
{}

Color3B::Color3B(const Color4F& color)
: r(color.r * 255.0f)
, g(color.g * 255.0f)
, b(color.b * 255.0f)
{}

Color3B::Color3B(std::u16string hex_str)
{
	if (hex_str.length() != 2 * 3)
	{
		r = g = b = 0.0f;
	}
	else
	{
		std::transform(hex_str.begin(), hex_str.end(), hex_str.begin(), std::ptr_fun<int, int>(std::toupper));

		auto toChar16 = [](const char16_t c) -> char16_t
		{
			if (c > '9') //ABCDEF
			{
				return c - 'A' + 0xA;
			}
			else //0-9
			{
				return c - '0';
			}
		};

		r = toChar16(hex_str[0]) * 16 + toChar16(hex_str[1]);
		g = toChar16(hex_str[2]) * 16 + toChar16(hex_str[3]);
		b = toChar16(hex_str[4]) * 16 + toChar16(hex_str[5]);
	}
}

bool Color3B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b);
}

bool Color3B::operator==(const Color4B& right) const
{
    return (r == right.r && g == right.g && b == right.b && 255 == right.a);
}

bool Color3B::operator==(const Color4F& right) const
{
    return (right.a == 1.0f && Color4F(*this) == right);
}

bool Color3B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

/**
 * Color4B
 */

Color4B::Color4B()
: r(0)
, g(0)
, b(0)
, a(0)
{}

Color4B::Color4B(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
: r(_r)
, g(_g)
, b(_b)
, a(_a)
{}

Color4B::Color4B(uint32_t argb)
: a(argb >> 24)
, r((argb & 0x00ff0000) >> 16)
, g((argb & 0x0000ff00) >> 8)
, b(argb & 0x000000ff)
{

}

Color4B::Color4B(const Color3B& color, GLubyte _a)
: r(color.r)
, g(color.g)
, b(color.b)
, a(_a)
{}

Color4B::Color4B(const Color4F& color)
: r(color.r * 255)
, g(color.g * 255)
, b(color.b * 255)
, a(color.a * 255)
{}

bool Color4B::operator==(const Color4B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == 255);
}

bool Color4B::operator==(const Color4F& right) const
{
    return (*this == Color4B(right));
}

bool Color4B::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

bool Color4B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color4B::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

const Color4B& Color4B::operator=(const Color4F& right)
{
    r = right.r * 255;
    g = right.g * 255;
    b = right.b * 255;
    a = right.a * 255;
    return *this;
}

uint32_t Color4B::toABGR() const
{
    return *reinterpret_cast<uint32_t*>(const_cast<Color4B*>(this));
}

uint32_t Color4B::toRGBA() const
{
    return r << 24 | g << 16 | b << 8 | a;
}

/**
 * Color4F
 */

Color4F::Color4F()
: r(0.0f)
, g(0.0f)
, b(0.0f)
, a(0.0f)
{}

Color4F::Color4F(float _r, float _g, float _b, float _a)
: r(_r)
, g(_g)
, b(_b)
, a(_a)
{}

Color4F::Color4F(const Color3B& color, float _a)
: r(color.r / 255.0f)
, g(color.g / 255.0f)
, b(color.b / 255.0f)
, a(_a)
{}

Color4F::Color4F(const Color4B& color)
: r(color.r / 255.0f)
, g(color.g / 255.0f)
, b(color.b / 255.0f)
, a(color.a / 255.0f)
{}

Vec4 Color4F::toVec4() const
{
    return Vec4{ r, g, b, a };
}

bool Color4F::operator==(const Color4F& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4F::operator==(const Color3B& right) const
{
    return (a == 1.0f && Color3B(*this) == right);
}

bool Color4F::operator==(const Color4B& right) const
{
    return (*this == Color4F(right));
}

bool Color4F::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

bool Color4F::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color4F::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

/**
 * Color constants
 */

const Color3B Color3B::WHITE  (255, 255, 255);
const Color3B Color3B::YELLOW (255, 255,   0);
const Color3B Color3B::GREEN  (  0, 255,   0);
const Color3B Color3B::BLUE   (  0,   0, 255);
const Color3B Color3B::RED    (255,   0,   0);
const Color3B Color3B::MAGENTA(255,   0, 255);
const Color3B Color3B::BLACK  (  0,   0,   0);
const Color3B Color3B::ORANGE (255, 127,   0);
const Color3B Color3B::GRAY   (166, 166, 166);

const Color4B Color4B::WHITE  (255, 255, 255, 255);
const Color4B Color4B::YELLOW (255, 255,   0, 255);
const Color4B Color4B::GREEN  (  0, 255,   0, 255);
const Color4B Color4B::BLUE   (  0,   0, 255, 255);
const Color4B Color4B::RED    (255,   0,   0, 255);
const Color4B Color4B::MAGENTA(255,   0, 255, 255);
const Color4B Color4B::BLACK  (  0,   0,   0, 255);
const Color4B Color4B::ORANGE (255, 127,   0, 255);
const Color4B Color4B::GRAY   (166, 166, 166, 255);

const Color4F Color4F::WHITE  (    1,     1,     1, 1);
const Color4F Color4F::YELLOW (    1,     1,     0, 1);
const Color4F Color4F::GREEN  (    0,     1,     0, 1);
const Color4F Color4F::BLUE   (    0,     0,     1, 1);
const Color4F Color4F::RED    (    1,     0,     0, 1);
const Color4F Color4F::MAGENTA(    1,     0,     1, 1);
const Color4F Color4F::BLACK  (    0,     0,     0, 1);
const Color4F Color4F::ORANGE (    1,  0.5f,     0, 1);
const Color4F Color4F::GRAY   (0.65f, 0.65f, 0.65f, 1);

const BlendFunc BlendFunc::DISABLE{BlendFunc::One, BlendFunc::Zero};
const BlendFunc BlendFunc::ALPHA_PREMULTIPLIED{BlendFunc::One, BlendFunc::InvSrcAlpha};
const BlendFunc BlendFunc::ALPHA_NON_PREMULTIPLIED{BlendFunc::SrcAlpha, BlendFunc::InvSrcAlpha};
const BlendFunc BlendFunc::ADDITIVE{BlendFunc::SrcAlpha, BlendFunc::One};


static uint32_t BlendGLtoBGFX(uint32_t input)
{
    using blend = BlendFunc::eBlend;
    switch (input)
    {
    case BlendFunc::One:
        return uint32_t(blend::One);
    case BlendFunc::Zero:
        return uint32_t(blend::Zero);
    case BlendFunc::SrcColor:
        return uint32_t(blend::SrcColor);
    case BlendFunc::SrcAlpha:
        return uint32_t(blend::SrcAlpha);
    case BlendFunc::DstColor:
        return uint32_t(blend::DstColor);
    case BlendFunc::DstAlpha:
        return uint32_t(blend::DstAlpha);
    case BlendFunc::InvSrcColor:
        return uint32_t(blend::InvSrcColor);
    case BlendFunc::InvSrcAlpha:
        return uint32_t(blend::InvSrcAlpha);
    case BlendFunc::InvDstColor:
        return uint32_t(blend::InvDstColor);
    case BlendFunc::InvDstAlpha:
        return uint32_t(blend::InvDstAlpha);
    default:
        return uint32_t(blend::One);
    }

}

uint64_t BlendFunc::toValue()
{
    return BGFX_STATE_BLEND_FUNC(BlendGLtoBGFX(src), BlendGLtoBGFX(dst));
}

Flag::Flag(uint32_t value)
	:_flags(value)
{

}

void Flag::setFlag(uint32_t type, bool value)
{
	if (value)
	{
		_flags |= type;
	}
	else
	{
		_flags &= ~type;
	}
}

bool Flag::getFlag(uint32_t type) const
{
    return _flags & type;
}

void Flag::toggle(uint32_t type)
{
	setFlag(type, !isOn(type));
}

bgfx::VertexDecl V3F_C4B_T2F::ms_decl;
V3F_C4B_T2F::Init V3F_C4B_T2F::init;

bgfx::VertexDecl DrawVertex::ms_decl;
DrawVertex::Init DrawVertex::init;

bgfx::VertexDecl VecVertex::ms_decl;
VecVertex::Init VecVertex::init;

NS_CC_END

