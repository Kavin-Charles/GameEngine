# GameEngine — Project Memory

> This file serves as persistent memory for AI-assisted development on this project.
> **⚠️ RULE: Update the "What's Been Done" section after EVERY task completed.**
> Last updated: **2026-02-16**

---

## Project Overview

A **C++ game engine** built as a DLL (`Engine.dll`) with a separate client application (`Sandbox.exe`). Follows a Cherno-style engine architecture with an event system, logging, windowing (GLFW), and an abstract OpenGL rendering pipeline. Uses a highly modular, OOP design with abstract interfaces and platform-specific implementations.

---

## Build System

| Tool | Details |
|---|---|
| **Build generator** | [Premake5](https://premake.github.io/) (`premake5.lua` at root) |
| **IDE target** | Visual Studio 2022 (`vs2022`) |
| **Language** | C++17 |
| **Architecture** | x64 only |
| **Configurations** | `Debug`, `Release`, `Dist` |
| **Projects** | `GLFW` (static lib), `Engine` (shared lib/DLL), `Sandbox` (console app) |

### How to Build

```powershell
# 1. Generate VS2022 solution (or run genproject.bat)
vendor\bin\premake\premake5.exe vs2022

# 2. Build with MSBuild
& "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Engine.sln /p:Configuration=Debug /p:Platform=x64 /m

# 3. Run
.\bin\Debug-windows-x86_64\Sandbox\Sandbox.exe
```

### Build Output Paths

```
bin/<Config>-windows-x86_64/GLFW/      → GLFW.lib (static)
bin/<Config>-windows-x86_64/Engine/    → Engine.dll
bin/<Config>-windows-x86_64/Sandbox/   → Sandbox.exe + Engine.dll (copied via post-build)
bin-int/<Config>-windows-x86_64/       → intermediate obj files
```

### Important Build Flags

- `/utf-8` — required by spdlog for Unicode support on MSVC
- `GLFW_INCLUDE_NONE` — prevents GLFW from pulling in its own GL headers (we use GLAD)
- C4251 warning disabled — harmless warning for DLL-exporting classes with `std::shared_ptr` members
- `ENGINE_BUILD_DLL` defined when compiling Engine (exports symbols)
- `ENGINE_PLATFORM_WINDOWS` defined for both Engine and Sandbox

---

## Project Structure

```
GameEngine/
├── premake5.lua              # Build config (GLFW + ImGui + Engine + Sandbox)
├── genproject.bat            # Shortcut: runs premake5.exe vs2022
│
├── Engine/
│   ├── src/
│   │   ├── Engine.h          # Master include header
│   │   ├── Engine/
│   │   │   ├── Core.h        # ENGINE_API macro, BIT() macro
│   │   │   ├── Application.h/.cpp  # App base class (owns Window, LayerStack, ImGuiLayer)
│   │   │   ├── Window.h      # Abstract Window interface + factory
│   │   │   ├── Layer.h       # Abstract Layer (OnAttach/OnDetach/OnUpdate/OnImGuiRender)
│   │   │   ├── LayerStack.h/.cpp   # Ordered layer management (layers + overlays)
│   │   │   ├── Input.h       # Abstract Input polling (static IsKeyPressed, GetMousePos)
│   │   │   ├── Log.h/.cpp    # spdlog wrapper
│   │   │   ├── EntryPoint.h  # main()
│   │   │   ├── Events/       # Event system (wired to GLFW callbacks)
│   │   │   ├── ImGui/
│   │   │   │   └── ImGuiLayer.h/.cpp  # ImGui lifecycle as overlay Layer
│   │   │   └── Renderer/
│   │   │       ├── Renderer.h/.cpp        # Static Renderer
│   │   │       ├── Shader.h / Buffer.h / VertexArray.h  # Abstract interfaces
│   │   │       ├── Camera.h/.cpp          # PerspectiveCamera + SetPositionAndTarget
│   │   │       ├── CameraController.h/.cpp # FPS camera (WASD + right-click mouse look)
│   │   │       └── Mesh.h/.cpp            # Mesh + UV sphere factory
│   │   └── Platform/
│   │       ├── Windows/
│   │       │   ├── WindowsWindow.h/.cpp    # GLFW window
│   │       │   └── WindowsInput.h/.cpp     # GLFW input polling
│   │       └── OpenGL/
│   │           ├── OpenGLBuffer.h/.cpp
│   │           ├── OpenGLShader.h/.cpp
│   │           └── OpenGLVertexArray.h/.cpp
│   └── vendor/
│       ├── spdlog/ glfw/ glad/ glm/ imgui/
│
├── Sandbox/
│   └── src/
│       └── Sandbox.cpp       # SandboxLayer with ImGui sphere editor + CameraController
│
└── vendor/bin/premake/premake5.exe
```

---

## Architecture

### DLL Export Model

- `Core.h` defines `ENGINE_API` macro:
  - When `ENGINE_BUILD_DLL` is defined (Engine project): `__declspec(dllexport)`
  - Otherwise (Sandbox/client): `__declspec(dllimport)`
- All public engine classes use `ENGINE_API` for symbol export

### Entry Point (`EntryPoint.h`)

The `main()` function lives **inside the Engine** (in `EntryPoint.h`), not in Sandbox. Flow:

1. `Engine::Log::Init()` — sets up spdlog loggers
2. Logs `"Engine Initialized!"`
3. Calls `Engine::createApplication()` — implemented by client (Sandbox)
4. Calls `app->Run()` — the main game loop
5. Deletes the app

### Application Class

- `Engine::Application` — base class that owns the `Window`
- Creates window in constructor, initializes `Renderer`
- `Run()` — game loop with delta time: calls `OnUpdate(deltaTime)` then `window->OnUpdate()`
- Handles `WindowCloseEvent` → sets `m_Running = false`
- Handles `WindowResizeEvent` → updates viewport
- Client extends it and overrides `OnUpdate(float dt)`
- Static `Application::Get()` accessor for singleton

### Abstract Window System

- `Window` — abstract interface with `OnUpdate()`, `GetWidth/Height()`, `SetEventCallback()`, `SetVSync()`
- `Window::Create(props)` — factory method, returns platform-specific implementation
- `WindowsWindow` — GLFW implementation with full event callback wiring (resize, close, key, mouse)
- Initializes GLAD for OpenGL function loading

### Abstract Renderer API

All renderer interfaces are **API-agnostic** — OpenGL implementations live in `Platform/OpenGL/`:

| Interface | Factory | OpenGL Implementation |
|---|---|---|
| `Shader` | `Shader::Create(vert, frag)` | `OpenGLShader` — compiles/links GLSL, uniforms |
| `VertexBuffer` | `VertexBuffer::Create(data, size)` | `OpenGLVertexBuffer` — GL buffer objects |
| `IndexBuffer` | `IndexBuffer::Create(data, count)` | `OpenGLIndexBuffer` — GL element buffers |
| `VertexArray` | `VertexArray::Create()` | `OpenGLVertexArray` — GL VAO with auto attribute config |

- `BufferLayout` — describes vertex attributes (`ShaderDataType`, name, offset, stride)
- `Renderer` — static class: `Init()`, `BeginScene(camera)`, `Submit(shader, vao, transform)`, `EndScene()`
- `PerspectiveCamera` — position, rotation, FOV → computes view-projection matrix (GLM)
- `Mesh` — holds a `VertexArray`, factory `CreateSphere(radius, sectors, stacks)`

### Logging (`Log.h` / `Log.cpp`)

- Wraps **spdlog** with two loggers:
  - `s_CoreLogger` ("Engine") — internal engine logs
  - `s_ClientLogger` ("Client") — client application logs
- Macros: `ENGINE_LOG_ERROR/WARN/INFO/TRACE`, `CLIENT_LOG_ERROR/WARN/INFO/TRACE`

### Event System (`Events/`)

Wired up to GLFW via `WindowsWindow` callbacks:

| Category | Events |
|---|---|
| Application | `WindowClose`, `WindowResize`, `AppTick`, `AppUpdate`, `AppRender` |
| Keyboard | `KeyPressed` (with repeat count), `KeyReleased` |
| Mouse | `MouseMoved`, `MouseScrolled`, `MouseButtonPressed`, `MouseButtonReleased` |

---

## Dependencies

| Dependency | Location | Purpose |
|---|---|---|
| **spdlog** | `Engine/vendor/spdlog` (git submodule) | Fast C++ logging |
| **GLFW** | `Engine/vendor/glfw` (git clone) | Windowing and input |
| **GLAD** | `Engine/vendor/glad` (generated) | OpenGL 4.6 Core function loader |
| **GLM** | `Engine/vendor/glm` (git clone) | Math library (vectors, matrices) |
| **ImGui** | `Engine/vendor/imgui` (git clone) | Debug UI overlay (GLFW + OpenGL3 backends) |
| **Premake5** | `vendor/bin/premake/premake5.exe` | Build configuration generator |

---

## Known Issues & Gotchas

1. **spdlog submodule** — `git submodule update --init` can fail. Fallback: clone directly with `git clone https://github.com/gabime/spdlog.git Engine/vendor/spdlog`
2. **Post-build copy** — uses `{MKDIR}` now in premake to auto-create output dir, but can occasionally fail on first build
3. **GLAD generation** — generated via `python -m glad --api "gl:core=4.6" --out-path Engine/vendor/glad c` (requires `pip install glad2`)

---

## What's Been Done (Session Log)

### 2026-02-16 — Initial Setup & Build
- Cloned `spdlog` submodule (git submodule init failed, cloned directly)
- Downloaded and extracted `premake5.exe` (v5.0.0-beta2) to `vendor/bin/premake/`
- Fixed `#include"memory"` → `#include <memory>` in `Log.h` and `Log.cpp`
- Added `/utf-8` build option to `premake5.lua` for both Engine and Sandbox (required by spdlog)
- Disabled C4251 warning in Engine project (DLL export of `shared_ptr` members)
- Generated VS2022 solution and built successfully
- Ran `Sandbox.exe` — output confirmed working (spdlog init + infinite loop printing "test")
- Created `MEMORY.md` project memory file

### 2026-02-16 — Window + OpenGL Sphere Rendering
- **Dependencies**: Cloned GLFW and GLM into `Engine/vendor/`. Generated GLAD (OpenGL 4.6 Core) via `glad2`
- **Build system**: Rewrote `premake5.lua` — added GLFW static lib project, GLAD source compilation, OpenGL linking, GLM includes, `GLFW_INCLUDE_NONE`, fixed post-build with `{MKDIR}`
- **Abstract Window**: Created `Window` interface (`Window.h`) with factory method. Implemented `WindowsWindow` using GLFW with full event callbacks
- **Abstract Renderer**: Created abstract interfaces for `Shader`, `VertexBuffer`, `IndexBuffer`, `VertexArray` with static `Create()` factories. Created `BufferLayout` descriptor system
- **OpenGL implementations**: `OpenGLShader` (compile/link/uniforms), `OpenGLBuffer` (VBO/IBO), `OpenGLVertexArray` (VAO with auto attribute config from layout)
- **Renderer**: Static `Renderer` class with `Init()`, `BeginScene(camera)`, `Submit(shader, vao, transform)`, `EndScene()`
- **Camera**: `PerspectiveCamera` with position, rotation, FOV, view-projection matrix via GLM
- **Mesh**: `Mesh` class with UV sphere factory (`CreateSphere(radius, sectors, stacks)`)
- **Application**: Rewritten — owns `Window`, has proper game loop with delta time, handles `WindowCloseEvent` and `WindowResizeEvent`, virtual `OnUpdate(float dt)`
- **Event system**: Fixed all headers (angle brackets, `const override`, removed stray declarations, renamed enum collision `None` → `EventCategoryNone`)
- **Sandbox**: Renders a Phong-lit, rotating blue sphere with vertex/fragment shaders
- **Verified**: Build succeeds (0 errors), window opens at 1280×720, OpenGL 4.6.0 confirmed on NVIDIA, sphere renders and rotates

### 2026-02-16 — ImGui Integration + Movable Camera
- **Dependencies**: Cloned ImGui into `Engine/vendor/imgui`. Added as static lib project in `premake5.lua` with GLFW+OpenGL3 backends
- **Layer system**: Created `Layer` abstract class, `LayerStack` (push/pop layers + overlays), wired into `Application` game loop
- **ImGuiLayer**: Initializes ImGui with GLFW+OpenGL3 backends, manages new frame / render lifecycle, pushed as overlay
- **Input system**: Abstract `Input` class with static polling (`IsKeyPressed`, `GetMousePosition`). `WindowsInput` GLFW implementation
- **CameraController**: FPS-style — WASD movement, right-click + mouse drag for look, Space/Shift for up/down. Configurable speed/sensitivity
- **Sandbox reimplemented**: `SandboxLayer` with full ImGui panel for sphere properties (position, scale, rotation, color, ambient, specular, shininess, light position/color, background color, camera speed/sensitivity)
- **Bug fixes**: Added `<ostream>` include to `Event.h` (fixed 46 MSVC errors), linked ImGui to Sandbox project (fixed 10 LNK2019 errors)
- **Verified**: Full rebuild succeeds (0 errors), app runs with ImGui panel and camera controls

---

## Next Steps (Suggested Roadmap)

1. **Precompiled headers (PCH)** — Speed up compilation
2. **Textures** — Abstract Texture2D with OpenGL implementation
3. **Scene graph** — Entity/component system
4. **Multiple objects** — Scene with multiple meshes and transforms
5. **2D renderer** — Sprite batching, quad renderer
