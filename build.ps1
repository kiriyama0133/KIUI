<#
Simple build script to avoid typing CMake toolchain flags.

Usage:
  ./build.ps1                    # configure + build Debug version (default)
  ./build.ps1 -Release           # configure + build Release version
  ./build.ps1 -Debug             # configure + build Debug version
  ./build.ps1 -Clean             # delete the preset build dir first
  ./build.ps1 -Release -Clean   # clean and build Release version
  ./build.ps1 -Release -Install # build and install to SDK directory

Relies on CMake preset "classic-vcpkg" defined in CMakePresets.json and
VCPKG_ROOT pointing to your vcpkg clone.
#>

param(
    [switch]$Clean,
    [switch]$Release,
    [switch]$Debug,
    [switch]$Install
)

$preset = "classic-vcpkg"
$buildDir = Join-Path $PSScriptRoot "out/build/$preset"

# Determine build type
if ($Release) {
    $buildType = "Release"
} elseif ($Debug) {
    $buildType = "Debug"
} else {
    $buildType = "Debug"
}

# Check VCPKG_ROOT
if (-not $env:VCPKG_ROOT) {
    Write-Error "VCPKG_ROOT is not set. Please set it to your vcpkg path."
    exit 1
}

# Clean build directory if requested
if ($Clean -and (Test-Path $buildDir)) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}

# Configure
Write-Host "Configuring with preset '$preset'..." -ForegroundColor Cyan
cmake --preset $preset
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

# Build
Write-Host "Building $buildType version..." -ForegroundColor Green
cmake --build $buildDir --config $buildType
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "Output directory: $buildDir\$buildType" -ForegroundColor Cyan

# Show build artifacts
$foundationLib = Join-Path $buildDir "foundation\$buildType\Foundation.lib"
if (Test-Path $foundationLib) {
    Write-Host "Foundation library: $foundationLib" -ForegroundColor Green
}

$foundationTests = Join-Path $buildDir "foundation\$buildType\FoundationTests.exe"
if (Test-Path $foundationTests) {
    Write-Host "Foundation tests: $foundationTests" -ForegroundColor Green
}

# Install if requested
if ($Install) {
    Write-Host "Installing to SDK directory..." -ForegroundColor Cyan
    cmake --install $buildDir --config $buildType
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Installation failed!"
        exit $LASTEXITCODE
    }
    
    $sdkDir = Join-Path $PSScriptRoot "SDK"
    if (Test-Path $sdkDir) {
        Write-Host "Installation completed!" -ForegroundColor Green
        Write-Host "SDK directory: $sdkDir" -ForegroundColor Cyan
        Write-Host "  - Headers: $sdkDir\include" -ForegroundColor Yellow
        Write-Host "  - Libraries: $sdkDir\lib" -ForegroundColor Yellow
    }
}

exit $LASTEXITCODE
