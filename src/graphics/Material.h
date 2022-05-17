#pragma once

#include <memory>
#include "Texture.h"
#include "Shader.h"

namespace Engine {

    class Material final {

    private:

    public:
        explicit Material(const std::shared_ptr<Shader>& shader);
        // Copy / Moves not allowed (expensive)
        Material(const Material&) = delete;
        Material(Material&&) = delete;
        Material& operator=(const Material&) = delete;
        Material& operator=(Material&&) = delete;

        // Default destructor
        ~Material() = default;

        [[nodiscard]] std::shared_ptr<Shader> shader() const;

        void setTexture(const char* name, const std::shared_ptr<Texture>& texture, int arrayIndex = 0);
        void setTexture(int slot, const std::shared_ptr<Texture>& texture, int arrayIndex = 0);

        [[nodiscard]] std::shared_ptr<Texture> getTexture(const char* name, int index) const;
        [[nodiscard]] std::shared_ptr<Texture> getTexture(int slot, int index) const;

        void setSampler(const char* name, const TextureSampler& sampler, int arrayIndex = 0);
        void setSampler(int slot, const TextureSampler& sampler, int arrayIndex = 0);

        TextureSampler getSampler(const char* name, int arrayIndex = 0) const;
        [[nodiscard]] TextureSampler getSampler(int slot, int arrayIndex = 0) const;

        // Sets the value in memory (uploading to GPU happens before rendering, see RenderPass#perform)
        // "length" is the total number of floats to set
        // Example: If uniform is a float2[4], a total of 8 float values can be set
        void setUniform(const char* name, const float* value, int64_t length);

        const float* getValue(const char* name, int64_t* length = nullptr) const;
        bool hasValue(const char* name) const;

        [[nodiscard]] const std::vector<std::shared_ptr<Texture>>& textures() const;

        [[nodiscard]] const std::vector<TextureSampler>& samplers() const;

        [[nodiscard]] const float* data() const;

    private:

        std::shared_ptr<Shader> mShader;
        std::vector<std::shared_ptr<Texture>> mTextures;
        std::vector<TextureSampler> mSamplers;
        // Used in the *RenderPass* class when uploading uniforms to OpenGL 
        std::vector<float> uniformData;
    };

}