module;

#include <d2d1.h>
#include <d3dx8.h>
#include <dwrite.h>
#include <wrl/client.h>

export module gm:draw;

import std;
import :core;
import :engine;

using Microsoft::WRL::ComPtr;

namespace gm {

    export class Draw {
        std::unique_ptr<u8> _bitmap;
        ComPtr<IDirect3DTexture8> _texture;
        ComPtr<ID3DXSprite> _sprite;

    public:
        bool init() noexcept {
            u32 width{ 256 }, height{ 256 };
            _bitmap.reset(new u8[width * height * 4]);

            IDirect3DDevice8* device{ IDirect3DResource::device() };
            if (FAILED(D3DXCreateTexture(device, width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, _texture.GetAddressOf()))
                || FAILED(D3DXCreateSprite(device, _sprite.GetAddressOf()))) {
                return false;
            }

            return true;
        }

        bool text(f64 x, f64 y, std::string_view text) const noexcept {
            u32 width{ 256 }, height{ 256 };
            std::mt19937 gen{ std::random_device{}() };
            std::uniform_int_distribution<u32> dist(0, 0xffffff);
            for (u32 i{}; i != width * height; ++i) {
                new(_bitmap.get() + i * 4) u32{ dist(gen) | 0xff000000 };
            }

            D3DLOCKED_RECT locked;
            if (FAILED(_texture->LockRect(0, &locked, nullptr, 0))) {
                return false;
            }
            auto data{ static_cast<u8*>(locked.pBits) };
            for (u32 y{}; y < height; ++y) {
                memcpy(data + y * locked.Pitch, _bitmap.get() + y * width * 4, width * 4);
            }
            if (FAILED(_texture->UnlockRect(0))) {
                return false;
            }

            D3DXVECTOR2 pos{ 100, 100 };
            if (FAILED(_sprite->Draw(_texture.Get(), nullptr, nullptr, nullptr, 0, &pos, 0xffffffff))) {
                return false;
            }

            return true;
        }
    };

}
