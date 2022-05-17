#pragma once

#include <inttypes.h>
#include <memory>
#include <array>
#include <vector>
#include "glad/glad.h"

namespace Engine {
    enum class VertexType {
        None,
        Float,
        Float2,
        Float3,
        Float4,
        Byte4,
        UByte4,
        Short2,
        UShort2,
        Short4,
        UShort4
    };

    struct VertexAttribute {
        // Location / Attribute Index
        int index = 0;

        // Vertex Type
        VertexType type = VertexType::None;

        // Whether the Vertex should be normalized (doesn't apply to Floats)
        bool normalized = false;
    };

    struct VertexFormat {
        // List of Attributes
        std::vector<VertexAttribute> attributes;

        // Total size in bytes of each Vertex element
        int stride = 0;

        VertexFormat() = default;

        VertexFormat(std::initializer_list<VertexAttribute> attributes, int stride = 0);
    };

    enum class IndexFormat {
        UInt16,
        UInt32
    };

    class Mesh;

    typedef std::shared_ptr<Mesh> MeshRef;

    class Mesh {
    protected:

    public:
        Mesh();

        // Copy / Moves not allowed
        Mesh(const Mesh&) = delete;

        Mesh(Mesh&&) = delete;

        Mesh& operator=(const Mesh&) = delete;

        Mesh& operator=(Mesh&&) = delete;

        // Default Destructor
        ~Mesh();

        [[nodiscard]] GLuint getId() const;

        [[nodiscard]] GLenum indexFormat() const;

        [[nodiscard]] int indexSize() const;

        // Uploads the given index buffer to the Mesh
        void index_data(IndexFormat format, const void* indices, int64_t count);

        // Uploads the given vertex buffer to the Mesh
        void vertex_data(const VertexFormat& format, const void* vertices, int64_t count);

        // Gets the index count of the Mesh
        [[nodiscard]] int64_t index_count() const;

        // Gets the vertex count of the Mesh
        [[nodiscard]] int64_t vertex_count() const;

    private:
        GLuint mId;
        GLuint indexBuffer;
        GLuint vertexBuffer;
        GLuint instanceBuffer;
        int64_t indexCount;
        int64_t vertexCount;
        int64_t instanceCount;
        int64_t vertexSize;
        int64_t instanceSize;
        GLenum mIndexFormat;
        int mIndexSize;
        uint8_t vertexAttribsEnabled;
    };


}