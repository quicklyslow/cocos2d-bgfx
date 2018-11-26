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

#ifndef __BASE_CCTYPES_H__
#define __BASE_CCTYPES_H__

#include "math/CCGeometry.h"
#include "platform/CCGL.h"
#include "bgfx/bgfx.h"


/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

#define IOS_MAX_TOUCHES_COUNT     10

struct Color4B;
struct Color4F;

/**
 * RGB color composed of bytes 3 bytes.
 * @since v3.0
 */
struct CC_DLL Color3B
{
    Color3B();
    Color3B(uint8_t _r, uint8_t _g, uint8_t _b);
    explicit Color3B(const Color4B& color);
    explicit Color3B(const Color4F& color);
	explicit Color3B(std::u16string hex_str);

    bool operator==(const Color3B& right) const;
    bool operator==(const Color4B& right) const;
    bool operator==(const Color4F& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4B& right) const;
    bool operator!=(const Color4F& right) const;

    bool equals(const Color3B& other) const
    {
        return (*this == other);
    }

    uint8_t r;
    uint8_t g;
    uint8_t b;

    static const Color3B WHITE;
    static const Color3B YELLOW;
    static const Color3B BLUE;
    static const Color3B GREEN;
    static const Color3B RED;
    static const Color3B MAGENTA;
    static const Color3B BLACK;
    static const Color3B ORANGE;
    static const Color3B GRAY;
};

/**
 * RGBA color composed of 4 bytes.
 * @since v3.0
 */
struct CC_DLL Color4B
{
    Color4B();
    Color4B(uint32_t argb);
    Color4B(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a);
    explicit Color4B(const Color3B& color, GLubyte _a = 255);
    explicit Color4B(const Color4F& color);
    uint32_t toABGR() const;
    uint32_t toRGBA() const;

    inline void set(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    bool operator==(const Color4B& right) const;
    bool operator==(const Color3B& right) const;
    bool operator==(const Color4F& right) const;
    bool operator!=(const Color4B& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4F& right) const;
    const Color4B& operator=(const Color4F& right);

    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;

    static const Color4B WHITE;
    static const Color4B YELLOW;
    static const Color4B BLUE;
    static const Color4B GREEN;
    static const Color4B RED;
    static const Color4B MAGENTA;
    static const Color4B BLACK;
    static const Color4B ORANGE;
    static const Color4B GRAY;
};


/**
 * RGBA color composed of 4 floats.
 * @since v3.0
 */
struct CC_DLL Color4F
{
    Color4F();
    Color4F(float _r, float _g, float _b, float _a);
    explicit Color4F(const Color3B& color, float _a = 1.0f);
    explicit Color4F(const Color4B& color);

    bool operator==(const Color4F& right) const;
    bool operator==(const Color3B& right) const;
    bool operator==(const Color4B& right) const;
    bool operator!=(const Color4F& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4B& right) const;

    bool equals(const Color4F &other) const
    {
        return (*this == other);
    }

    Vec4 toVec4() const;

    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;

    static const Color4F WHITE;
    static const Color4F YELLOW;
    static const Color4F BLUE;
    static const Color4F GREEN;
    static const Color4F RED;
    static const Color4F MAGENTA;
    static const Color4F BLACK;
    static const Color4F ORANGE;
    static const Color4F GRAY;
};

struct TouchEventBEC
{
    intptr_t ids[IOS_MAX_TOUCHES_COUNT];
    float xs[IOS_MAX_TOUCHES_COUNT];
    float ys[IOS_MAX_TOUCHES_COUNT];
};

struct TouchEventMove
{
    intptr_t ids[IOS_MAX_TOUCHES_COUNT];
    float xs[IOS_MAX_TOUCHES_COUNT];
    float ys[IOS_MAX_TOUCHES_COUNT];
    float fs[IOS_MAX_TOUCHES_COUNT];
    float ms[IOS_MAX_TOUCHES_COUNT];
};

/** A vertex composed of 2 floats: x, y
 @since v3.0
 */
// struct Vertex2F
// {
//     Vertex2F(float _x, float _y) :x(_x), y(_y) {}

//     Vertex2F(): x(0.f), y(0.f) {}

//     GLfloat x;
//     GLfloat y;
// };


/** A vertex composed of 2 floats: x, y
 @since v3.0
 */
// struct Vertex3F
// {
//     Vertex3F(float _x, float _y, float _z)
//         : x(_x)
//         , y(_y)
//         , z(_z)
//     {}

//     Vertex3F(): x(0.f), y(0.f), z(0.f) {}

//     GLfloat x;
//     GLfloat y;
//     GLfloat z;
// };

/** @struct Tex2F
 * A TEXCOORD composed of 2 floats: u, y
 * @since v3.0
 */
struct CC_DLL Tex2F {
    Tex2F(float _u, float _v): u(_u), v(_v) {}

