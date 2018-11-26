echo off
IF NOT EXIST ".\shader\." (
mkdir ".\shader"
)
IF NOT EXIST ".\shader\metal\." (
mkdir ".\shader\metal"
)
echo on
shaderc.exe -f .\Draw\vs_draw.sc -o .\shader\metal\vs_draw.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform ios -p metal --type vertex -O3
shaderc.exe -f .\Draw\fs_draw.sc -o .\shader\metal\fs_draw.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Simple\vs_poscolor.sc -o .\shader\metal\vs_poscolor.bin  -i .\ --varyingdef .\Simple\varying.def.sc --platform ios -p metal --type vertex -O3
shaderc.exe -f .\Simple\fs_poscolor.sc -o .\shader\metal\fs_poscolor.bin  -i .\ --varyingdef .\Simple\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Sprite\vs_sprite.sc -o .\shader\metal\vs_sprite.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform ios -p metal --type vertex -O3
shaderc.exe -f .\Sprite\vs_spritemodel.sc -o .\shader\metal\vs_spritemodel.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform ios -p metal --type vertex -O3
shaderc.exe -f .\Sprite\fs_sprite.sc -o .\shader\metal\fs_sprite.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Sprite\fs_spritegray.sc -o .\shader\metal\fs_spritegray.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Sprite\fs_spritealphatest.sc -o .\shader\metal\fs_spritealphatest.bin  -i .\ --varyingdef .\Sprite\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Label\vs_labelposition.sc -o .\shader\metal\vs_labelposition.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p metal --type vertex -O3
shaderc.exe -f .\Label\vs_label.sc -o .\shader\metal\vs_label.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p metal --type vertex -O3
shaderc.exe -f .\Label\fs_labelnormal.sc -o .\shader\metal\fs_labelnormal.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Label\fs_labeloutline.sc -o .\shader\metal\fs_labeloutline.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Label\fs_labelgradient.sc -o .\shader\metal\fs_labelgradient.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Label\fs_labelgradientoutline.sc -o .\shader\metal\fs_labelgradientoutline.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Label\fs_labeldfglow.sc -o .\shader\metal\fs_labeldfglow.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Label\fs_labeldf.sc -o .\shader\metal\fs_labeldf.bin  -i .\ --varyingdef .\Label\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Draw\vs_graphics.sc -o .\shader\metal\vs_graphics.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform ios -p metal --type vertex -O3
shaderc.exe -f .\Draw\fs_graphics.sc -o .\shader\metal\fs_graphics.bin  -i .\ --varyingdef .\Draw\varying.def.sc --platform ios -p metal --type fragment -O3
shaderc.exe -f .\Simple\vs_poscolor.sc -o .\shader\metal\vs_poscolor.bin  -i .\ --varyingdef .\Simple\varying.def.sc --platform ios -p metal --type vertex -O3
shaderc.exe -f .\Simple\fs_poscolor.sc -o .\shader\metal\fs_poscolor.bin  -i .\ --varyingdef .\Simple\varying.def.sc --platform ios -p metal --type fragment -O3