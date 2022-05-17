#pragma once

#include <iostream>
#include <Color.h>
#include <vector>

namespace Engine {

    class Aseprite {

    public:

        struct Image {
            int width;
            int height;
            Engine::Color* pixels;
        };

        struct Cel {
            int layer_index = 0;
            int linked_frame_index = -1;
            int x = 0;
            int y = 0;
            unsigned char alpha = 0;
            Image image;
        };

        struct Tag {
            enum class AnimDirection {
                Forward, Reverse, PingPong
            };

            uint16_t from;
            uint16_t to;
            AnimDirection animationDirection;
            std::string name;
        };

        struct Frame {
            int duration = 0;
            Image image;
            std::vector<Cel> cels{};
        };

        struct Layer {
            uint16_t flags;
            uint16_t type;
            std::string name;
            uint16_t child_level = 0;
            uint16_t blendmode = 0;
            uint8_t opacity = 0;
            bool visible = true;
        };

        struct Slice {
            int32_t pivotX;
            int32_t pivotY;
        };

        explicit Aseprite(const std::string& path);

        std::vector<Frame> frames{};
        std::vector<Engine::Color> palette;
        std::vector<Layer> layers;
        std::vector<Tag> tags;
        std::vector<Slice> slices;

    private:

        enum Modes {
            INDEXED = 1,
            GRAYSCALED = 2,
            RGBA = 4
        };

        enum class Chunks {
            OldPaletteA = 0x0004,
            OldPaletteB = 0x0011,
            Layer = 0x2004,
            Cel = 0x2005,
            CelExtra = 0x2006,
            Mask = 0x2016,
            Path = 0x2017,
            FrameTags = 0x2018,
            Palette = 0x2019,
            UserData = 0x2020,
            Slice = 0x2022,
            ColorProfile = 0x2007
        };

        typedef uint32_t DWORD;     // 4
        typedef uint16_t WORD;      // 2
        typedef int16_t SHORT;      // 2
        typedef uint8_t BYTE;       // 1
        typedef int32_t LONG;       // 4

        struct AsepriteFrameHeader {
            DWORD bytes;            // Bytes in this frame
            WORD magicNumber;       // Magic number (always 0xF1FA)
            WORD chunkCountOld;     // Old field which specifies the number of "chunks" in this frame. If this value is 0xFFFF, we might have more chunks to read in this frame (so we have to use the new field)
            WORD frameDuration;     // Frame duration (in milliseconds)
            BYTE unused[2];         // For future (set to zero)
            DWORD chunkCountNew;    // New field which specifies the number of "chunks" in this frame (if this is 0, use the old field)
        };

        struct AsepriteHeader {
            DWORD fileSize = 0;
            WORD magicNumber = 0;
            WORD frameCount = 0;
            WORD w, h = 0;
            WORD colorDepth = 0;
            DWORD flags;
            WORD speed;
            DWORD zeroA;
            DWORD zeroB;
            BYTE paletteEntry;
            BYTE ignored[3];
            WORD colorCount = 0;
            BYTE pixelWidth = 0;
            BYTE pixelHeight = 0;
            SHORT x, y = 0;
            WORD gridWidth;
            WORD gridHeight;
            BYTE unused[84];
        };

        struct AsepriteLayer {
            WORD flags;
            WORD type;
            WORD childLevel;
            WORD width;
            WORD height;
            WORD blendMode;
            BYTE opacity;
            BYTE future[3]; // reserved for future use
            WORD nameLength;
        };

        struct AsepritePaletteEntry {
            WORD flags{}; // 1 = has name
            Engine::Color color;
        };

        struct AsepritePalette {
            DWORD entryCount;
            DWORD firstColorIndex;
            DWORD lastColorIndex;
        };

        /// PIXEL: One pixel, depending on the image pixel format:
        //RGBA: BYTE[4], each pixel have 4 bytes in this order Red, Green, Blue, Alpha.
        //Grayscale: BYTE[2], each pixel have 2 bytes in the order Value, Alpha.
        //Indexed: BYTE, Each pixel uses 1 byte (the index).
        /// \param path

        AsepriteHeader info;

        void parsePalette(std::ifstream& reader);

        void parseCel(std::ifstream& reader, int chunkEnd, Frame& currentFrame);

        void parseLayer(std::ifstream& reader);

        void parseTag(std::ifstream& reader);

        void parseSlice(std::ifstream& ifstream);
    };

}