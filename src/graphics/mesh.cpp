#include "mesh.h"
#include "glad/glad.h"

using namespace Engine;

Mesh::Mesh() {
    mId = 0;
    indexBuffer = 0;
    vertexBuffer = 0;
    instanceBuffer = 0;
    indexCount = 0;
    vertexCount = 0;
    vertexSize = 0;
    vertexAttribsEnabled = 0;

    glGenVertexArrays(1, &mId);
}

Mesh::~Mesh() {
    if (vertexBuffer != 0) glDeleteBuffers(1, &vertexBuffer);
    if (indexBuffer != 0) glDeleteBuffers(1, &indexBuffer);
    if (mId != 0) glDeleteVertexArrays(1, &mId);
    mId = 0;
}

GLuint Mesh::getId() const {
    return mId;
}

GLenum Mesh::indexFormat() const {
    return mIndexFormat;
}

int Mesh::indexSize() const {
    return mIndexSize;
}

void Mesh::index_data(IndexFormat format, const void *indices, int64_t count) {
    indexCount = count;
    glBindVertexArray(mId);
    {
        if (indexBuffer == 0) glGenBuffers(1, &indexBuffer);
        switch (format) {
            case IndexFormat::UInt16:
                mIndexFormat = GL_UNSIGNED_SHORT;
                mIndexSize = 2;
                break;
            case IndexFormat::UInt32:
                mIndexFormat = GL_UNSIGNED_INT;
                mIndexSize = 4;
                break;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexSize * count, indices, GL_DYNAMIC_DRAW);
    }
    glBindVertexArray(0);
}

void Mesh::vertex_data(const VertexFormat &format, const void *vertices, int64_t count) {
    vertexCount = count;
    glBindVertexArray(mId);
    {
        if (vertexBuffer == 0) glGenBuffers(1, &vertexBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        {
            size_t ptr = 0;
            for (int n = 0; n < format.attributes.size(); n++) {
                auto &attribute = format.attributes[n];
                GLenum type = GL_UNSIGNED_BYTE;
                size_t componentsSize = 0;
                int components = 1;

                switch (attribute.type) {
                    case VertexType::Float:
                        type = GL_FLOAT;
                        componentsSize = 4;
                        components = 1;
                        break;
                    case VertexType::Float2:
                        type = GL_FLOAT;
                        componentsSize = 4;
                        components = 2;
                        break;
                    case VertexType::Float3:
                        type = GL_FLOAT;
                        componentsSize = 4;
                        components = 3;
                        break;
                    case VertexType::Float4:
                        type = GL_FLOAT;
                        componentsSize = 4;
                        components = 4;
                        break;
                    case VertexType::Byte4:
                        type = GL_BYTE;
                        componentsSize = 1;
                        components = 4;
                        break;
                    case VertexType::UByte4:
                        type = GL_UNSIGNED_BYTE;
                        componentsSize = 1;
                        components = 4;
                        break;
                    case VertexType::Short2:
                        type = GL_SHORT;
                        componentsSize = 2;
                        components = 2;
                        break;
                    case VertexType::UShort2:
                        type = GL_UNSIGNED_SHORT;
                        componentsSize = 2;
                        components = 2;
                        break;
                    case VertexType::Short4:
                        type = GL_SHORT;
                        componentsSize = 2;
                        components = 4;
                        break;
                    case VertexType::UShort4:
                        type = GL_UNSIGNED_SHORT;
                        componentsSize = 2;
                        components = 4;
                        break;
                    default:
                        break;
                }

                auto location = (uint32_t) attribute.index;
                glEnableVertexAttribArray(location);
                glVertexAttribPointer(location, components, type, attribute.normalized, format.stride, (void *) ptr);
                // todo: get and set divisor if you want to support instanced rendering: glVertexAttribDivisor(location, 0);
                ptr += components * componentsSize;
            }
        }

        vertexSize = format.stride;
        glBufferData(GL_ARRAY_BUFFER, vertexSize * count, vertices, GL_DYNAMIC_DRAW);
    }
    glBindVertexArray(0);
}

int64_t Mesh::index_count() const {
    return indexCount;
}

int64_t Mesh::vertex_count() const {
    return vertexCount;
}

VertexFormat::VertexFormat(std::initializer_list<VertexAttribute> attributes, int stride) {
    for (auto &it : attributes)
        this->attributes.push_back(it);

    if (stride <= 0) {
        stride = 0;
    }

    for (auto &it : attributes) {
        switch (it.type) {
            case VertexType::Float:
                stride += 4;
                break;
            case VertexType::Float2:
                stride += 8;
                break;
            case VertexType::Float3:
                stride += 12;
                break;
            case VertexType::Float4:
                stride += 16;
                break;
            case VertexType::Byte4:
            case VertexType::UByte4:
            case VertexType::Short2:
            case VertexType::UShort2:
                stride += 4;
                break;
            case VertexType::Short4:
            case VertexType::UShort4:
                stride += 8;
                break;
            case VertexType::None:
                break;
        }
    }

    this->stride = stride;
}