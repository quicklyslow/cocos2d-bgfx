echo off
IF NOT EXIST ".\shader\." (
mkdir ".\shader"
)
IF NOT EXIST ".\shader\essl\." (
mkdir ".\shader\essl"
)
echo on
shaderc.exe -f .\Draw\vs_draw.sc -o .\shader\essl\vs_draw.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform ios -p 120 --type vertex -O3
shaderc.exe -f .\Draw\fs_draw.sc -o .\shader\essl\fs_draw.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Sprite\vs_spritemodel.sc -o .\shader\essl\vs_spritemodel.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform ios -p 120 --type vertex -O3
shaderc.exe -f .\Sprite\fs_spritegray.sc -o .\shader\essl\fs_spritegray.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Sprite\fs_spritealphatest.sc -o .\shader\essl\fs_spritealphatest.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Label\vs_labelposition.sc -o .\shader\essl\vs_labelposition.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p 120 --type vertex -O3
shaderc.exe -f .\Label\vs_label.sc -o .\shader\essl\vs_label.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p 120 --type vertex -O3
shaderc.exe -f .\Label\fs_labelnormal.sc -o .\shader\essl\fs_labelnormal.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labeloutline.sc -o .\shader\essl\fs_labeloutline.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labelgradient.sc -o .\shader\essl\fs_labelgradient.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labelgradientoutline.sc -o .\shader\essl\fs_labelgradientoutline.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labeldfglow.sc -o .\shader\essl\fs_labeldfglow.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Label\fs_labeldf.sc -o .\shader\essl\fs_labeldf.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p 120 --type fragment -O3
shaderc.exe -f .\Draw\vs_graphics.sc -o .\shader\essl\vs_graphics.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform ios -p 120 --type vertex -O3
shaderc.exe -f .\Draw\fs_graphics.sc -o .\shader\essl\fs_graphics.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform ios -p 120 --type fragment -O3