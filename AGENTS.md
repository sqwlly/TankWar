# Repository Guidelines

## Project Structure & Module Organization
Source code lives under `src/` (core loop, entities, states, collision, rendering, input) with mirrored public headers in `include/`. Runtime data such as sprites, audio, and levels are in `assets/` and copied next to the built binary. Tests reside in `tests/` (GoogleTest under `tests/unit/` plus mocks in `tests/mocks/`). Helper scripts and toolchains are under `scripts/` and `cmake/`. Generated folders (`build*/`, `deps/`, `release/`) stay out of version control.

## Build, Test, and Development Commands
Configure and build on Linux/macOS:
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/TankGame
```
Enable unit tests:
```
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```
Windows developers should follow `BUILD_WINDOWS.md`. Cross-compilation scripts live in `scripts/` (e.g., `./scripts/cross-build.sh --package`).

## Coding Style & Naming Conventions
The project targets C++17 with 4-space indentation and K&R braces. Types use PascalCase, functions camelCase, members end with `_`, and constants prefer `tank::Constants::UPPER_SNAKE_CASE`. Headers require `#pragma once`. Keep modules small, reuse existing layers instead of adding new abstractions, and maintain ASCII unless assets require otherwise.

## Testing Guidelines
GoogleTest drives unit tests. Place new suites under `tests/unit/`, name files with `*Test.cpp`, and reuse mocks from `tests/mocks/`. Tests should be deterministic and SDL-free whenever possible. Run `ctest --test-dir build --output-on-failure` before submitting; add focused regression tests when fixing bugs.

## Commit & Pull Request Guidelines
Commit messages follow `type: summary` (e.g., `fix: 修复子弹碰撞`, `feat: add power-up HUD`). Pull requests must describe purpose and validation steps (commands + platform) and include screenshots/GIFs for rendering/UI updates. Link related issues and keep changesets scoped; never commit generated artifacts or downloaded dependencies.
