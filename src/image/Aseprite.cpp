#include <Log.h>
#include "Aseprite.h"
#include "Color.h"
#include "fstream"

#define STBI_NO_STDIO
#define STBI_ONLY_ZLIB

#include "stb/stb_image.h"

Engine::Aseprite::Aseprite(const std::string &path) {
    std::ifstream reader(path, std::ios::binary);
    if (!reader.is_open()) ENGINE_CORE_ERROR("Could not open Aseprite file {}", path);

    reader.read((char *) &info, 128);

    frames.resize(info.frameCount);

    for (int frameIndex = 0; frameIndex < info.frameCount; frameIndex++) {
        AsepriteFrameHeader frameHeader{};
        reader.read((char *) &frameHeader, 16);

        if (frameHeader.magicNumber != 0xF1FA) {
            ENGINE_CORE_ERROR("File is not a valid Aseprite file");
            return;
        }

        auto &frame = frames[frameIndex];
        frame.duration = frameHeader.frameDuration;
        frame.image.width = info.w;
        frame.image.height = info.h;
        frame.image.pixels = new Engine::Color[info.w * info.h];

        uint16_t chunkCount;
        if (frameHeader.chunkCountOld == 0xFFFF) {
            chunkCount = frameHeader.chunkCountOld;
        } else {
            chunkCount = frameHeader.chunkCountNew;
        }

        for (int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++) {
            auto chunkStart = reader.tellg();
            DWORD chunkSize;
            WORD chunkType;

            reader.read((char *) &chunkSize, sizeof(DWORD));
            reader.read((char *) &chunkType, sizeof(WORD));

            auto type = static_cast<Chunks>(chunkType);
            switch (type) {
                case Chunks::OldPaletteA :
                case Chunks::OldPaletteB :
                case Chunks::ColorProfile :
                case Chunks::CelExtra :
                case Chunks::Path :
                case Chunks::UserData :
                case Chunks::Mask : //(DEPRECATED)
                    break;
                case Chunks::Slice :
                    parseSlice(reader);
                    break;
                case Chunks::FrameTags :
                    parseTag(reader);
                    break;
                case Chunks::Layer :
                    parseLayer(reader);
                    break;
                case Chunks::Cel :
                    parseCel(reader, chunkSize + chunkStart, frame);
                    break;
                case Chunks::Palette :
                    parsePalette(reader);
                    break;
            }

            reader.seekg(chunkSize + chunkStart, std::ios_base::beg);
        }
    }
}

void Engine::Aseprite::parseLayer(std::ifstream &reader) {

    layers.emplace_back();
    auto &layer = layers.back();

    reader.read((char *) &layer.flags, sizeof(WORD));
    reader.read((char *) &layer.type, sizeof(WORD));
    reader.read((char *) &layer.child_level, sizeof(WORD));
    reader.seekg(sizeof(WORD) * 2, std::__1::ios_base::cur); // x,y ignored
    reader.read((char *) &layer.blendmode, sizeof(WORD));
    reader.read((char *) &layer.opacity, sizeof(BYTE));
    reader.seekg(sizeof(BYTE) * 3, std::__1::ios_base::cur); // reserved for future use

    WORD nameLength;
    reader.read((char *) &nameLength, sizeof(WORD));
    layer.name = std::string{};
    layer.name.resize(nameLength);
    reader.read(&layer.name[0], nameLength);
}

void Engine::Aseprite::parseCel(std::ifstream &reader, int chunkEnd, Frame &currentFrame) {
    currentFrame.cels.emplace_back();
    auto &cel = currentFrame.cels.back();
    reader.read((char *) &cel.layer_index, sizeof(WORD));
    reader.read((char *) &cel.x, sizeof(SHORT));
    reader.read((char *) &cel.y, sizeof(SHORT));
    reader.read((char *) &cel.alpha, sizeof(BYTE));
    WORD type;
    reader.read((char *) &type, sizeof(WORD));

    reader.seekg(7, std::__1::ios_base::cur); // ignored (for future use)

    switch (type) {
        case 0:      // Raw
        case 2: {   // ZLib
            WORD width;
            reader.read((char *) &width, sizeof(WORD));
            WORD height;
            reader.read((char *) &height, sizeof(WORD));
            cel.image.width = width;
            cel.image.height = height;
            cel.image.pixels = new Engine::Color[width * height];
            size_t count = width * height * sizeof(Color);

            if (type == 0) { // Raw
                reader.read((char *) cel.image.pixels, count);
            }
            if (type == 2) { // ZLib
                size_t isize = chunkEnd - reader.tellg();
                char *ibuffer = new char[isize];
                reader.read(ibuffer, isize);
                auto res = stbi_zlib_decode_buffer((char *) cel.image.pixels, count, ibuffer, isize);
                if (res < 0) ENGINE_CORE_ERROR("Could not read ase file data");
                delete[] ibuffer;
            }

            switch (info.colorDepth / 8) {
                case Modes::RGBA: {
                    // no op
                    break;
                }
                case Modes::GRAYSCALED: {
                    //Each pixel contains 2 bytes [value, alpha]
                    auto src = (unsigned char *) cel.image.pixels;
                    auto dst = cel.image.pixels;
                    for (int d = width * height - 1, s = (width * height - 1) * 2;
                         d >= 0; d--, s -= 2) {
                        dst[d] = Color(src[s], src[s], src[s], src[s + 1]);
                    }
                    cel.image.pixels = cel.image.pixels;
                    break;
                }
                case Modes::INDEXED: {
                    auto src = (unsigned char *) cel.image.pixels;
                    auto dst = cel.image.pixels;
                    // This copies in-place from the end to the beginning to avoid copying over data we still need to read.
                    // Explanation: we copy (8 bits at a time - by decrementing an unsigned char *)
                    // we get a 32 bits Color from thhe palette and we paste these 32 bits into the array.
                    // All of this happens in place. So if we pasted these 32 bits in theh beggining, we would be pasing it over
                    // the next 8 bits we were supposed to read.
                    for (int x = width * height - 1; x >= 0; x--) {
                        dst[x] = palette[src[x]];
                    }
                }
                    break;
            }
        }
            break;
        case 1:     // Linked cel (https://www.aseprite.org/docs/linked-cels/)
            reader.read((char *) &cel.linked_frame_index, sizeof(WORD));
            break;
        default:
            break;
    }


    // render cel to current frame
    // TODO: account for cel x and y (right now we assume the cel size and position equals that of the frame)
    {
        for (int j = 0; j < currentFrame.image.height; j++) {
            for (int i = 0; i < currentFrame.image.width; i++) {
                if (i >= cel.x && i < cel.x + cel.image.width &&
                    j >= cel.y && j < cel.y + cel.image.height) {
                    auto frameCoords = i + j * currentFrame.image.width;
                    auto celCoords = (i - cel.x) + (j - cel.y) * cel.image.width;
                    // todo: blending?
                    currentFrame.image.pixels[frameCoords] = cel.image.pixels[celCoords];
                }
            }
        }
    }

    //memcpy(currentFrame.image.pixels, cel.image.pixels, sizeof(Engine::Color) * cel.image.width * cel.image.height);

}

