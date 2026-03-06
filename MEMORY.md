# GameEngine - Memory File for AI

## Project Overview
A modular C++ game engine built as a DLL (`Engine.dll`) with a `Sandbox.exe` client application. Follows a Cherno-style architecture with instance-based systems, ECS-driven scene model, and dual-view rendering.

## Architecture
- **Engine** → Shared Library (`Engine.dll`) — all editor features, rendering, ECS, picking, highlighting
- **Sandbox** → Console Application (`Sandbox.exe`) — **user game code and assets ONLY**
- **Rule**: Engine features (editor UI, gizmos, selection, framebuffers, menus) must NEVER live in Sandbox. Sandbox should only contain scene setup, user shaders, and game logic.
- **Build System** → Premake5 generating VS2022 solutions
- **C++ Standard** → C++17, x64 only

## Directory Structure
```
Engine/
├── src/
│   ├── Engine.h                    # Master include header
│   ├── Engine/
│   │   ├── Core.h                  # DLL macros, Ref/Scope aliases, Asserts
│   │   ├── Application.h/.cpp      # Owns Window, Renderer, InputSystem, EngineContext
│   │   ├── EngineContext.h          # Aggregate: Renderer*, InputSystem*, Window*
│   │   ├── EntryPoint.h            # Main() definition
│   │   ├── Layer.h                 # Abstract Layer (OnUpdate takes Timestep)
│   │   ├── LayerStack.h/.cpp       # Layer management
│   │   ├── Log.h/.cpp              # spdlog-based logging
│   │   ├── Input.h                 # Static input polling (legacy)
│   │   ├── InputSystem.h/.cpp      # Instance-based input (queries Window)
│   │   ├── Window.h                # Abstract window interface
│   │   ├── Timestep.h              # Delta time wrapper
│   │   ├── UUID.h                  # 64-bit UUID generator
│   │   ├── Events/                 # Event system
│   │   ├── ImGui/
│   │   │   └── ImGuiLayer.h/.cpp   # ImGui integration layer
│   │   ├── Renderer/
│   │   │   ├── RendererAPI.h/.cpp  # Abstract GPU API interface
│   │   │   ├── RenderCommand.h/.cpp# Static facade for low-level GPU commands
│   │   │   ├── Renderer.h/.cpp     # Instance renderer: RenderScene + RenderSceneWithCamera
│   │   │   ├── Buffer.h            # VertexBuffer/IndexBuffer
│   │   │   ├── VertexArray.h       # VertexArray
│   │   │   ├── Shader.h            # Abstract shader (Create + CreateFromFile)
│   │   │   ├── ShaderLibrary.h/.cpp# Named shader cache
│   │   │   ├── Texture.h           # Texture base + Texture2D (Create from file or w/h)
│   │   │   ├── Material.h          # PBR-ready material struct
│   │   │   ├── Camera.h/.cpp       # Projection + FOV, view from Transform
│   │   │   ├── CameraController.h/.cpp # WASD + right-click mouse look
│   │   │   ├── Mesh.h/.cpp         # Mesh with sphere generation (pos+normal+uv)
│   │   │   └── Framebuffer.h/.cpp  # Abstract framebuffer
│   │   ├── Scene/
│   │   │   ├── Transform.h         # Position/Rotation/Scale + GetMatrix()
│   │   │   ├── Entity.h/.inl       # Lightweight ECS handle (uint32_t + Scene*)
│   │   │   ├── Registry.h          # Type-erased ComponentPool storage
│   │   │   ├── Components.h        # TagComponent, TransformComponent, MeshRendererComponent
│   │   │   ├── CameraComponent.h   # Camera + Primary + IsGameCamera flags
│   │   │   ├── Scene.h/.cpp        # ECS Registry owner, FindGameCameraID
│   │   │   └── SceneHierarchyPanel.h/.cpp # ECS-based hierarchy and properties UI
│   │   └── Asset/
│   │       └── AssetManager.h/.cpp
│   └── Platform/
│       ├── OpenGL/
│       └── Windows/
Sandbox/
├── src/
│   └── Sandbox.cpp               # Dual-view: Scene View + Game View
└── assets/
    ├── shaders/
    │   ├── phong.vert             # Vertex shader (pos+normal+uv, MVP)
    │   └── phong.frag             # Fragment shader (Phong + albedo texture)
    ├── textures/
    │   └── checkerboard.png
    ├── models/                    # Future mesh files
    └── scenes/                    # Future scene files
```

