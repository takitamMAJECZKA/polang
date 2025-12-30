@echo off
title Instalator Benzin
color 0A

echo ===================================================
echo        INSTALATOR JEZYKA BENZIN
echo ===================================================
echo.

:: Sprawdzanie uprawnien administratora
net session >nul 2>&1
if %errorLevel% == 0 (
    echo [OK] Uprawnienia Administratora wykryte.
) else (
    color 0C
    echo [BLAD] Brak uprawnien Administratora!
    echo.
    echo Prosze kliknac prawym przyciskiem myszy na ten plik
    echo i wybrac "Uruchom jako administrator".
    echo.
    pause
    exit
)

echo.
echo 1. Tworzenie folderu C:\Benzin...
if not exist "C:\Benzin" mkdir "C:\Benzin"

echo 2. Kopiowanie plikow...
copy /Y "%~dp0benzin.exe" "C:\Benzin\benzin.exe" >nul
if %errorLevel% neq 0 (
    color 0C
    echo [BLAD] Nie udalo sie skopiowac pliku benzin.exe!
    pause
    exit
)

echo 3. Konfiguracja zmiennej PATH...
powershell -Command "$oldPath = [Environment]::GetEnvironmentVariable('Path', 'Machine'); if ($oldPath -notlike '*C:\Benzin*') { [Environment]::SetEnvironmentVariable('Path', $oldPath + ';C:\Benzin', 'Machine'); Write-Host '   -> Dodano C:\Benzin do zmiennej systemowej PATH.' } else { Write-Host '   -> C:\Benzin juz znajduje sie w PATH.' }"

echo.
echo ===================================================
echo        INSTALACJA ZAKONCZONA SUKCESEM!
echo ===================================================
echo.
echo Aby uzyc jezyka, otworz NOWE okno konsoli (CMD lub PowerShell)
echo i wpisz komende:
echo.
echo    benzin twoj_plik.bzn
echo.
pause