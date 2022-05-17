#pragma once

#include <cinttypes>
#include "Material.h"
#include "FrameBuffer.h"
#include "mesh.h"
#include "blend.h"
#include "rect.h"

namespace Engine {
    enum class Compare {
        None,
        Always,
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreatorOrEqual
    };

    enum class Cull {
        None = 0,
        Front = 1,
        Back = 2,
    };

    struct RenderPass {
        // FrameBuffer to draw to
        std::shared_ptr<FrameBuffer> target;

        // Mesh to draw with
        std::shared_ptr<Mesh> mesh;

        // Material to draw with
        std::shared_ptr<Material> material;

        // Whether the RenderPass should use a specific viewport
        bool has_viewport;

        // Whether the RenderPass should use a scissor rectangle
        bool has_scissor;

        // The viewport (only used if hasViewport is true)
        Rect viewport;

        // The scissor rectangle (only used if hasScissor is true)
        Rect scissor;

        // First index in the Mesh to draw from
        int64_t index_start;

        // Total amount of indices to draw from the Mesh
        int64_t index_count;

        // Total amount of instances to draw from the Mesh
        int64_t instance_count;

        // Depth Compare Function
        Compare depth;

        // Cull Mode
        Cull cull;

        // Blend Mode
        BlendMode blend;

        // Initializes a default RenderPass
        RenderPass();

        // Performs the render
        void perform();
    };
}