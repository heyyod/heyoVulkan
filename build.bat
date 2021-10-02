@echo off

set COMPILER_FLAGS=-MTd -WL -O2 -nologo -fp:fast -fp:except- -Gm- -EHsc -Zo -Oi -W4 -wd4100 -wd4458 -wd4505 -wd4201 -FC -Zi -GS-
set LINKER_FLAGS=-subsystem:windows -incremental:no -opt:ref user32.lib gdi32.lib
set EXE_NAME=main

if not exist .\\build mkdir .\\build
pushd .\\build

del *.pdb > NUL 2> NUL
cl  %COMPILER_FLAGS% ..\\code\\hy3d_engine.cpp -Fmhy3d_engine.map -LD -link -incremental:no -opt:ref  -PDB:hy3d_engine_%RANDOM%.pdb -EXPORT:UpdateAndRender
cl  %COMPILER_FLAGS% ..\\code\\win32_platform.cpp -Fmwin32_platform.map -Fe%EXE_NAME% -link %LINKER_FLAGS%
popd
