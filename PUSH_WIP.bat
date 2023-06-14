@echo off 

:: not working

IF "%~1"=="" (
  echo "no commit message given" 
  goto jmp_exit
)
:: ELSE (
  :: git submodule foreach git add .
  :: git submodule foreach git commit -m %1
  :: git submodule foreach git push origin main
::   goto jmp_exit
:: 
::   :: -- global --
::   cd src\global
::   echo -- global -- 
::   goto submodule_push
:: 
::   :: -- math --
::   cd src\math
::   echo -- math -- 
::   goto submodule_push
::   
::   :: -- phys --
::   cd src\phys
::   echo -- phys -- 
::   goto submodule_push
::     
::   :: -- serialization --
::   cd src\serialization
::   echo -- serialization -- 
::   goto submodule_push
::   
::   :: -- text --
::   cd src\text
::   echo -- text -- 
::   goto submodule_push
:: 
::   :: -- main repo --
::   goto push
:: )

goto push

git submodule foreach git add .
git submodule foreach git commit -m %1
git submodule foreach git push origin main

goto jmp_exit

:push
git add .
git commit -m %1
git push origin main

:submodule_push
git checkout main
git add .
git commit -m %1
git push origin main
cd ..\..
git add .

:jmp_exit
