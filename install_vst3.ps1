# Install SJ Channel Strip VST3 to FL Studio plugin directory
param (
    [string]$Config = "Release",
    [switch]$DeployStandalone = $false
)

$sourceDir = "$PSScriptRoot\build-release\SJChannelStrip_artefacts\$Config\VST3\SJ Channel Strip.vst3"
if (-not (Test-Path $sourceDir)) {
    $sourceDir = "$PSScriptRoot\build\SJChannelStrip_artefacts\Debug\VST3\SJ Channel Strip.vst3"
    $Config = "Debug"
}

if (-not (Test-Path $sourceDir)) {
    Write-Error "Could not find built VST3 at $sourceDir. Please build the project first."
    exit 1
}

# 1. User VST3 location (Steinberg standard, scanned by FL Studio without requiring admin permissions)
$userVst3Dir = "$env:LOCALAPPDATA\Programs\Common\VST3"
if (-not (Test-Path $userVst3Dir)) {
    New-Item -ItemType Directory -Force -Path $userVst3Dir | Out-Null
}

$destDir = "$userVst3Dir\SJ Channel Strip.vst3"
Write-Host "Installing SJ Channel Strip ($Config) to $destDir..." -ForegroundColor Cyan
if (Test-Path $destDir) {
    Remove-Item -Recurse -Force $destDir
}
Copy-Item -Recurse -Force -Path $sourceDir -Destination $userVst3Dir

Write-Host "Successfully installed SJ Channel Strip VST3 to:" -ForegroundColor Green
Write-Host "  $destDir" -ForegroundColor White

# 2. Check if System VST3 location is writable (if elevated)
$sysVst3Dir = "C:\Program Files\Common Files\VST3"
if (Test-Path $sysVst3Dir) {
    try {
        $sysDestDir = "$sysVst3Dir\SJ Channel Strip.vst3"
        Copy-Item -Recurse -Force -Path $sourceDir -Destination $sysVst3Dir -ErrorAction Stop
        Write-Host "Also installed to system-wide VST3 directory:" -ForegroundColor Green
        Write-Host "  $sysDestDir" -ForegroundColor White
    } catch {
        # System directory requires elevated admin rights; standard user directory is already active
    }
}

# 3. Optional Standalone Deployment
if ($DeployStandalone) {
    $standaloneExe = "$PSScriptRoot\build-release\SJChannelStrip_artefacts\$Config\Standalone\SJ Channel Strip.exe"
    if (Test-Path $standaloneExe) {
        $standaloneDest = "$env:APPDATA\SJ Audio\SJ Channel Strip\Standalone"
        if (-not (Test-Path $standaloneDest)) {
            New-Item -ItemType Directory -Force -Path $standaloneDest | Out-Null
        }
        Copy-Item -Force -Path $standaloneExe -Destination $standaloneDest
        Write-Host "Standalone executable deployed to:" -ForegroundColor Green
        Write-Host "  $standaloneDest\SJ Channel Strip.exe" -ForegroundColor White
    }
}

Write-Host "`nReady for FL Studio!" -ForegroundColor Yellow
Write-Host "Open FL Studio > Options > Manage Plugins > 'Find installed plugins'." -ForegroundColor Yellow
