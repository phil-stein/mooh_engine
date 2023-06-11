@echo off 

:: not working

IF "%~1"=="" (
  echo "no commit message given" 
  goto jmp_exit
)
ELSE (
  :: git submodule foreach git add .
  :: git submodule foreach git commit -m %1
  :: git submodule foreach git push origin main

  :: -- global --
  cd src\global
  goto submodule_push

  :: -- math --
  cd src\math
  goto submodule_push
  
  :: -- phys --
  cd src\phys
  goto submodule_push
    
  :: -- serialization --
  cd src\serialization
  goto submodule_push
  
  :: -- text --
  cd src\text
  goto submodule_push

  :: -- main repo --
  goto push
)

goto jmp_exit

:push
git add .
git commit -m %1
git push origin main

:submodule_push
git checkout main
goto push
cd ..\..
git add .

:jmp_exit
