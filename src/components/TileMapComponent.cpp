#include "glm/glm.hpp"
#include "TileMapComponent.h"
#include "Ecs.h"
#include "Batch.h"
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/Layer.hpp>
#include <Content.h>

TileMapComponent::TileMapComponent(const MapInfo* mapInfo) : mapInfo{mapInfo} {}

void TileMapComponent::setCell(unsigned int x, unsigned int y, const Engine::Subtexture &sprite) {
    grid[x + y * columns] = sprite;
}

void TileMapComponent::render(Engine::Batch &batch) {
    batch.pushMatrix(glm::mat3x2{1.0f, 0.0f, 0.0f, 1.0f, entity->position.x, entity->position.y});
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            batch.tex(
                    grid[i + j * columns],
                    glm::vec2{
                            (float) i * grid[i + j * columns].width(),
                            (float) j * grid[i + j * columns].height()
                    },
                    Engine::Color(0xFFFFFF)
            );
        }
    }
    batch.popMatrix();
}


bool TileMapComponent::awake() {
    if (!mapInfo) return false;
    tmx::Map map;
    map.load(mapInfo->fileName);

    entity->position.x = mapInfo->rect.left();
    entity->position.y = mapInfo->rect.top();

    columns = map.getTileCount().x;
    rows = map.getTileCount().y;
    grid.clear();
    grid.resize(columns * rows);

    auto *collider = get<Collider>();
    if (!collider) {
        collider = &entity->add<Collider>(columns, rows, 16);
        collider->mask = Collider::Mask::SOLID;
    } else {
        collider->clear();
    }

    auto &tileSet = *map.getTilesets().begin();
    auto &imagePath = tileSet.getImagePath();
    auto texture = Engine::Texture::create(imagePath.c_str());

    tmx::TileLayer *solidLayer = nullptr;
    tmx::TileLayer *backgroundLayer = nullptr;

    // Fetch layers
    for (auto &layer : map.getLayers()) {
        if (layer->getName() == "Solid") {
            solidLayer = &layer->getLayerAs<tmx::TileLayer>();
        }

        if (layer->getName() == "Background") {
            backgroundLayer = &layer->getLayerAs<tmx::TileLayer>();
        }

        if (layer->getType() == tmx::Layer::Type::Object) {
            tmx::ObjectGroup *objectGroup = nullptr;
            objectGroup = &layer->getLayerAs<tmx::ObjectGroup>();
            objects.reserve(objectGroup->getObjects().size());
            for (auto &object : objectGroup->getObjects()) {
                objects.push_back(
                        MapObject{
                                entity->position.x + (int) object.getPosition().x,
                                entity->position.y + (int) object.getPosition().y,
                                object.getType()
                        }
                );
            }
        }
    }

    for (int i = 0; i < map.getTileCount().x; i++) {
        for (int j = 0; j < map.getTileCount().y; j++) {
            if (solidLayer) {
                auto tileId = solidLayer->getTiles()[i + j * map.getTileCount().x].ID;
                auto tile = tileSet.getTile(tileId);
                if (tile) {
                    collider->setCell(i, j, true);
                    auto sprite = Engine::Subtexture(
                            texture,
                            Engine::Rect(
                                    {(float) tile->imagePosition.x, (float) tile->imagePosition.y},
                                    {tile->imageSize.x, tile->imageSize.y}
                            )
                    );

                    setCell(i, j, sprite);
                }
            }

            if (backgroundLayer) {
                auto backgroundTileId = backgroundLayer->getTiles()[i + j * map.getTileCount().x].ID;
                auto tile = tileSet.getTile(backgroundTileId);
                if (tile) {
                    auto sprite = Engine::Subtexture(
                            texture,
                            Engine::Rect(
                                    {(float) tile->imagePosition.x, (float) tile->imagePosition.y},
                                    {tile->imageSize.x, tile->imageSize.y}
                            )
                    );
                    setCell(i, j, sprite);
                }
            }
        }
    }
    return true;
}

Engine::RectI TileMapComponent::bounds() const {
    return Engine::RectI(
            entity->position, {columns * 16, rows * 16}
    );
}

