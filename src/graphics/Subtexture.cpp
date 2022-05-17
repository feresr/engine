//
// Created by Fernando Raviola on 14/03/2021.
//

#include "Subtexture.h"

#include <utility>

Engine::Subtexture::Subtexture() = default;

Engine::Subtexture::Subtexture(const std::shared_ptr<Texture>& texture, Engine::Rect source) : texture{texture}, rect{source} {

}


