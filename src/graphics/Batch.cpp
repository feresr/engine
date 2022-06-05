#include <Log.h>
#include "renderpass.h"
#include "Batch.h"
#include "imgui.h"
#include "iostream"
#include "DefaultShader.h"

namespace Engine
{
    const VertexFormat format = VertexFormat(
        {
            {0, VertexType::Float2, false}, // position
            {1, VertexType::Float2, false}, // uv
            {2, VertexType::UByte4, true},  // color
            {3, VertexType::UByte4, true},  // type (mult, wash, fill)
        });

    Batch::Batch()
    {
        matrixUniform = "u_matrix";
        textureUniform = "u_texture";
        clear();
    }

    Batch::~Batch()
    {
        dispose();
    }

    void Batch::pushMatrix(const glm::mat3x2 &matrix, bool absolute)
    {
        m_matrix_stack.push_back(m_matrix);
        if (absolute)
        {
            m_matrix = matrix;
        }
        else
        {
            m_matrix = m_matrix * glm::mat3x3(matrix);
        }
    }

    void Batch::pushMaterial(std::shared_ptr<Engine::Material> material)
    {
        // if the current batch has elements (who need the previous material)
        // then create a new batch
        if (m_currentBatch.elements > 0)
        {
            m_material_stack.push_back(m_currentBatch.material);
            m_batches.push_back(m_currentBatch);
            m_currentBatch.offset += m_currentBatch.elements;
        }
        m_currentBatch.material = material;
        m_currentBatch.elements = 0;
    }

    void Batch::popMaterial()
    {
        auto was = m_currentBatch.material;

        // If the current batch has elements (who will need the previous material)
        // then create a new batch
        if (m_currentBatch.elements > 0)
        {
            m_batches.push_back(m_currentBatch);
            m_currentBatch.offset += m_currentBatch.elements;
            m_currentBatch.elements = 0;
        }
        if (m_material_stack.size() > 0)
        {
            // get the previous material from the stack,
            auto material = m_material_stack.back();
            m_material_stack.pop_back();
            m_currentBatch.material = material;
        }
        else
        {
            // or the default one if empty
            m_currentBatch.material = nullptr;
        }
    }

    void Batch::pushBlend(const BlendMode &blend)
    {
        if (m_currentBatch.elements > 0)
        {
            m_blend_stack.push_back(m_currentBatch.blend);
            m_batches.push_back(m_currentBatch);
            m_currentBatch.offset += m_currentBatch.elements;
        }
        m_currentBatch.blend = blend;
        m_currentBatch.elements = 0;
    }

    void Batch::popBlend()
    {
        if (m_blend_stack.size() > 0)
        {
            m_batches.push_back(m_currentBatch);
            BlendMode blend = m_blend_stack.back();
            m_currentBatch.blend = blend;
            m_blend_stack.pop_back();
            m_currentBatch.offset += m_currentBatch.elements;
            m_currentBatch.elements = 0;
        }
    }

    glm::mat3x2 Batch::popMatrix()
    {
        auto was = m_matrix;
        m_matrix = m_matrix_stack.back();
        m_matrix_stack.pop_back();
        return was;
    }

    void Batch::setTexture(const std::shared_ptr<Engine::Texture> &texture)
    {
        // if the current batch has elements but are not using this texture
        // (meaning those elements should be render with a different texture),
        // Then we'll need to "store" the current batch in the stack and start a new one with the new texture
        // Quesiton: do batches only support one texture each? Whats the point of materials with multiple textures then
        // Answer: looks like it, DrawBatch only support one texture (this gets set in the material in slot 0, see Batch::render()
        // Answer2 (months later): Well no, the above is not true, nothing stops you from using other texture slots. But this is how it currently works
        if (m_currentBatch.elements > 0 && texture != m_currentBatch.texture && m_currentBatch.texture)
        {
            // copy current batch into baches
            m_batches.push_back(m_currentBatch);

            // re-use (reset) current bach
            // as the indices are stored in the `Batch` class and not each individual `DrawBatch`,
            // we keep a reference to the start (offset)
            m_currentBatch.offset += m_currentBatch.elements;
            m_currentBatch.elements = 0;
        }

        // This checks seems useless but actually covers the case where we're using an empty
        // batch (nothing to draw, then the previous if-check would have failed) with a different texture
        // In that case the batch is empty, we can safely swap the textures.
        if (m_currentBatch.texture != texture)
        {
            m_currentBatch.texture = texture;
            m_currentBatch.flipVertically = texture->isFramebuffer();
        }
    }

