$ExtensionName = "benzin-lang"
$SourceDir = $PSScriptRoot
$DestDir = "$env:USERPROFILE\.vscode\extensions\$ExtensionName"

Write-Host "Instalowanie rozszerzenia $ExtensionName..."

if (Test-Path $DestDir) {
    Write-Host "Usuwanie starej wersji..."
    Remove-Item -Recurse -Force $DestDir
}

Write-Host "Kopiowanie plików do $DestDir..."
New-Item -ItemType Directory -Force -Path $DestDir | Out-Null
Copy-Item -Path "$SourceDir\*" -Destination $DestDir -Recurse -Force -Exclude "install.ps1"

Write-Host "Zakończono pomyślnie! Zrestartuj VS Code, aby zobaczyć zmiany." -ForegroundColor Green