    Tex2F(): u(0.f), v(0.f) {}

    GLfloat u;
    GLfloat v;
};

/** @struct PointSprite
 * Vec2 Sprite component.
 */
struct CC_DLL PointSprite
{
    Vec2   pos;        // 8 bytes
    Color4B    color;      // 4 bytes
    GLfloat    size;       // 4 bytes
};

/** @struct Quad2
 * A 2D Quad. 4 * 2 floats.
 */
struct CC_DLL Quad2
{
    Vec2        tl;
    Vec2        tr;
    Vec2        bl;
    Vec2        br;
};

/** @struct Quad3
 * A 3D Quad. 4 * 3 floats.
 */
struct CC_DLL Quad3 {
    Vec3        bl;
    Vec3        br;
    Vec3        tl;
    Vec3        tr;
};

/** @struct V2F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct V2F_C4B_T2F
{
    /// vertices (2F)
    Vec2       vertices;
    /// colors (4B)
    Color4B        colors;
    /// tex coords (2F)
    Tex2F          texCoords;
};

/** @struct V2F_C4B_PF
 *
 */
struct V2F_C4B_PF
{
    /// vertices (2F)
    Vec2       vertices;
    /// colors (4B)
    Color4B        colors;
    /// pointsize
    float      pointSize;
};

/** @struct V2F_C4F_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4F.
 */
struct CC_DLL V2F_C4F_T2F
{
    /// vertices (2F)
    Vec2       vertices;
    /// colors (4F)
    Color4F        colors;
    /// tex coords (2F)
    Tex2F          texCoords;
};

/** @struct V3F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct CC_DLL V3F_C4B_T2F
{
    /// vertices (3F)
    Vec3     vertices;            // 12 bytes
    // tex coords (2F)
    Tex2F        texCoords;           // 8 bytes
    /// colors (4B)
    Color4B      colors;              // 4 bytes

    struct Init
    {
        Init()
        {
            ms_decl.begin()
                    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                   .end();
        }
    };

    static bgfx::VertexDecl ms_decl;
    static Init init;
};

struct DrawVertex
{
    float x, y, z, w;
    uint32_t abgr;
    float u, v;
    struct Init
    {
        Init()
        {
            ms_decl.begin()
                .add(bgfx::Attrib::Position, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        }
    };
    static bgfx::VertexDecl ms_decl;
    static Init init;
};

struct VecVertex 
{
    float x, y, u, v;
    struct Init
    {
        Init()
        {
            ms_decl.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        }
    };
    static bgfx::VertexDecl ms_decl;
    static Init init;
};

/** @struct V3F_T2F
 * A Vec2 with a vertex point, a tex coord point.
 */
struct CC_DLL V3F_T2F
{
    /// vertices (2F)
    Vec3       vertices;
    /// tex coords (2F)
    Tex2F          texCoords;
};

/** @struct V2F_C4B_T2F_Triangle
 * A Triangle of V2F_C4B_T2F.
 */
struct CC_DLL V2F_C4B_T2F_Triangle
{
    V2F_C4B_T2F a;
    V2F_C4B_T2F b;
    V2F_C4B_T2F c;
};

/** @struct V2F_C4B_T2F_Quad
 * A Quad of V2F_C4B_T2F.
 */
struct CC_DLL V2F_C4B_T2F_Quad
{
    /// bottom left
    V2F_C4B_T2F    bl;
    /// bottom right
    V2F_C4B_T2F    br;
    /// top left
    V2F_C4B_T2F    tl;
    /// top right
    V2F_C4B_T2F    tr;

