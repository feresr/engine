#pragma once

#include <cinttypes>
#include <glad/glad.h>

namespace Engine {
    enum class BlendOp {
        Add = GL_FUNC_ADD,
        Subtract = GL_FUNC_SUBTRACT,
        ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
        Min = GL_MIN,
        Max = GL_MAX
    };

    enum class BlendFactor {
        Zero = GL_ZERO,
        One = GL_ONE,
        SrcColor = GL_SRC_COLOR,
        OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
        DstColor = GL_DST_COLOR,
        OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        DstAlpha = GL_DST_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        ConstantColor = GL_CONSTANT_COLOR,
        OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha = GL_CONSTANT_ALPHA,
        OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
        SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE,
        Src1Color = GL_SRC1_COLOR,
        OneMinusSrc1Color = GL_ONE_MINUS_SRC1_COLOR,
        Src1Alpha = GL_SRC1_ALPHA,
        OneMinusSrc1Alpha = GL_ONE_MINUS_SRC1_ALPHA
    };

    enum class BlendMask {
        None = 0,
        Red = 1,
        Green = 2,
        Blue = 4,
        Alpha = 8,
        RGB = Red | Green | Blue,
        RGBA = Red | Green | Blue | Alpha,
    };

    struct BlendMode {
        // Normal, pre-multiplied, Blend Mode
        static const BlendMode Normal;

        // Subtractive Blend Mode
        static const BlendMode Subtract;
        static const BlendMode Additive;

        BlendOp color_op;
        BlendFactor color_src;
        BlendFactor color_dst;
        BlendOp alpha_op;
        BlendFactor alpha_src;
        BlendFactor alpha_dst;
        BlendMask mask;
        uint32_t rgba;

        BlendMode() = default;

        BlendMode(BlendOp op, BlendFactor src, BlendFactor dst) :
                color_op(op),
                color_src(src),
                color_dst(dst),
                alpha_op(op),
                alpha_src(src),
                alpha_dst(dst),
                mask(BlendMask::RGBA),
                rgba(0xffffffff) {}

        BlendMode(
                BlendOp color_op, BlendFactor color_src, BlendFactor color_dst,
                BlendOp alpha_op, BlendFactor alpha_src, BlendFactor alpha_dst,
                BlendMask blend_mask, uint32_t blend_rgba) :
                color_op(color_op),
                color_src(color_src),
                color_dst(color_dst),
                alpha_op(alpha_op),
                alpha_src(alpha_src),
                alpha_dst(alpha_dst),
                mask(blend_mask),
                rgba(blend_rgba) {}

        bool operator==(const BlendMode& rhs) const {
            return
                    color_op == rhs.color_op && color_src == rhs.color_src && color_dst == rhs.color_dst &&
                    alpha_op == rhs.alpha_op && alpha_src == rhs.alpha_src && alpha_dst == rhs.alpha_dst &&
                    mask == rhs.mask && rgba == rhs.rgba;
        }

        bool operator!=(const BlendMode& rhs) const {
            return !(*this == rhs);
        }
    };
}