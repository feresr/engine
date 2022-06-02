#include "renderpass.h"
#include "Log.h"
#include "glad/glad.h"

using namespace Engine;

// convert blend op enum
GLenum gl_get_blend_func(BlendOp operation)
{
    switch (operation)
    {
    case BlendOp::Add:
        return GL_FUNC_ADD;
    case BlendOp::Subtract:
        return GL_FUNC_SUBTRACT;
    case BlendOp::ReverseSubtract:
        return GL_FUNC_REVERSE_SUBTRACT;
    case BlendOp::Min:
        return GL_MIN;
    case BlendOp::Max:
        return GL_MAX;
    };
    return GL_FUNC_ADD;
}

GLenum gl_get_blend_factor(BlendFactor factor)
{
    switch (factor)
    {
    case BlendFactor::Zero:
        return GL_ZERO;
    case BlendFactor::One:
        return GL_ONE;
    case BlendFactor::SrcColor:
        return GL_SRC_COLOR;
    case BlendFactor::OneMinusSrcColor:
        return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DstColor:
        return GL_DST_COLOR;
    case BlendFactor::OneMinusDstColor:
        return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::SrcAlpha:
        return GL_SRC_ALPHA;
    case BlendFactor::OneMinusSrcAlpha:
        return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DstAlpha:
        return GL_DST_ALPHA;
    case BlendFactor::OneMinusDstAlpha:
        return GL_ONE_MINUS_DST_ALPHA;
    case BlendFactor::ConstantColor:
        return GL_CONSTANT_COLOR;
    case BlendFactor::OneMinusConstantColor:
        return GL_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::ConstantAlpha:
        return GL_CONSTANT_ALPHA;
    case BlendFactor::OneMinusConstantAlpha:
        return GL_ONE_MINUS_CONSTANT_ALPHA;
    case BlendFactor::SrcAlphaSaturate:
        return GL_SRC_ALPHA_SATURATE;
    case BlendFactor::Src1Color:
        return GL_SRC1_COLOR;
    case BlendFactor::OneMinusSrc1Color:
        return GL_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::Src1Alpha:
        return GL_SRC1_ALPHA;
    case BlendFactor::OneMinusSrc1Alpha:
        return GL_ONE_MINUS_SRC1_ALPHA;
    };

    return GL_ZERO;
}

RenderPass::RenderPass()
{
    blend = BlendMode::Normal;
    target = FrameBuffer::BackBuffer();
    mesh = std::shared_ptr<Mesh>();
    material = std::shared_ptr<Material>();
    has_viewport = false;
    has_scissor = false;
    viewport = Rect();
    scissor = Rect();
    index_start = 0;
    index_count = 0;
    instance_count = 0;
    depth = Compare::None;
    cull = Cull::None;
}

