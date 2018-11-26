echo off
IF NOT EXIST ".\shader\." (
mkdir ".\shader"
)
IF NOT EXIST ".\shader\glsl\." (
mkdir ".\shader\glsl"
)
echo on
shaderc.exe -f .\Draw\vs_draw.sc -o .\shader\glsl\vs_draw.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform linux -p 120 --type vertex -O3
shaderc.exe -f .\Draw\fs_draw.sc -o .\shader\glsl\fs_draw.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Simple\vs_poscolor.sc -o .\shader\glsl\vs_poscolor.bin  -i .\ --varyingdef .\Simple\varying.def.sc --platform linux -p 120 --type vertex -O3
shaderc.exe -f .\Simple\fs_poscolor.sc -o .\shader\glsl\fs_poscolor.bin  -i .\ --varyingdef .\Simple\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Sprite\fs_sprite.sc -o .\shader\glsl\fs_sprite.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Sprite\fs_spritelight.sc -o .\shader\glsl\fs_spritelight.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform linux -p 120 --type fragment -O3
::shaderc.exe -f .\Sprite\vs_spritemodel.sc -o .\shader\glsl\vs_spritemodel.bin.h --bin2c spritemodedx11  -i .\ --varyingdef .\Draw\varying.def.sc --platform windows -p vs_4_0 -O 3 --type vertex -O3
shaderc.exe -f .\Sprite\vs_spritemodel.sc -o .\shader\glsl\vs_spritemodel.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform linux -p 120 --type vertex -O3
shaderc.exe -f .\Sprite\fs_spritegray.sc -o .\shader\glsl\fs_spritegray.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Sprite\fs_spritealphatest.sc -o .\shader\glsl\fs_spritealphatest.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Label\vs_labelposition.sc -o .\shader\glsl\vs_labelposition.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform linux -p 120 --type vertex -O3
shaderc.exe -f .\Label\vs_label.sc -o .\shader\glsl\vs_label.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform linux -p 120 --type vertex -O3
shaderc.exe -f .\Label\fs_labelnormal.sc -o .\shader\glsl\fs_labelnormal.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labeloutline.sc -o .\shader\glsl\fs_labeloutline.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labelgradient.sc -o .\shader\glsl\fs_labelgradient.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labelgradientoutline.sc -o .\shader\glsl\fs_labelgradientoutline.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labeldfglow.sc -o .\shader\glsl\fs_labeldfglow.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labeldf.sc -o .\shader\glsl\fs_labeldf.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform linux -p 120 --type fragment -O3
shaderc.exe -f .\Draw\vs_graphics.sc -o .\shader\glsl\vs_graphics.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform linux -p 120 --type vertex -O3
shaderc.exe -f .\Draw\fs_graphics.sc -o .\shader\glsl\fs_graphics.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform linux -p 120 --type fragment -O3