void Engine::Aseprite::parsePalette(std::ifstream &reader) {
    AsepritePalette asepritePalette;
    reader.read((char *) &asepritePalette.entryCount, sizeof(DWORD));
    reader.read((char *) &asepritePalette.firstColorIndex, sizeof(DWORD));
    reader.read((char *) &asepritePalette.lastColorIndex, sizeof(DWORD));
    reader.seekg(8, std::__1::ios_base::cur); // skip 8 bytes, future use

    palette.reserve(asepritePalette.entryCount);

    for (int p = 0; p < asepritePalette.entryCount; p++) {
        AsepritePaletteEntry entry;
        reader.read((char *) &entry.flags, sizeof(WORD));
        reader.read((char *) &entry.color, sizeof(Color));

        palette.push_back(entry.color);

        if (entry.flags == 1) {
            // has name
            WORD nameLength;
            reader.read((char *) &nameLength, sizeof(WORD));
            char *entryName = new char[nameLength + 1];
            entryName[nameLength] = '\0';
            reader.read(entryName, nameLength);
            std::cout << entryName << std::endl;
            delete[] entryName;
        }
    }
}

void Engine::Aseprite::parseTag(std::ifstream &reader) {
    WORD numberOfTags;
    reader.read((char *) &numberOfTags, sizeof(WORD));
    reader.seekg(8, std::ios_base::cur);
    for (int tagIndex = 0; tagIndex < numberOfTags; ++tagIndex) {
        auto &tag = tags.emplace_back();
        reader.read((char *) &tag.from, sizeof(WORD));
        reader.read((char *) &tag.to, sizeof(WORD));
        BYTE animDirection;
        reader.read((char *) &animDirection, sizeof(BYTE));
        tag.animationDirection = static_cast<Tag::AnimDirection>(animDirection);

        reader.seekg(8, std::ios_base::cur); // for future
        reader.seekg(3, std::ios_base::cur); // tag-color
        reader.seekg(1, std::ios_base::cur); // extra byte (0)

        WORD nameLength;
        reader.read((char *) &nameLength, sizeof(WORD));
        tag.name = std::string{};
        tag.name.resize(nameLength);
        reader.read(&tag.name[0], nameLength);
    }
}

void Engine::Aseprite::parseSlice(std::ifstream &reader) {
    DWORD sliceCount;
    reader.read((char *) &sliceCount, sizeof(DWORD));
    if (sliceCount <= 0) return;
    DWORD flags;
    reader.read((char *) &flags, sizeof(DWORD));
    if ((flags & 0b10) == 0) return; // has pivot information
    reader.seekg(sizeof(DWORD), std::ios_base::cur); //reserved

    WORD nameLenght;
    reader.read((char *) &nameLenght, sizeof(WORD));
    reader.seekg(nameLenght, std::ios_base::cur); //name

    for (int i = 0; i < sliceCount; i++) {
        DWORD frameNumber;
        reader.read((char *) &frameNumber, sizeof(DWORD));
        LONG sliceX;
        reader.read((char *) &sliceX, sizeof(LONG));
        LONG sliceY;
        reader.read((char *) &sliceY, sizeof(LONG));
        DWORD sliceW;
        reader.read((char *) &sliceW, sizeof(DWORD));
        DWORD sliceH;
        reader.read((char *) &sliceH, sizeof(DWORD));
        if ((flags & 0b1) > 0) {
            // 9-patch (unsopported)
            reader.seekg(sizeof(LONG) * 2, std::ios_base::cur);
            reader.seekg(sizeof(DWORD) * 2, std::ios_base::cur);
        }

        if ((flags & 0b10) > 0) {
            // pivot
            LONG pivotX;
            reader.read((char *) &pivotX, sizeof(LONG));
            LONG pivotY;
            reader.read((char *) &pivotY, sizeof(LONG));

            int32_t px = pivotX;
            int32_t py = pivotY;
            slices.push_back({px, py});
        }
    }


}