    inline operator V2F_C4B_T2F*()
    {
        return reinterpret_cast<V2F_C4B_T2F*>(this);
    }
    inline operator const V2F_C4B_T2F*() const
    {
        return reinterpret_cast<const V2F_C4B_T2F*>(this);
    }
};

/** @struct V3F_C4B_T2F_Quad
 * 4 Vertex3FTex2FColor4B.
 */
struct CC_DLL V3F_C4B_T2F_Quad
{
    /// top left
    V3F_C4B_T2F    tl;
    /// bottom left
    V3F_C4B_T2F    bl;
    /// top right
    V3F_C4B_T2F    tr;
    /// bottom right
    V3F_C4B_T2F    br;
};

/** @struct V2F_C4F_T2F_Quad
 * 4 Vertex2FTex2FColor4F Quad.
 */
struct CC_DLL V2F_C4F_T2F_Quad
{
    /// bottom left
    V2F_C4F_T2F    bl;
    /// bottom right
    V2F_C4F_T2F    br;
    /// top left
    V2F_C4F_T2F    tl;
    /// top right
    V2F_C4F_T2F    tr;
};

/** @struct V3F_T2F_Quad
 *
 */
struct CC_DLL V3F_T2F_Quad
{
    /// bottom left
    V3F_T2F    bl;
    /// bottom right
    V3F_T2F    br;
    /// top left
    V3F_T2F    tl;
    /// top right
    V3F_T2F    tr;
};

/** @struct BlendFunc
 * Blend Function used for textures.
 */
struct CC_DLL BlendFunc
{
    /** source blend function */
    uint32_t src;
    /** destination blend function */
    uint32_t dst;

    enum class eBlend
    {
        One = uint32_t(BGFX_STATE_BLEND_ONE),
        Zero = uint32_t(BGFX_STATE_BLEND_ZERO),
        SrcColor = uint32_t(BGFX_STATE_BLEND_SRC_COLOR),
        SrcAlpha = uint32_t(BGFX_STATE_BLEND_SRC_ALPHA),
        DstColor = uint32_t(BGFX_STATE_BLEND_DST_COLOR),
        DstAlpha = uint32_t(BGFX_STATE_BLEND_DST_ALPHA),
        InvSrcColor = uint32_t(BGFX_STATE_BLEND_INV_SRC_COLOR),
        InvSrcAlpha = uint32_t(BGFX_STATE_BLEND_INV_SRC_ALPHA),
        InvDstColor = uint32_t(BGFX_STATE_BLEND_INV_DST_COLOR),
        InvDstAlpha = uint32_t(BGFX_STATE_BLEND_INV_DST_ALPHA)
    };

    enum
    {
        One = GL_ONE,
        Zero = GL_ZERO,
        SrcColor = GL_SRC_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        DstColor = GL_DST_COLOR,
        DstAlpha = GL_DST_ALPHA,
        InvSrcColor = GL_ONE_MINUS_SRC_COLOR,
        InvSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        InvDstColor = GL_ONE_MINUS_DST_COLOR,
        InvDstAlpha = GL_ONE_MINUS_DST_ALPHA
    };

    uint64_t toValue();
    /** Blending disabled. Uses {GL_ONE, GL_ZERO} */
    static const BlendFunc DISABLE;
    /** Blending enabled for textures with Alpha premultiplied. Uses {GL_ONE, GL_ONE_MINUS_SRC_ALPHA} */
    static const BlendFunc ALPHA_PREMULTIPLIED;
    /** Blending enabled for textures with Alpha NON premultiplied. Uses {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA} */
    static const BlendFunc ALPHA_NON_PREMULTIPLIED;
    /** Enables Additive blending. Uses {GL_SRC_ALPHA, GL_ONE} */
    static const BlendFunc ADDITIVE;

