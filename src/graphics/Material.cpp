//
// Created by Fernando Raviola on 21/02/2021.
//
#include "Material.h"
#include "Log.h"
#include "Shader.h"

using namespace Engine;

int calc_uniform_size(const UniformInfo &uniform)
{
    int components = 0;

    switch (uniform.type)
    {
    case UniformType::Int:
        components = 1;
        break;
    case UniformType::Int2:
        components = 2;
        break;
    case UniformType::Float:
        components = 1;
        break;
    case UniformType::Float2:
        components = 2;
        break;
    case UniformType::Float3:
        components = 3;
        break;
    case UniformType::Float4:
        components = 4;
        break;
    case UniformType::Mat3x2:
        components = 6;
        break;
    case UniformType::Mat4x4:
        components = 16;
        break;
    default:
        ENGINE_CORE_ERROR("Unexpected Uniform Type");
        break;
    }

    return components * uniform.arrayLength;
}

Engine::Material::Material(const std::shared_ptr<Shader> &shader)
{
    ENGINE_ASSERT(shader, "Material is being created with an invalid shader");

    mShader = shader;

    auto &uniforms = shader->uniforms();
    int floatSize = 0;

    for (auto &uniform : uniforms)
    {

        if (uniform.type == UniformType::None)
            continue;
        if (uniform.type == UniformType::Texture2D)
        {
            for (int i = 0; i < uniform.arrayLength; i++)
            {
                mTextures.emplace_back();
            }
            continue;
        }
        if (uniform.type == UniformType::Sampler2D)
        {
            for (int i = 0; i < uniform.arrayLength; i++)
            {
                mSamplers.emplace_back();
            }
            continue;
        }

        floatSize += calc_uniform_size(uniform);
    }
    // uniform data reserves as much data as needed to fit all the uniforms in this shader
    // the order is important (renderpass uses this same order when uploading the data to the gpu)
    uniformData.reserve(floatSize);
}

std::shared_ptr<Shader> Engine::Material::shader() const
{
    return mShader;
}

void Engine::Material::setTexture(const char *name, const std::shared_ptr<Texture> &texture, int arrayIndex)
{
    ENGINE_ASSERT(mShader, "Material shader is invalid");

    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type != UniformType::Texture2D)
            continue;

        if (uniform.name == name)
        {
            // This class keeps all samplers and textures in two vectors<>
            // A Texture2D uniform can "use" multiple textures.
            // They will be stored continuously,
            // Offset tells us where each Texture2D[] starts. And array index will tell us the
            // precise texture position within that Texture2D[]
            // [ TEXA , TEXB, TEXC, TECD,]
            // If our shader looks like this
            // sampler texA  ------> array index 0 (this sampler is not an array), offset = 0
            // sampler texBC[2]  ----> arrayIndex 0 = B | arrayIndex 1 = C, offset = 1
            // sampler tex D; ---> arrayIndex 0, offset 3

            mTextures[offset + arrayIndex] = texture;
            return;
        }

        offset += uniform.arrayLength;
        if (offset + arrayIndex >= mTextures.size())
            break;
    }

    ENGINE_CORE_WARN("No texture unform {} at index {} exists", name, arrayIndex);
}

void Material::setTexture(int slot, const std::shared_ptr<Texture> &texture, int index)
{
    ENGINE_ASSERT(mShader, "Material shader is invalid");

    int s = 0;
    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type != UniformType::Texture2D)
            continue;

        if (s == slot)
        {
            if (index > uniform.arrayLength)
                break;

            mTextures[offset + index] = texture;
            break;
        }

        offset += uniform.arrayLength;
        s++;
    }
}

std::shared_ptr<Texture> Material::getTexture(const char *name, int index) const
{
    ENGINE_ASSERT(mShader, "Material shader is invalid");

    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type != UniformType::Texture2D)
            continue;

        if (uniform.name == name)
            return mTextures[offset + index];

        offset += uniform.arrayLength;
        if (offset + index >= mTextures.size())
            break;
    }

    ENGINE_CORE_ERROR("No Texture Uniform {} at index {} exists", name, index);
    return std::shared_ptr<Texture>();
}

const float *Material::data() const
{
    return uniformData.data();
}

