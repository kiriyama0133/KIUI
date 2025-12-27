# 清理所有构建目录并重新配置
Write-Host "Cleaning build directories..." -ForegroundColor Yellow

# 删除旧的构建目录
Remove-Item -Recurse -Force "build" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "out" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force ".vs" -ErrorAction SilentlyContinue

Write-Host "Build directories cleaned." -ForegroundColor Green
Write-Host "Now run: .\build.ps1" -ForegroundColor Green

