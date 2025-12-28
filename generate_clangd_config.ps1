<#
Script to generate .clangd configuration with vcpkg paths
Run this script to update .clangd with your current vcpkg installation path
#>

param(
    [string]$VcpkgRoot = $env:VCPKG_ROOT
)

if (-not $VcpkgRoot) {
    Write-Error "VCPKG_ROOT environment variable is not set. Please set it or pass -VcpkgRoot parameter."
    exit 1
}

if (-not (Test-Path $VcpkgRoot)) {
    Write-Error "VCPKG_ROOT path does not exist: $VcpkgRoot"
    exit 1
}

$vcpkgInclude = $VcpkgRoot.Replace('\', '/') + '/installed/x64-windows/include'
$vcpkgBoost = $vcpkgInclude + '/boost'

if (-not (Test-Path $vcpkgInclude)) {
    Write-Warning "vcpkg include path does not exist: $vcpkgInclude"
    Write-Warning "Make sure you have installed packages with: vcpkg install <package> --triplet x64-windows"
}

$workspaceRoot = $PSScriptRoot.Replace('\', '/')

$clangdConfig = @"
CompileFlags:
  Add:
    # C++20 standard
    - -std=c++20
    # vcpkg include paths
    - -I$vcpkgInclude
    - -I$vcpkgBoost
    # Project include paths
    - -I$workspaceRoot/foundation/src
    - -I$workspaceRoot/foundation/src/hpps
    - -I$workspaceRoot/graphics/include
    - -I$workspaceRoot/SDK/include
    # Windows SDK paths
    - -D_WIN32_WINNT=0x0A00
    - -DUNICODE
    - -D_UNICODE
    # MSVC compatibility
    - -fms-compatibility
    - -fms-extensions
    # Boost
    - -DBOOST_ALL_NO_LIB
    # Foundation defines
    - -DFOUNDATION_USE_ANGLE
    - -DFOUNDATION_USE_GLFW
    # Graphics defines
    - -DGRAPHICS_USE_ANGLE
    - -DGRAPHICS_USE_SKIA
  Remove:
    # Remove any conflicting flags
    - -W*

Diagnostics:
  UnusedIncludes: Relaxed
  MissingIncludes: Relaxed
  
Index:
  Background: Build

InlayHints:
  Enabled: Yes
  ParameterNames: Yes
  DeducedTypes: Yes
"@

$clangdConfig | Out-File -FilePath "$PSScriptRoot/.clangd" -Encoding UTF8 -NoNewline

Write-Host "Generated .clangd configuration with vcpkg path: $vcpkgInclude" -ForegroundColor Green
Write-Host "Please restart your editor/IDE for clangd to pick up the changes." -ForegroundColor Yellow

