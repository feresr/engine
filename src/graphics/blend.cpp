#include "blend.h"

using namespace Engine;

const BlendMode BlendMode::Normal = BlendMode(
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::OneMinusSrcAlpha,
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::OneMinusSrcAlpha,
	BlendMask::RGBA,
	0xffffffff
);

const BlendMode BlendMode::Additive = BlendMode(
	BlendOp::Add,
	BlendFactor::SrcAlpha,
	BlendFactor::One,
	BlendOp::Add,
	BlendFactor::SrcAlpha,
	BlendFactor::One,
	BlendMask::RGBA,
	0xffffffff
);

const BlendMode BlendMode::Subtract = BlendMode(
	BlendOp::ReverseSubtract,
	BlendFactor::One, // src (what you are drawing)
	BlendFactor::One, // dst (what you already have)
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::One,
	BlendMask::RGBA,
	0xffffffff
);


