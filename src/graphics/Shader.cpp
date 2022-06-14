#include "Shader.h"
#include "Log.h"
#include <fstream>
#include "Application.h"
#include "Content.h"

using namespace Engine;

std::shared_ptr<Shader> Shader::create(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::string vertexSource;
    auto assets = Content::path().append(vertexPath);
    std::getline(std::ifstream(assets), vertexSource, '\0');

    std::string fragmentSource;
    assets = Content::path().append(fragmentPath);
    std::getline(std::ifstream(assets), fragmentSource, '\0');

    const Engine::ShaderData shader_data = {vertexSource, fragmentSource};
    return std::shared_ptr<Shader>(new Shader(shader_data));
}

Shader::Shader(const ShaderData &data)
{
    ENGINE_ASSERT(data.vertex.length() > 0, "Must provide a vertex shader");
    ENGINE_ASSERT(data.fragment.length() > 0, "Must provide a fragment shader");

    GLchar log[1024];
    GLsizei logLength = 0;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    {
        const GLchar *source = (const GLchar *)data.vertex.c_str();
        glShaderSource(vertexShader, 1, &source, nullptr);
        glCompileShader(vertexShader);
        glGetShaderInfoLog(vertexShader, 1024, &logLength, log);

        if (logLength > 0)
        {
            glDeleteShader(vertexShader);
            ENGINE_CORE_ERROR(log);
            return;
        }
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
        const GLchar *source = (const GLchar *)data.fragment.c_str();
        glShaderSource(fragmentShader, 1, &source, nullptr);
        glCompileShader(fragmentShader);
        glGetShaderInfoLog(fragmentShader, 1024, &logLength, log);

        if (logLength > 0)
        {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            ENGINE_CORE_ERROR(log);
            return;
        }
    }

    GLuint id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glGetProgramInfoLog(id, 1024, &logLength, log);
    glDetachShader(id, vertexShader);
    glDetachShader(id, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (logLength > 0)
    {
        ENGINE_CORE_ERROR(log);
        return;
    }

    // get uniforms
    bool validUniforms = true;
    {
        const int maxNameLength = 256;

        GLint activeUniforms = 0;
        glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &activeUniforms);

        for (int i = 0; i < activeUniforms; i++)
        {
            GLsizei length;
            GLsizei size;
            GLenum type;
            GLchar name[maxNameLength + 1];

            glGetActiveUniform(id, i, maxNameLength, &length, &size, &type, name);
            name[length] = '\0';

            // array names end with "[0]", and we don't want that
            for (int n = 0; n < maxNameLength; n++)
            {
                if (name[n] == '[')
                {
                    if (name[n + 1] == '0' && name[n + 2] == ']')
                    {
                        name[n] = '\0';
                        break;
                    }
                }
            }

            if (type == GL_SAMPLER_2D)
            {
                // GL_SAMPLER are special, so if it's a sampler push a texUniform AND a sampler uniform
                UniformInfo texUniform;
                texUniform.name = name;
                texUniform.bufferIndex = 0;
                texUniform.arrayLength = size;
                texUniform.type = UniformType::Texture2D;
                texUniform.shader = ShaderType::Fragment;
                uniformLocations.push_back(glGetUniformLocation(id, name));
                mUniforms.push_back(texUniform);

                UniformInfo samplerUniform;
                samplerUniform.name = std::string(name).append("_sampler");
                samplerUniform.bufferIndex = 0;
                samplerUniform.arrayLength = size;
                samplerUniform.type = UniformType::Sampler2D;
                samplerUniform.shader = ShaderType::Fragment;
                samplerUniform.location = glGetUniformLocation(id, name);
                mUniforms.push_back(samplerUniform);
            }
            else
            {
                // Otherwise just push the uniform
                UniformInfo uniform;
                uniform.name = name;
                uniform.type = UniformType::None;
                uniform.bufferIndex = 0;
                uniform.arrayLength = size;
                uniform.location = glGetUniformLocation(id, name);
                uniform.shader = (ShaderType)((int)ShaderType::Vertex | (int)ShaderType::Fragment);

                if (type == GL_FLOAT)
                    uniform.type = UniformType::Float;
                else if (type == GL_FLOAT_VEC2)
                    uniform.type = UniformType::Float2;
                else if (type == GL_FLOAT_VEC3)
                    uniform.type = UniformType::Float3;
                else if (type == GL_FLOAT_VEC4)
                    uniform.type = UniformType::Float4;
                else if (type == GL_FLOAT_MAT3x2)
                    uniform.type = UniformType::Mat3x2;
                else if (type == GL_FLOAT_MAT4)
                    uniform.type = UniformType::Mat4x4;
                else if (type == GL_INT)
                    uniform.type = UniformType::Int;
                else if (type == GL_INT_VEC2)
                    uniform.type = UniformType::Int2;
                else
                {
                    validUniforms = false;
                    ENGINE_CORE_ERROR("Unsupported uniform type");
                    break;
                }

                mUniforms.push_back(uniform);
            }
        }
    }

    // assign ID if the uniforms were valid
    if (!validUniforms)
        glDeleteProgram(id);
    else
        mId = id;
}

Shader::~Shader()
{
    if (mId > 0)
        glDeleteProgram(mId);
    mId = 0;
}

std::vector<UniformInfo> &Shader::uniforms()
{
    return mUniforms;
}

const std::vector<UniformInfo> &Shader::uniforms() const
{
    return mUniforms;
}

GLuint Shader::getId() const
{
    return mId;
}
