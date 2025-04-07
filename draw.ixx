module;

#include <d2d1.h>
#include <d3dx8.h>
#include <dwrite.h>

export module gm.draw;

import gm.core;
import gm.engine;

namespace gm::draw {

    class Draw {
        u8* _bitmap{};
        IDirect3DTexture8* _texture;
        ID3DXSprite* _sprite;

    public:
        Draw() noexcept = default;

        void init() noexcept {
            u32 width{ 256 }, height{ 256 };
            _bitmap = new u8[width * height * 4];

            IDirect3DDevice8* device{ gm::engine::direct3d.device() };
            D3DXCreateTexture(device, width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &_texture);
            D3DXCreateSprite(device, &_sprite);
        }

        ~Draw() noexcept {
            if (_bitmap != nullptr) {
                _sprite->Release();
                _texture->Release();
                delete[] _bitmap;
            }
        }

        bool text(f64 x, f64 y, std::string_view text) noexcept {
            if (_bitmap == nullptr) {
                return false;
            }

            u32 width{ 256 }, height{ 256 };
            std::mt19937 gen{ std::random_device{}() };
            std::uniform_int_distribution<u32> dist(0, 0xffffff);
            for (u32 i{}; i != width * height; ++i) {
                new (_bitmap + i * 4) u32{ dist(gen) | 0xff000000 };
            }

            D3DLOCKED_RECT locked;
            _texture->LockRect(0, &locked, nullptr, 0);
            auto data{ reinterpret_cast<u8*>(locked.pBits) };
            for (u32 y{}; y < height; ++y) {
                memcpy(data + y * locked.Pitch, _bitmap + y * width * 4, width * 4);
            }
            _texture->UnlockRect(0);

            D3DXVECTOR2 pos{ 100, 100 };
            _sprite->Draw(_texture, nullptr, nullptr, nullptr, 0, &pos, 0xffffffff);

            return true;
        }
    };

    export Draw draw;

}