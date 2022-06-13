#pragma once

#include <memory>
#include "Sampler.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "Subtexture.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Material.h"
#include "mesh.h"
#include "renderpass.h"
#include "blend.h"
#include "Color.h"
#include "font.h"

namespace Engine
{
    enum class ColorMode
    {
        // Draws textures and shapes normally
        Normal,
        // Ignores the texture color but still uses transparency, essentially
        // drawing the "shape" of the texture as a solid color
        Wash
    };

    // A 2D sprite batcher.
    class Batch
    {

    public:
        // Name of the matrix uniform in the Shader
        const char *matrixUniform;
        const char *textureUniform;

        // Set on clear
        TextureSampler defaultSampler;

        Batch();

        Batch(const Batch &other) = delete;

        Batch &operator=(const Batch &other) = delete;

        ~Batch();

        // param absolute: the provided matrix will not be transformed by the current stack.
        void pushMatrix(const glm::mat3x2 &matrix, bool absolute = false);
        glm::mat3x2 popMatrix();
        glm::mat3x2 peekMatrix() const;

        void pushMaterial(std::shared_ptr<Engine::Material> material);
        void popMaterial();

        void pushBlend(const BlendMode &blend);
        void popBlend();

        void pushColorMode(const ColorMode colorMode);
        const ColorMode popColorMode();
        const ColorMode peekColorMode() const;

        // TODO: layers - (z order)
        // void pushLayer(int i)
        // void popLayer()
        // int peekLayer()

		// Sets the current texture used for drawing. Note that certain functions will override
		// this (ex the `str` and `tex` methods)
        void setTexture(const std::shared_ptr<Engine::Texture> &texture);

        void render(const std::shared_ptr<Engine::FrameBuffer> &target); // make target = App::backbuffer

        void render(const std::shared_ptr<Engine::FrameBuffer> &target, glm::mat4x4 matrix);

        void clear();

        void dispose();

        void quad(const glm::vec2 &pos0,
                  const glm::vec2 &pos1,
                  const glm::vec2 &pos2,
                  const glm::vec2 &pos3,
                  const Color &color);

        void quad(const glm::vec2 &pos,
                  const glm::vec2 &size,
                  Color color);

        void tex(const std::shared_ptr<Texture> texture, const glm::vec2 &position, const Color &color);

        void tex(const Subtexture &sprite, const glm::vec2 &position, const Color &color);

        void line(const glm::vec2& from, const glm::vec2& to, float t, Color color);

        void tri(glm::vec2 pos0, glm::vec2 pos1, glm::vec2 pos2, Color color);

        // void line(const glm::vec2& from, const glm::vec2& to, float t, Color fromColor, Color toColor);

        void circle(const glm::vec2& center, float radius, int steps, Color color);

        // void circle(const glm::vec2& center, float radius, int steps, Color center_color, Color outer_color);

        enum TextAlign { LEFT, CENTERED };
        void str(const Engine::Font &font, const std::string &text, const glm::vec2 &position, const Color &color);
        void str(const Engine::Font &font, const std::string &text, const glm::vec2 &position, const Color &color, float scale);
        void str(const Engine::Font &font, const std::string &text, const glm::vec2 &position, const Color &color, float scale, TextAlign align);


    private:
        struct Vertex
        {
            glm::vec2 position;
            glm::vec2 texture{0};
            Color color;

            // these unsigned int get interpeted as floats in the fragment shader
            // they are meant to be values between (0 - 255) with represents (0.0 to 1.0) in floats
            uint8_t mult = 255;
            uint8_t wash = 0;
            uint8_t fill = 0;
            uint8_t pad = 0;
        };

        struct DrawBatch
        {
            int layer;
            int offset;   // indices are stored in the parent `Batch` class, the offset represents where this `DrawBatch` indices start
            int elements; // # of triangles triangles
            std::shared_ptr<Material> material;
            BlendMode blend;
            std::shared_ptr<Texture> texture;
            TextureSampler sampler;
            bool flipVertically;

            DrawBatch() : layer(0),
                          offset(0),
                          elements(0),
                          flipVertically(false) {}
        };

        std::shared_ptr<Material> mDefaultMaterial; // used when the DrawBatch doesn't specify a material

        /**
         * The mesh never changes, we could initialise it this here even,
         * the only reason we don't is because we want to make sure the OpenGL context is created.
         */
        std::shared_ptr<Mesh> m_mesh;
        glm::mat3x2 m_matrix = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
        ColorMode m_color_mode;
        uint8_t m_tex_mult;
        uint8_t m_tex_wash;
        DrawBatch m_currentBatch;
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        std::vector<glm::mat3x2> m_matrix_stack;
        // A drawbatch specifies a 'material', a 'texture' and the # number of triangles(elements) to draw using those
        // The actual vertices and indices are stored in the parent Batch (this object)
        std::vector<DrawBatch> m_batches;

        void render_single_batch(RenderPass &pass, const DrawBatch &b, const glm::mat4x4 &matrix);

        std::vector<ColorMode> m_color_mode_stack;
        std::vector<BlendMode> m_blend_stack;
        std::vector<std::shared_ptr<Engine::Material>> m_material_stack;
        // not used yet.
        std::vector<int> m_layer_stack;
    };
}