    bool operator==(const BlendFunc &a) const
    {
        return src == a.src && dst == a.dst;
    }

    bool operator!=(const BlendFunc &a) const
    {
        return src != a.src || dst != a.dst;
    }

    bool operator<(const BlendFunc &a) const
    {
        return src < a.src || (src == a.src && dst < a.dst);
    }
};

/** @struct TextVAlignment
 * Vertical text alignment type.
 *
 * @note If any of these enums are edited and/or reordered, update Texture2D.m.
 */
enum class CC_DLL TextVAlignment : char
{
    TOP,
    CENTER,
    BOTTOM
};

/** @struct TextHAlignment
 * Horizontal text alignment type.
 *
 * @note If any of these enums are edited and/or reordered, update Texture2D.m.
 */
enum class CC_DLL TextHAlignment : char
{
    LEFT,
    CENTER,
    RIGHT
};

/**
* @brief Possible GlyphCollection used by Label.
*
* Specify a collections of characters to be load when Label created.
* Consider using DYNAMIC.
*/
enum class GlyphCollection : char {
    DYNAMIC,
    NEHE,
    ASCII,
    CUSTOM
};

// Types for animation in particle systems

/** @struct T2F_Quad
 * Texture coordinates for a quad.
 */
struct CC_DLL T2F_Quad
{
    /// bottom left
    Tex2F    bl;
    /// bottom right
    Tex2F    br;
    /// top left
    Tex2F    tl;
    /// top right
    Tex2F    tr;
};

/** @struct AnimationFrameData
 * Struct that holds the size in pixels, texture coordinates and delays for animated ParticleSystemQuad.
 */
struct CC_DLL AnimationFrameData
{
    T2F_Quad texCoords;
    float delay;
    Size size;
};

/**
 types used for defining fonts properties (i.e. font name, size, stroke or shadow)
 */

/** @struct FontShadow
 * Shadow attributes.
 */
struct CC_DLL FontShadow
{
public:

    // shadow is not enabled by default
    FontShadow()
        : _shadowEnabled(false)
        , _shadowBlur(0)
        , _shadowOpacity(0)
    {}

    /// true if shadow enabled
    bool   _shadowEnabled;
    /// shadow x and y offset
    Size   _shadowOffset;
    /// shadow blurrines
    float  _shadowBlur;
    /// shadow opacity
    float  _shadowOpacity;
};

/** @struct FontStroke
 * Stroke attributes.
 */
struct CC_DLL FontStroke
{
public:

    // stroke is disabled by default
    FontStroke()
        : _strokeSize(0.0f)
        , _strokeColor(Color3B::BLACK)
        , _strokeAlpha(255)
        , _strokeEnabled(false)
    {}

    /// stroke size
    float     _strokeSize;
    /// stroke color
    Color3B   _strokeColor;
    /// stroke alpha
    GLubyte   _strokeAlpha;
    /// true if stroke enabled
    bool      _strokeEnabled;
};

/** @struct FontDefinition
 * Font attributes.
 */
struct CC_DLL FontDefinition
{
public:
    /**
     * @js NA
     * @lua NA
     */
    FontDefinition()
        : _fontSize(0)
        , _alignment(TextHAlignment::CENTER)
        , _vertAlignment(TextVAlignment::TOP)
        , _dimensions(Size::ZERO)
        , _fontFillColor(Color3B::WHITE)
        , _fontAlpha(255)
        , _enableWrap(true)
        , _overflow(0)
    {}