    void Batch::pushColorMode(const ColorMode colorMode)
    {
        m_color_mode_stack.push_back(m_color_mode);
        m_color_mode = colorMode;
    }

    const ColorMode Batch::popColorMode()
    {
        auto was = m_color_mode;
        if (!m_color_mode_stack.empty())
        {
            m_color_mode = m_color_mode_stack.back();
            m_color_mode_stack.pop_back();
        }
        return was;
    }

    const ColorMode Batch::peekColorMode() const
    {
        return m_color_mode;
    }

    void Batch::render(const std::shared_ptr<Engine::FrameBuffer> &target)
    {
        auto ortho = glm::ortho(0.0f, (float)target->width(), (float)target->height(), 0.0f);
        render(target, ortho);
    }

    void Batch::render(const std::shared_ptr<Engine::FrameBuffer> &target, const glm::mat4x4 projection)
    {
        if ((m_batches.empty() && m_currentBatch.elements <= 0) || m_indices.empty())
            return;

        // define defaults
        {
            if (!m_mesh)
                m_mesh = std::shared_ptr<Mesh>{new Mesh()};
            if (!mDefaultMaterial)
            {
                auto mDefaultShader = std::shared_ptr<Shader>(new Shader(shader_data));
                mDefaultMaterial = std::shared_ptr<Material>(new Material(mDefaultShader));
            }
        }

        // Why do we keep state as (mesh, and also m_indices and m_vertices) and
        // pack everything up here and send it to the RenderPass. Wouldn't it make more sense to
        // keep a mesh reference here and modify indices/vertex directly?
        //    For example, when calling Batch::Quad() instead of doing what we do currently m_indices.push_back(m_vertices.size() + 0);
        //    We could just touch the mesh directly? no?
        // Answer: NO, this would be wasteful. Methods in Mesh uploads the data to OpenGL, you want to do it here, right before rendering,
        // and not every time you add a quad
        m_mesh->vertex_data(format, m_vertices.data(), m_vertices.size());
        m_mesh->index_data(IndexFormat::UInt32, m_indices.data(), m_indices.size());

        RenderPass pass;
        pass.target = target; // where to render
        pass.mesh = m_mesh;   // what to render (vertex + indices)
        pass.has_viewport = false;
        pass.viewport = Rect();
        pass.instance_count = 0;
        pass.depth = Compare::None;
        pass.cull = Cull::None;

        // By this point all the mesh data is set up (all the vertex and indices are uploaded to the GPU)

        int drawCallCount = 0;
        for (auto &b : m_batches)
        { // render stacked batches (batches get stacked when we swap textures for example)
            drawCallCount++;
            render_single_batch(pass, b, projection);
        }
        if (m_currentBatch.elements > 0)
        { // render current batch
            drawCallCount++;
            render_single_batch(pass, m_currentBatch, projection);
        }

// DEBUG
#ifndef NDEBUG
        static std::string s{"Rendered in: "};
        std::string renderedIn = s + std::to_string(drawCallCount).c_str();
        // +1 for current batch
        ImGui::Text(renderedIn.c_str());
#endif
    }

