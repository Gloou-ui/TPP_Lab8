$ErrorActionPreference = 'Stop'
$null = & $env:ComSpec /c 'chcp 65001'
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)

$headerRoot = Join-Path $env:TEMP 'sqlite_orm_1_9_1'
$headerDir = Join-Path $headerRoot 'sqlite_orm'
$header = Join-Path $headerDir 'sqlite_orm.h'

if (-not (Test-Path $header)) {
    New-Item -ItemType Directory -Force -Path $headerDir | Out-Null
    Invoke-WebRequest `
        -Uri 'https://raw.githubusercontent.com/fnc12/sqlite_orm/v1.9.1/include/sqlite_orm/sqlite_orm.h' `
        -OutFile $header `
        -UseBasicParsing `
        -TimeoutSec 60
}

New-Item -ItemType Directory -Force -Path 'build' | Out-Null
& 'C:\msys64\mingw64\bin\g++.exe' `
    -w `
    -std=c++20 `
    -Iinclude `
    "-I$headerRoot" `
    src\main.cpp `
    src\doctor_repository.cpp `
    src\ticket_repository.cpp `
    src\sqlite_api_demo.cpp `
    src\demo.cpp `
    -o build\clinic_queue.exe `
    -lsqlite3

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& .\build\clinic_queue.exe
exit $LASTEXITCODE
