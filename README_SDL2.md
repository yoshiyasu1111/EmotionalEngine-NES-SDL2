# SDL2に移植した

とりあえずWindows11、Visual Studio 17 2022、SDL2 2.30.11で動作確認できた。

# SDL2をソースコードからビルドする

```bash
git clone --recursive https://github.com/libsdl-org/SDL
cd SDL
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -T host=x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="c:/sdl"
cmake --build build --config RELEASE
cmake --build build --config RELEASE --target INSTALL
```

システム環境変数**SDL2_ROOT**をc:\sdlに設定する。システム環境変数を設定した後はPCを再起動してください。

# EmotionalEngine-NES

本体EmotionalEngine-NESのビルド

```bash
git clone https://github.com/ymduu/EmotionalEngine-NES.git
cd EmotionalEngine-NES
cmake -S . -B .\Outputs\ -G "Visual Studio 17 2022" -T host=x64 -A x64
cmake --build .\Outputs\
```

- Unitテスト

```bash
.\Outputs\Tests\Unit\cassette\Debug\testUnit_Cassette.exe
HELLO, WORLD!
```

- Systemテスト

giko005とりあえずコメントアウト

```bash
 .\Outputs\Tests\Integrate\System\Debug\testIntegrate_System.exe
==== TestSystem_ReadWrite ====
Entry Point: 0x8000
====TestSystem_ReadWrite END ====
==== TestSystem_HelloWorld ====
====TestSystem_HelloWorld END ====
==== TestSystem_NesTest ====
==== TestSystem_NesTest OK ====
==== TestSystem_HelloWorld_Cpu_Ppu ====
====TestSystem_HelloWorld_Cpu_Ppu END ====
```

- Allテスト

testIntegrate_All.exeと同じディレクトリにSDL2.dllを配置する

```bash
.\Outputs\Tests\Integrate\All\Debug\testIntegrate_All.exe
```
