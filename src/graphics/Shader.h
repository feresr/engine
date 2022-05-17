#pragma once

#include <string>
#include <glad/glad.h>
#include <vector>

namespace Engine {

    enum class UniformType {
        None,
        Int,
        Int2,
        Float,
        Float2,
        Float3,
        Float4,
        Mat3x2,
        Mat4x4,
        Texture2D,
        Sampler2D
    };

    enum class ShaderType {
        None = 0,          // 000
        Vertex = 1 << 0,     // 001
        Fragment = 1 << 1      // 010
    };

    struct UniformInfo {
        std::string name;

        UniformType type;

        ShaderType shader;

        int bufferIndex;

        int arrayLength;

        GLint location;
    };


    struct ShaderData {
        std::string vertex;
        std::string fragment;
    };

    class Shader {
    private:
        GLuint mId;
        std::vector<UniformInfo> mUniforms{};

    protected:
        Shader() = default;

    public:
        explicit Shader(const ShaderData& data);

        static std::shared_ptr<Shader> create(const std::string& vertexPath, const std::string& fragmentPath);

        std::vector<GLint> uniformLocations;

        Shader(const Shader&) = delete;

        Shader(Shader&&) = delete;

        Shader& operator=(const Shader&) = delete;

        Shader& operator=(Shader&&) = delete;

        ~Shader();

        virtual std::vector<UniformInfo>& uniforms();

        virtual const std::vector<UniformInfo>& uniforms() const;

        GLuint getId() const;

    };

}