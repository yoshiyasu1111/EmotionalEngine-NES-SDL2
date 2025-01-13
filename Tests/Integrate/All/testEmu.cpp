#include <SDL.h>
#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>
#include "Nes.h"
#include "testUtil.h"

namespace {
    const int NesGraphicWidth = 256;
    const int NesGraphicHeight = 240;

    const int SamplingFreq = 44100;  // サンプリング周波数 44.1kHz
    const int MaxSampleCount = SamplingFreq / 30;  // 30fpsのためのサンプル数（1フレーム分）

    // 音のバッファ
    std::vector<int16_t> audioBuffer;
    int audioBufferPos = 0;

    // HELLO WORLD する ROM 読み込む
    void ReadRom(std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize)
    {
        auto rootPath = test::GetRepositoryRootPath();
        assert(rootPath);

        auto nesFile = rootPath.value();
        // nesFile += "/Tests/TestBinaries/helloworld/sample1/sample1.nes";
        nesFile += "/Tests/TestBinaries/nestest/nestest.nes";

        test::ReadFile(nesFile, pOutBuf, pOutSize);
    }

    // 音をバッファに追加
    void AddWaveSample(int sample)
    {
        // 音のサンプルをaudioBufferに追加（10倍にして出力音量を増加）
        sample *= 10;

        // バッファが満杯であれば、古いサンプルを消去
        if (audioBuffer.size() < MaxSampleCount) {
            audioBuffer.push_back(sample);
        } else {
            audioBuffer[audioBufferPos] = sample;
        }

        audioBufferPos = (audioBufferPos + 1) % MaxSampleCount;  // サンプルポインタの循環
    }

    // サウンドのコールバック関数
    void AudioCallback(void* userdata, Uint8* stream, int len)
    {
        int16_t* buffer = reinterpret_cast<int16_t*>(stream);
        int numSamples = len / sizeof(int16_t);  // サンプルの数

        // // バッファにある音データをstreamにコピー
        // for (int i = 0; i < numSamples; ++i) {
        //     if (audioBuffer.size() > 0) {
        //         buffer[i] = audioBuffer[audioBufferPos];
        //         audioBufferPos = (audioBufferPos + 1) % MaxSampleCount;
        //     } else {
        //         buffer[i] = 0;  // 音がない場合は0（無音）
        //     }
        // }
        // // バッファにデータがない場合は無音にする
        // if (audioBuffer.size() == 0) {
        //     memset(buffer, 0, len);  // バッファが空なら無音にする
        // } else {
        //     // バッファにある音データをストリームにコピー
        //     for (int i = 0; i < numSamples; ++i) {
        //         buffer[i] = audioBuffer[audioBufferPos];
        //         audioBufferPos = (audioBufferPos + 1) % MaxSampleCount;
        //     }
        // }
    // バッファにサンプルがある場合のみ処理
    if (audioBuffer.size() > 0) {
        for (int i = 0; i < numSamples; ++i) {
            // audioBufferPosが範囲外でないかを確認
            if (audioBufferPos < audioBuffer.size()) {
                buffer[i] = audioBuffer[audioBufferPos];
            } else {
                buffer[i] = 0;  // 範囲外の場合は無音
            }
            audioBufferPos = (audioBufferPos + 1) % MaxSampleCount;
        }
    } else {
        // バッファが空の場合は無音
        memset(buffer, 0, len);
    }
    }
}