    void Batch::render_single_batch(RenderPass &pass, const Batch::DrawBatch &b, const glm::mat4x4 &matrix)
    {
        pass.material = b.material;
        if (!pass.material)
            pass.material = mDefaultMaterial;

        // upload the texture in the batch when using the default material
        // (or a custom material with a shader containing a "u_texture" uniform)
        if (pass.material->hasValue(Batch::textureUniform))
        {
            // if u_texture exists, set the batch texture there
            pass.material->setTexture(Batch::textureUniform, b.texture);
            pass.material->setSampler(Batch::textureUniform, b.sampler);
        }
        else
        {
            // otherwise put it in the first slot 0
            // (if the user used the batch::pushTexture api, we assume they want the texture available somewhere in the sahder)
            pass.material->setTexture(0, b.texture);
            pass.material->setSampler(0, b.sampler);
        }
        if (pass.material->hasValue(Batch::matrixUniform))
        {
            pass.material->setUniform(Batch::matrixUniform, glm::value_ptr(matrix), 16);
        }

        pass.blend = b.blend;
        // pass.has_scissor = b.scissor.w >= 0 && b.scissor.h >= 0;
        // pass.scissor = b.scissor;

        pass.index_start = (int64_t)b.offset * 3; // Triangles have 3 sides D:
        pass.index_count = (int64_t)b.elements * 3;

        pass.perform();
    }

    glm::mat3x2 Batch::peekMatrix() const
    {
        return m_matrix;
    }

    void Batch::clear()
    {
        m_matrix = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
        m_color_mode = ColorMode::Normal;

        m_vertices.clear();
        m_indices.clear();

        m_currentBatch.layer = 0;
        m_currentBatch.elements = 0;
        m_currentBatch.offset = 0;
        m_currentBatch.blend = BlendMode::Normal;
        m_currentBatch.material.reset();
        m_currentBatch.texture.reset();
        m_currentBatch.sampler = defaultSampler;

        m_matrix_stack.clear();
        m_blend_stack.clear();
        m_material_stack.clear();
        m_color_mode_stack.clear();
        m_layer_stack.clear();
        m_batches.clear();
    }

    void Batch::dispose()
    {
        clear();
        m_vertices.clear();
        m_indices.clear();
        m_matrix_stack.clear();
        m_blend_stack.clear();
        m_material_stack.clear();
        m_color_mode_stack.clear();
        m_layer_stack.clear();
        m_batches.clear();

        mDefaultMaterial.reset();
        m_mesh.reset();
    }

    void Batch::quad(const glm::vec2 &pos0,
                     const glm::vec2 &pos1,
                     const glm::vec2 &pos2,
                     const glm::vec2 &pos3,
                     const Color &color)
    {

        // Two triangles
        m_currentBatch.elements += 2;

        // Add 6 indices to m_indices
        m_indices.reserve(m_indices.size() + 6);

        m_indices.push_back(m_vertices.size() + 0);
        m_indices.push_back(m_vertices.size() + 1);
        m_indices.push_back(m_vertices.size() + 2);
        m_indices.push_back(m_vertices.size() + 0);
        m_indices.push_back(m_vertices.size() + 2);
        m_indices.push_back(m_vertices.size() + 3);

        // Add 4 vertices (make sure to use the matrix)
        // Resize m_vertices to have 4 additial spaces [..., _, _, _, _]
        m_vertices.resize(m_vertices.size() + 4);

        // [..., _, _, _, _]
        //                ^ m_vertices.back()
        //   ^ m_vertices.back() - 4
        auto *p = &m_vertices.back() - 4;
        const glm::vec2 *positions[4] = {&pos0, &pos1, &pos2, &pos3};
        glm::vec2 uvs[4]{
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
        };
        uint8_t uvi = 0;
        for (auto &position : positions)
        {
            ++p;
            p->position = m_matrix * glm::vec3(*position, 1.0);
            p->color = color;
            p->texture = uvs[uvi];
            uvi++;
            p->wash = 255;
            p->fill = 255;
            p->mult = 0;
        }
    }