    /// font name
    std::string           _fontName;
    /// font size
    int                   _fontSize;
    /// enable shrink font size
    int                  _overflow;
    /// rendering box
    Size                  _dimensions;
    /// font color
    Color3B               _fontFillColor;
    /// font alpha
    GLubyte               _fontAlpha;
    /// font shadow
    FontShadow            _shadow;
    /// font stroke
    FontStroke            _stroke;
    /// horizontal alignment
    TextHAlignment        _alignment;
    /// vertical alignment
    TextVAlignment _vertAlignment;
    /// enable text wrap
    bool                  _enableWrap;
    bool                 _enableBold;
};

struct Triangles
{
    /**Vertex data pointer.*/
    V3F_C4B_T2F* verts;
    /**Index data pointer.*/
    unsigned short* indices;
    /**The number of vertices.*/
    int vertCount;
    /**The number of indices.*/
    int indexCount;
};

/**Uniform is a structure to encapsulate data got from glGetActiveUniform and glGetUniformLocation.*/
struct Uniform
{
    /**The place where the uniform placed, starts from 0.*/
    GLint location;
    /**Number of data type in attribute.*/
    GLint size;
    /**Data type of the attribute.*/
    GLenum type;
    /**String of the uniform name.*/
    std::string name;
};

/**
 * @brief Effects used by `Label`
 *
 */
enum LabelEffect {
    // FIXME: Covert them to bitwise. More than one effect should be supported
    NORMAL = 1 << 0,
    OUTLINE = 1 << 1,
    SHADOW = 1 << 2,
    GLOW = 1 << 3,
    ITALICS = 1 << 4,
    BOLD = 1 << 5,
    UNDERLINE = 1 << 6,
    STRIKETHROUGH = 1 << 7,
	GRADIENT = 1 << 8,
    ALL = (1 << 9) - 1
};

extern int cocosType;

template<typename T>
int CocosType()
{
    static int type = cocosType++;
    return type;
}

#define COCOS_TYPE(type) \
public: int getCocosType() const \
{ \
    return cocos2d::CocosType<type>(); \
}

#define COCOS_TYPE_BASE(type) \
public: virtual int getCocosType() const \
{ \
    return cocos2d::CocosType<type>(); \
}

#define COCOS_TYPE_OVERRIDE(type) \
public: virtual int getCocosType() const override \
{ \
    return cocos2d::CocosType<type>(); \
}

template<typename OutT, typename InT>
OutT* CocosCast(InT* obj)
{
    return (obj && obj->getCocosType() == CocosType<OutT>()) ? static_cast<OutT*>(obj) : nullptr;
}

namespace Switch
{
    inline constexpr std::size_t hash(char const* input)
    {
        return *input ? *input + 33ull * hash(input + 1) : 5381;
    }
    inline std::size_t hash(const char* input, int size, int index)
    {
        return index < size ? input[index] + 33ull * hash(input, size, index + 1) : 5381;
    }
    inline std::size_t hash(String str)
    {
        return hash(str.rawData(), static_cast<int>(str.size()), 0);
    }
    namespace Literals
    {
        inline std::size_t constexpr operator"" _hash(const char* s, size_t)
        {
            return hash(s);
        }
    }
}

class CC_DLL Flag
{
public:
	Flag(uint32_t value);
	void setFlag(uint32_t type, bool value);
    bool getFlag(uint32_t type) const;
	void toggle(uint32_t type);
	uint32_t value() const { return _flags; }
	inline void setValue(uint32_t v) { _flags = v; }
	inline void setOn(uint32_t type) { _flags |= type; }
	inline void setOff(uint32_t type) { _flags &= ~type; }
	inline bool isOn(uint32_t type) const { return (_flags & type) != 0; }
	inline bool isOff(uint32_t type) const { return (_flags & type) == 0; }
private:
	uint32_t _flags;
};

/** @struct Acceleration
 * The device accelerometer reports values for each axis in units of g-force.
 */
class CC_DLL Acceleration
    /*: public Ref*/
{
public:
    double x;
    double y;
    double z;

    double timestamp;

    Acceleration(): x(0), y(0), z(0), timestamp(0) {}
};

extern const std::string CC_DLL STD_STRING_EMPTY;
extern const ssize_t CC_DLL CC_INVALID_INDEX;

NS_CC_END
// end group
/// @}
#endif //__BASE_CCTYPES_H__