// キー入力処理
void InputKey(nes::Emulator* emu)
{
    const Uint8* keyInput = SDL_GetKeyboardState(NULL);

    // キー入力の状態に応じてNESのボタン操作を設定
    if (keyInput[SDL_SCANCODE_LEFT]) {
        emu->PushButton(nes::PadId::Zero, nes::PadButton::LEFT);
    } else {
        emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::LEFT);
    }

    if (keyInput[SDL_SCANCODE_RIGHT]) {
        emu->PushButton(nes::PadId::Zero, nes::PadButton::RIGHT);
    } else {
        emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::RIGHT);
    }

    if (keyInput[SDL_SCANCODE_UP]) {
        emu->PushButton(nes::PadId::Zero, nes::PadButton::UP);
    } else {
        emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::UP);
    }

    if (keyInput[SDL_SCANCODE_DOWN]) {
        emu->PushButton(nes::PadId::Zero, nes::PadButton::DOWN);
    } else {
        emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::DOWN);
    }

    if (keyInput[SDL_SCANCODE_Z]) {
        emu->PushButton(nes::PadId::Zero, nes::PadButton::A);
    } else {
        emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::A);
    }

    if (keyInput[SDL_SCANCODE_X]) {
        emu->PushButton(nes::PadId::Zero, nes::PadButton::B);
    } else {
        emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::B);
    }

    if (keyInput[SDL_SCANCODE_S]) {
        emu->PushButton(nes::PadId::Zero, nes::PadButton::SELECT);
    } else {
        emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::SELECT);
    }

    if (keyInput[SDL_SCANCODE_A]) {
        emu->PushButton(nes::PadId::Zero, nes::PadButton::START);
    } else {
        emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::START);
    }
}

// SDL2で画面を更新
void DrawEmulatedPicture(SDL_Renderer* renderer, nes::Color result[][256])
{
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, NesGraphicWidth * 2, NesGraphicHeight * 2);

    // ピクセルデータを準備
    std::vector<Uint8> pixels(NesGraphicWidth * NesGraphicHeight * 3 * 4); // 2倍サイズ、1pxあたり3バイト

    for (int y = 0; y < NesGraphicHeight * 2; ++y) {
        for (int x = 0; x < NesGraphicWidth * 2; ++x) {
            auto& c = result[y / 2][x / 2];
            int pixelIndex = (y * NesGraphicWidth * 2 + x) * 3;

            pixels[pixelIndex + 0] = c.Blue;
            pixels[pixelIndex + 1] = c.Green;
            pixels[pixelIndex + 2] = c.Red;
        }
    }

    // テクスチャの更新
    SDL_UpdateTexture(texture, NULL, pixels.data(), NesGraphicWidth * 2 * 3);

    // 描画
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, NesGraphicWidth * 2, NesGraphicHeight * 2, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // SDL_AudioSpec の設定
    SDL_AudioSpec wantSpec, haveSpec;
    SDL_zero(wantSpec);
    wantSpec.freq = SamplingFreq;
    wantSpec.format = AUDIO_S16SYS;
    wantSpec.channels = 1;
    wantSpec.samples = 2048;
    wantSpec.callback = AudioCallback;

    // // オーディオデバイスのオープン
    // if (SDL_OpenAudioDevice(NULL, 0, &wantSpec, &haveSpec, 0) == 0) {
    //     SDL_PauseAudioDevice(SDL_GetAudioDeviceID(NULL), 0);
    // } else {
    //     std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << std::endl;
    //     return -1;
    // }
    // オーディオデバイスのオープン
    // SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(nullptr, 0, &wantSpec, &haveSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(nullptr, 0, &wantSpec, &haveSpec, 0);

    if (deviceId == 0) {
        std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // オーディオ再生開始
    SDL_PauseAudioDevice(deviceId, 0);
    // ROM読み込み
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadRom(&rom, &size);

    nes::Emulator emu(rom, size, AddWaveSample);

    // ゲーム画面のデータ
    nes::Color result[NesGraphicHeight][NesGraphicWidth];

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            }
        }

        InputKey(&emu);

        // エミュレータ1フレーム進める
        emu.StepFrame();
        emu.GetPicture(result);

        // 画面に描画
        SDL_RenderClear(renderer);
        DrawEmulatedPicture(renderer, result);
        SDL_RenderPresent(renderer);
    }

    return 0;
}