    void Batch::quad(const glm::vec2 &pos, const glm::vec2 &size, Color color)
    {
        quad(glm::vec2(pos.x, pos.y),
             glm::vec2(pos.x + size.x, pos.y),
             glm::vec2(pos.x + size.x, pos.y + size.y),
             glm::vec2(pos.x, pos.y + size.y),
             color);
    }

    void Batch::tex(const std::shared_ptr<Texture> texture, const glm::vec2 &position, const Color &color)
    {

        setTexture(texture);

        m_currentBatch.elements += 2; // Two triangles

        // Add 6 indices to m_indices
        m_indices.reserve(m_indices.size() + 6);

        m_indices.push_back(m_vertices.size() + 0);
        m_indices.push_back(m_vertices.size() + 1);
        m_indices.push_back(m_vertices.size() + 2);
        m_indices.push_back(m_vertices.size() + 0);
        m_indices.push_back(m_vertices.size() + 2);
        m_indices.push_back(m_vertices.size() + 3);

        // Add 4 vertices (make sure to use the matrix)

        // yeah, I don't think you can do this (reserve).
        // It breaks the std::vector API. How will it know that size changed?
        // m_vertices.reserve(m_vertices.size() + 4);

        m_vertices.resize(m_vertices.size() + 4);
        auto *p = &m_vertices.back() - 4;

        glm::vec2 positions[4]{
            glm::vec2{0, 0},
            glm::vec2(texture->getWidth(), 0.0f),
            glm::vec2{texture->getWidth(), texture->getHeight()},
            glm::vec2(0.0f, texture->getHeight())};

        glm::vec2 uvs[4]{
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
        };

        if (m_currentBatch.flipVertically)
        {
            for (int i = 0; i < 4; i++)
            {
                uvs[i].y = 1.0 - uvs[i].y;
            }
        }

        auto wash = m_color_mode == ColorMode::Wash ? 255 : 0;
        auto mult = m_color_mode == ColorMode::Normal ? 255 : 0;
        for (int i = 0; i < 4; i++)
        {
            ++p;
            p->position = m_matrix * glm::vec3(position + positions[i], 1.0f);
            p->color = color;
            p->texture = uvs[i];
            p->wash = wash;
            p->fill = 0;
            p->mult = mult;
        }
    }

    void Batch::tex(const Subtexture &sprite, const glm::vec2 &position, const Color &color)
    {
        if (!sprite.texture)
            return;
        setTexture(sprite.texture);

        m_currentBatch.elements += 2; // Two triangles

        // Add 6 indices to m_indices
        m_indices.reserve(m_indices.size() + 6);

        m_indices.push_back(m_vertices.size() + 0);
        m_indices.push_back(m_vertices.size() + 1);
        m_indices.push_back(m_vertices.size() + 2);
        m_indices.push_back(m_vertices.size() + 0);
        m_indices.push_back(m_vertices.size() + 2);
        m_indices.push_back(m_vertices.size() + 3);

        // Add 4 vertices (make sure to use the matrix)
        m_vertices.resize(m_vertices.size() + 4);
        auto *p = &m_vertices.back() - 4;

        // Todo: perhaps calculate the texture UVs once, instead of on every frame?
        // put the UVs in the sprite.

        auto textureSize = glm::vec2{
            (float)sprite.texture->getWidth(),
            (float)sprite.texture->getHeight(),
        };

        glm::vec2 positions[4]{
            glm::vec2{0, 0},
            glm::vec2(sprite.width(), 0.0f),
            glm::vec2{sprite.width(), sprite.height()},
            glm::vec2(0.0f, sprite.height())};
        glm::vec2 uvs[4]{
            sprite.rect.top_left(),
            sprite.rect.top_right(),
            sprite.rect.bottom_right(),
            sprite.rect.bottom_left(),
        };
        if (m_currentBatch.flipVertically)
        {
            for (int i = 0; i < 4; i++)
            {
                uvs[i].y = 1.0 - uvs[i].y;
            }
        }

        auto wash = m_color_mode == ColorMode::Wash ? 255 : 0;
        auto mult = m_color_mode == ColorMode::Normal ? 255 : 0;
        for (int i = 0; i < 4; i++)
        {
            ++p;
            p->position = m_matrix * glm::vec3(position + positions[i], 1.0f);
            p->color = color;
            p->texture = uvs[i] / textureSize;
            p->mult = mult;
            p->wash = wash;
            p->fill = 0;
        }
    }

