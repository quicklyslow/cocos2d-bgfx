cocos2d-bgfx
==========================

[cocos2d-x-lite](https://github.com/cocos-creator/cocos2d-x-lite/tree/v1.10-release) use [bgfx](https://github.com/bkaradzic/bgfx) as the rendering backend

------------------------------------------------

### It is deeply inspired by [Dorothy-SSR](https://github.com/IppClub/Dorothy-SSR). A simple and clean 2d game engine, without it I definitely could not finish this.

------------------------------------------------
Currently it is not fully comaptible with cocos2d-x-lite, because some rendering node type is not implemented completely.
```
- The complete parts:
  - Sprite(used by System Font label atlas in CocosCreator)
  - Scale9SpriteV2(used by CocosCreator as Sprite component)
  - Label(add gradient), but need optimization heavily
  - ParticleSystem(maybe missing postStep)
  - ClippingNode(with alphaRef can be used by image stencil)
  - GraphicsNode
  - DrawNode(not completed)
  - TextureAtlas and so SpriteBatch
  - EditBox(iOS getText must be called in UI Thread, this is not fixed)
  - Spine(TwoColorBatch skeleton is not completed)

- The uncomplete parts:
  - TMXTiledMap(Layer, fast map)
  - RenderTexture
  - MotionStreak
  - Grid
  - DrawingPrimitives
  - DragonBones

- Only windows and iOS is runnable
- The platform specify code for Android is not complete, so Android is not runnable

- bgfx compatible problems
  - [stencil write mask](https://github.com/bkaradzic/bgfx/issues/1541) problem arised when using ClippingNode with more than one layer
  - [24bit rgb texture](https://github.com/bkaradzic/bgfx/issues/1554) 24bit rgb texture on DX12 is not rendered correctly, although this texture format should not be used in production
```

Build Guide
-----------------------
Clone:
```
git clone https://github.com/quicklyslow/cocos2d-bgfx.git
cd cocos2d-bgfx
```
Then update submodules(bgfx,bimg,bx)
```
git submodule init
git submodule update
```
Go to the BGFX/bgfx folder
```
win:..\bx\tools\bin\windows\genie.exe --with-dynamic-runtime vs2017
ios:../bx/tools/bin/darwin/genie --with-tools --xcode=ios xcode9
This will make bgfx use Runtime Library(Multi-threaded /Debug/ DLL), ease a lot of ABI problems across different versions of runtime lib
```
Open simulator.sln
```
cd tools\simulator\frameworks\runtime-src\proj.win32
and open the sln
```
Run
```
Compile shaders and copy shaders and js scripts to simulator folder can just be done
run copyres.bat on root folder.
External libs is required, whether run download-deps.py or copy from CocosCreator's cocos2d-x folder is ok
```

CocosCreator Compatibility
------------------------
```
I had test across ccc from 1.6x to 1.1x, most of them is OK, when compiled the simulator, can use the folder as the Cocos2d-x engine folder in ccc.
Because the render implementation is changes since ccc 2.x, this will not comaptible with ccc 2.x
```

Suggestions and issues
-----------------------
Any suggestion will be a lot of help.

The road ahead
----------------------
```
A lot works need to be done.
First of all is the cleanup of code, approching to the cleaness of Dorothy-SSR as much as possible.
Then the optimizations, including Label and Batch and a lot, I need some help from [korok](https://github.com/KorokEngine/Korok)

Improve Locality
```
Contributing to the Project
--------------------------------

cocos2d-bgfx is licensed under the [MIT License](https://opensource.org/licenses/MIT). Anyone is welcome participation