void Material::setUniform(const char *name, const float *value, int64_t length)
{
    ENGINE_ASSERT(mShader, "Material shader is invalid");
    ENGINE_ASSERT(length >= 0, "Length must be >= 0");

    int index = 0;
    int offset = 0;
    // uniformData is a vector of floats that fits all the uniform data values in order.
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type == UniformType::Texture2D ||
            uniform.type == UniformType::Sampler2D ||
            uniform.type == UniformType::None)
            continue;

        if (uniform.name == name)
        {
            auto max = calc_uniform_size(uniform);
            if (length > max)
            {
                ENGINE_CORE_WARN("Exceeding length of Uniform '%s' (%i / %i)", name, length, max);
                length = max;
            }

            memcpy(uniformData.data() + offset, value, sizeof(float) * length);
            return;
        }

        offset += calc_uniform_size(uniform);
        index++;
    }

    ENGINE_CORE_WARN("No Uniform {} exists", name);
}

const float *Material::getValue(const char *name, int64_t *length) const
{
    ENGINE_ASSERT(mShader, "Material Shader is invalid");

    int index = 0;
    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type == UniformType::Texture2D ||
            uniform.type == UniformType::Sampler2D ||
            uniform.type == UniformType::None)
            continue;

        if (uniform.name == name)
        {
            if (length != nullptr)
                *length = calc_uniform_size(uniform);
            return uniformData.data() + offset;
        }

        index++;
        offset += calc_uniform_size(uniform);
    }

    *length = 0;
    ENGINE_CORE_WARN("Could not get Uniform, '%s' does not exists", name);
    return nullptr;
}

bool Material::hasValue(const char *name) const
{
    if (name != nullptr && name[0] != '\n')
    {
        for (auto &uniform : mShader->uniforms()) {
            if (strcmp(uniform.name.c_str(), name) == 0) {
                return true;
            }
        }
    }
    return false;
}

void Material::setSampler(const char *name, const TextureSampler &sampler, int index)
{
    ENGINE_ASSERT(mShader, "Material Shader is invalid");
    std::string samplerName{ name };
    samplerName.append("_sampler");

    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type != UniformType::Sampler2D)
            continue;

        if (uniform.name == samplerName)
        {
            mSamplers[offset + index] = sampler;
            return;
        }

        offset += uniform.arrayLength;
        if (offset + index >= mSamplers.size())
            break;
    }

    ENGINE_CORE_WARN("No Sampler Uniform '%s' at index [%i] exists", name, index);
}

void Material::setSampler(int slot, const TextureSampler &sampler, int index)
{
    ENGINE_ASSERT(mShader, "Material Shader is invalid");

    int s = 0;
    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type != UniformType::Sampler2D)
            continue;

        if (s == slot)
        {
            if (index > uniform.arrayLength)
                break;

            mSamplers[offset + index] = sampler;
            break;
        }

        offset += uniform.arrayLength;
        s++;
    }
}

TextureSampler Material::getSampler(const char *name, int index) const
{
    ENGINE_ASSERT(mShader, "Material Shader is invalid");

    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type != UniformType::Sampler2D)
            continue;

        if (uniform.name == name)
            return mSamplers[offset + index];

        offset += uniform.arrayLength;
        if (offset + index >= mSamplers.size())
            break;
    }

    ENGINE_CORE_WARN("No Sampler Uniform '%s' at index [%i] exists", name, index);
    return TextureSampler();
}

TextureSampler Material::getSampler(int slot, int index) const
{
    ENGINE_ASSERT(mShader, "Material Shader is invalid");

    int s = 0;
    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type != UniformType::Sampler2D)
            continue;

        if (s == slot)
        {
            if (index > uniform.arrayLength)
                break;

            return mSamplers[offset + index];
        }

        offset += uniform.arrayLength;
        if (offset + index >= mSamplers.size())
            break;

        s++;
    }

    ENGINE_CORE_WARN("No Sampler Uniform {} at index {} exists", slot, index);
    return TextureSampler();
}

const std::vector<std::shared_ptr<Texture>> &Material::textures() const
{
    return mTextures;
}

const std::vector<TextureSampler> &Material::samplers() const
{
    return mSamplers;
}

std::shared_ptr<Texture> Material::getTexture(int slot, int index) const
{
    ENGINE_ASSERT(mShader, "Material Shader is invalid");

    int s = 0;
    int offset = 0;
    for (auto &uniform : mShader->uniforms())
    {
        if (uniform.type != UniformType::Texture2D)
            continue;

        if (s == slot)
        {
            if (index > uniform.arrayLength)
                break;

            return mTextures[offset + index];
        }

        offset += uniform.arrayLength;
        if (offset + index >= mTextures.size())
            break;

        s++;
    }

    ENGINE_CORE_WARN("No Texture Uniform [{}] at index [{}] exists", slot, index);
    return std::shared_ptr<Texture>();
};