    void Batch::circle(const glm::vec2 &center, float radius, int steps, Color color)
    {
        ENGINE_ASSERT(steps >= 3, "Circle must have at least 3 steps");
        const float advance = glm::two_pi<float>() / steps;

        glm::vec2 last = {center.x + radius * cos(0), center.y + radius * sin(0)};
        for (int i = 1; i <= steps; i++)
        {
            glm::vec2 next = {center.x + radius * cos(advance * i), center.y + radius * sin(advance * i)};
            tri(center, last, next, color);
            last = next;
        }
    }

    void Batch::tri(glm::vec2 pos0, glm::vec2 pos1, glm::vec2 pos2, Color color)
    {
        // one triangle
        m_currentBatch.elements += 1;

        // Add 6 indices to m_indices
        m_indices.reserve(m_indices.size() + 3);

        m_indices.push_back(m_vertices.size() + 0);
        m_indices.push_back(m_vertices.size() + 1);
        m_indices.push_back(m_vertices.size() + 2);

        // Add 4 vertices (make sure to use the matrix)
        // Resize m_vertices to have 4 additial spaces [..., _, _, _, _]
        m_vertices.resize(m_vertices.size() + 3);

        // [..., _, _, _, _]
        //                ^ m_vertices.back()
        //   ^ m_vertices.back() - 3
        auto *p = &m_vertices.back() - 3;
        const glm::vec2 *positions[3] = {&pos0, &pos1, &pos2};
        for (auto &position : positions)
        {
            ++p;
            p->position = m_matrix * glm::vec3(*position, 1.0);
            p->color = color;
            p->texture = glm::vec2(0.0f, 0.0f);
            p->wash = 255;
            p->fill = 255;
            p->mult = 0;
        }
    }

    void Batch::line(const glm::vec2 &from, const glm::vec2 &to, float t, Color color)
    {
        glm::vec2 tangent = glm::normalize(to - from);
        glm::vec2 normal = glm::vec2(tangent.y, -tangent.x);

        const glm::vec2 pos1 = glm::vec2{from.x + (normal * t).x * 0.5, from.y + (normal * t).y * 0.5};
        const glm::vec2 pos2 = glm::vec2{from.x - (normal * t).x * 0.5, from.y - (normal * t).y * 0.5};
        const glm::vec2 pos3 = glm::vec2{to.x - (normal * t).x * 0.5, to.y - (normal * t).y * 0.5};
        const glm::vec2 pos4 = glm::vec2{to.x + (normal * t).x * 0.5, to.y + (normal * t).y * 0.5};

        quad(pos1, pos2, pos3, pos4, color);
    }

    void Batch::str(const Engine::Font &font, const std::string &text, const glm::vec2 &position, const Color &color, float scale)
    {
        auto txt = text.c_str();
        float x = 0;
        float y = 0;
        pushMatrix(glm::mat3x2{scale, 0.0, 0.0, scale, position.x, position.y + font.ascent + font.descent});

        while (*txt)
        {
            auto character = font.getCharacter(txt);
            x += character.offset_x; // this character left padding
            y = character.offset_y;
            tex(character.texture, glm::vec2{x, y}, color);
            x += font.getAdvance(*txt, *(txt + 1)); // next character space
            ++txt;
        }
        popMatrix();
    }
    void Batch::str(const Engine::Font &font, const std::string &text, const glm::vec2 &position, const Color &color)
    {
        str(font, text, position, color, 1.0f);
    }
}