void RenderPass::perform()
{
    ENGINE_ASSERT(material, "Trying to draw with an invalid material");
    ENGINE_ASSERT(material->shader(), "Trying to draw with an invalid Shader");
    ENGINE_ASSERT(mesh, "Trying to draw with an invalid Mesh");

    // Validate BackBuffer
    if (!target)
    {
        target = FrameBuffer::BackBuffer();
        ENGINE_CORE_WARN("Trying to draw with an invalid Target; falling back to Back Buffer");
    }

    // Validate Index Count
    int64_t meshIndexCount = mesh->index_count();
    if (index_start + index_count > meshIndexCount)
    {
        ENGINE_CORE_WARN(
            "Trying to draw more indices than exist in the index buffer {}-{} / {}); trimming extra indices",
            index_start,
            index_start + index_count,
            meshIndexCount);

        if (index_start > index_count)
            return;

        index_count = index_count - index_start;
    }

    // get the total drawable size
    glm::vec2 targetDimensions = glm::vec2(target->width(), target->height());

    // Validate Viewport
    if (!has_viewport)
    {
        viewport.x = 0;
        viewport.y = 0;
        viewport.w = targetDimensions.x;
        viewport.h = targetDimensions.y;
    }
    else
    {
        viewport = viewport.overlap_rect(Rect(0, 0, targetDimensions.x, targetDimensions.y));
    }

    // Validate Scissor
    if (this->has_scissor)
        scissor = scissor.overlap_rect(Rect(0, 0, targetDimensions.x, targetDimensions.y));

    // -- Perform render --
    // bind FrameBuffer
    target->bind();

    {
        glUseProgram(material->shader()->getId());

        int texture_slot = 0;
        int gl_texture_slot = 0;
        GLint texture_slots[64];
        auto &uniforms = material->shader()->uniforms();
        auto data = this->material->data(); // uniform values

        // Uplaod uniforms
        for (int i = 0; i < uniforms.size(); i++)
        {
            auto &uniform = uniforms[i];
            auto location = uniform.location;

            //  TODO: research why does UniformType::Sampler2D and UniformType::Texture2D are two needed, we ignore UniformType::Sampler2D
            // in OpenGL shaders samplers are just a uniform pointing to a glTexture, which also defines the sampling mechanism.
            // in other words, a pointer to both texture and sampler method.
            if (uniform.type == UniformType::Sampler2D)
                continue;

            if (uniform.type == UniformType::Texture2D)
            {
                // This is in case you have an array of samplers[] (or sampler2DArray?)
                for (int n = 0; n < uniform.arrayLength; n++)
                {
                    auto tex = this->material->getTexture(texture_slot, n);
                    auto sampler = this->material->getSampler(texture_slot, n);

                    // We start consuming available texture slots as we need them here
                    // Start with the very first texture slot. And increase it (gl_texture_slot
                    // does not get reset after the for loop)
                    glActiveTexture(GL_TEXTURE0 + gl_texture_slot);
                    if (!tex)
                    {
                        glBindTexture(GL_TEXTURE_2D, 0);
                    }
                    else
                    {
                        auto glTex = tex.get();
                        glTex->updateSampler(sampler); // config the texture.
                        // Put the texture into whatever slot is 'active'
                        glBindTexture(GL_TEXTURE_2D, glTex->getId());
                    }

                    // We just put the texture into the slot, now we need to remember this slot to assign it
                    // to one of our sampler later on.
                    // texture_slots gets "overwritten" every time we get into the loop
                    texture_slots[n] = gl_texture_slot;
                    gl_texture_slot++;
                }

                // here's where the magic happens! we tell opengl OK this sampler in {location} should sample from these
                // [texture_slots]
                glUniform1iv(location, uniform.arrayLength, &texture_slots[0]);

                // I understand that texture_slots map 1:1 to samples, counting samplers[n] as 1 sampler.
                // And gl_texture_slot map 1:1 to samplers, counting samplers[n] as n samplers.
                texture_slot++;
                continue;
            }

            // Float
            if (uniform.type == UniformType::Float)
            {
                glUniform1fv(location, (GLint)uniform.arrayLength, data);
                data += uniform.arrayLength;
            }
            // Float2
            else if (uniform.type == UniformType::Float2)
            {
                glUniform2fv(location, (GLint)uniform.arrayLength, data);
                data += 2 * uniform.arrayLength;
            }
            // Float3
            else if (uniform.type == UniformType::Float3)
            {
                glUniform3fv(location, (GLint)uniform.arrayLength, data);
                data += 3 * uniform.arrayLength;
            }
            // Float4
            else if (uniform.type == UniformType::Float4)
            {
                glUniform4fv(location, (GLint)uniform.arrayLength, data);
                data += 4 * uniform.arrayLength;
            }
            // Matrix3x2
            else if (uniform.type == UniformType::Mat3x2)
            {
                glUniformMatrix3x2fv(location, (GLint)uniform.arrayLength, 0, data);
                data += 6 * uniform.arrayLength;
            }
            // Matrix4x4
            else if (uniform.type == UniformType::Mat4x4)
            {
                glUniformMatrix4fv(location, (GLint)uniform.arrayLength, 0, data);
                data += 16 * uniform.arrayLength;
            }
            else if (uniform.type == UniformType::Int)
            {
                // todo: this cast might be just working because sizeof(float) == sizeof(int)
                // but maybe we should no rely on this
                glUniform1iv(location, (GLint)uniform.arrayLength, (const GLint *)data);
                data += uniform.arrayLength;
            }
            else if (uniform.type == UniformType::Int2)
            {
                glUniform2iv(location, (GLint)uniform.arrayLength, (const GLint *)data);
                data += uniform.arrayLength * 2;
            }
        }
    }

    // BLEND MODE
    {
        glEnable(GL_BLEND);
        glBlendEquationSeparate(gl_get_blend_func(blend.color_op), gl_get_blend_func(blend.alpha_op));
        glBlendFuncSeparate(gl_get_blend_factor(blend.color_src), gl_get_blend_factor(blend.color_dst), gl_get_blend_factor(blend.alpha_src), gl_get_blend_factor(blend.alpha_dst));
    }

    // DEPTH FUNCTION
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
    }

    // CULL MODE
    {
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
        // glFrontFace(GL_CW);
    }

    // Viewport
    {
        glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
    }

    // DRAW THE MESH
    {
        glBindVertexArray(mesh->getId());

        int indexSize = mesh->indexSize();

        if (this->instance_count > 0)
        {
            // not suported yet
            glDrawElementsInstanced(
                GL_TRIANGLES,
                (GLint)(this->index_count),
                mesh->indexFormat(),
                (void *)(indexSize * this->index_start),
                (GLint)this->instance_count);
        }
        else
        {
            glDrawElements(
                GL_TRIANGLES,
                (GLint)(this->index_count),
                mesh->indexFormat(),
                (void *)(indexSize * this->index_start));
        }

        glBindVertexArray(0);
    }
}