## Entity-Component System (ECS)
Custom lightweight ECS implementation (no external library):
- **Registry** (`Registry.h`): Type-erased `ComponentPool` storage, manages entity creation/destruction and component add/get/has/remove
- **Entity** (`Entity.h/.inl`): Lightweight handle (`uint32_t` ID + `Scene*`), forwards AddComponent/GetComponent/HasComponent/RemoveComponent to Registry
- **Components** (`Components.h`):
  - `TagComponent` — Name string
  - `TransformComponent` — Wraps `Transform` (Position/Rotation/Scale + GetMatrix())
  - `MeshRendererComponent` — `shared_ptr<Mesh>` + `Material` (Color)
- **CameraComponent** (`CameraComponent.h`): Camera + `Primary` + `IsGameCamera` flags
- **Scene** (`Scene.h/.cpp`): Owns the Registry, provides `CreateEntity()`, `FindGameCameraID()`, iterates entities for rendering

## Asset System
- **Asset source**: `Sandbox/assets/` — the canonical location for all assets
- **Postbuild copy**: Premake copies `Sandbox/assets/` → `bin/.../Sandbox/assets/` on every build
- **Asset paths** use simple relative format: `"assets/textures/..."`, `"assets/shaders/..."`
- **Shader loading**: `Shader::CreateFromFile(name, vertPath, fragPath)` reads GLSL from disk via `std::ifstream`
- **Texture loading**: `Texture2D::Create(path)` loads via stb_image; `Texture2D::Create(w, h)` for runtime data
- **stb_image** vendored at `Engine/vendor/stb/stb_image.h` with implementation in `stb_image_impl.cpp`

## Key Design Decisions

### Dual-View Rendering
- **Scene View** → Editor camera (NOT an Entity, owned by SandboxLayer)
- **Game View** → Game Camera (Entity with `IsGameCamera=true`, hidden from hierarchy)
- Two separate framebuffers, two render passes
- Each view resizes independently

### Camera System
- `Camera` class: projection data only, view matrix computed from Transform
- `CameraComponent`: wraps Camera + `Primary` + `IsGameCamera` flags
- `CameraController`: always-on WASD + mouse look, modifies Transform directly
- Editor camera: owned by client layer, not in scene
- Game camera: Entity in scene, `IsGameCamera=true`

### Rendering Flow
```
OnUpdate():
  1. CameraController.OnUpdate(dt, input, editorTransform)
  2. SceneFramebuffer.Bind()
     → Renderer.RenderSceneWithCamera(scene, shader, editorCamera, editorTransform)
     SceneFramebuffer.Unbind()
  3. GameFramebuffer.Bind()
     → Renderer.RenderSceneWithCamera(scene, shader, gameCam, gameCamTransform)
     GameFramebuffer.Unbind()
OnImGuiRender():
  Scene View panel → SceneFramebuffer texture
  Game View panel → GameFramebuffer texture
```

### Instance-Based Systems
- `Renderer` is instance-based (no static state)
- `InputSystem` queries GLFW via Window reference
- `EngineContext` aggregates system pointers
- `RenderCommand` stays static (low-level GPU facade)

### Build Notes
- ImGuizmo linked with `/WHOLEARCHIVE:ImGuizmo`
- `GLM_ENABLE_EXPERIMENTAL` for glm/gtx headers
- OneDrive may cause sharing violations — kill Sandbox.exe before rebuilding

## Dependencies
| Dependency | Location | Purpose |
|-----------|----------|---------|
| spdlog | Engine/vendor/spdlog | Logging |
| GLFW | Engine/vendor/glfw | Window/Input |
| GLAD | Engine/vendor/glad | OpenGL loading |
| GLM | Engine/vendor/glm | Math |
| ImGui | Engine/vendor/imgui | Debug UI |
| ImGuizmo | Engine/vendor/ImGuizmo | Transform gizmos |
| stb_image | Engine/vendor/stb | Image loading (textures) |

## Future TODOs

### Build Pipeline (in order)
1. Scene serialization (JSON/YAML) — save/load entities + components
2. Standalone Game Runtime project — separate exe without editor UI
3. Asset packaging — bundle assets into build output
4. Wire Build button — trigger MSBuild + copy Engine.dll + assets

### Engine Features
- Parent-child hierarchy for Entities
- More mesh primitives (cube, plane, etc.)
- Lighting system (directional, point, spot lights)
- Shadow mapping
- Physics system integration
- Audio system
