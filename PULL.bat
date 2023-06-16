@echo off 

:: not working

IF "%~1"=="" (
  echo "no commit message given" 
  goto jmp_exit
)


:: git add .
:: git commit -m %1
:: git push origin main
:: 
:: git submodule foreach git add .
:: git submodule foreach git commit -m %1
:: git submodule foreach git push origin main
::   
:: goto jmp_exit


:: ELSE (
  :: git submodule foreach git add .
  :: git submodule foreach git commit -m %1
  :: git submodule foreach git push origin main
::   goto jmp_exit
:: 
:: -- global --
cd src\global
echo -- global -- 
:: goto submodule_push
  git checkout main
  git add .
  git commit -m %1
  git pull origin main
  cd ..\..
  git add .

:: -- math --
cd src\math
echo -- math -- 
:: goto submodule_push
  git checkout main
  git add .
  git commit -m %1
  git pull origin main
  cd ..\..
  git add .


:: -- phys --
cd src\phys
echo -- phys -- 
:: goto submodule_push
  git checkout main
  git add .
  git commit -m %1
  git pull origin main
  cd ..\..
  git add .     

  :: -- serialization --
cd src\serialization
echo -- serialization -- 
:: goto submodule_push
  git checkout main
  git add .
  git commit -m %1
  git pull origin main
  cd ..\..
  git add .     

:: -- text --
cd src\text
echo -- text -- 
:: goto submodule_push
  git checkout main
  git add .
  git commit -m %1
  git pull origin main
  cd ..\..
  git add .

:: -- main repo --
echo -- text -- 
:: goto push
git add .
git commit -m %1
git pull origin main

goto jmp_exit

:: :push
:: git add .
:: git commit -m %1
:: git push origin main

:: :submodule_push
::   git checkout main
::   git add .
::   git commit -m %1
::   git push origin main
::   cd ..\..
::   git add .

:jmp_exit
