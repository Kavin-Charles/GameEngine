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
│   │   ├── Application.h/.cpp      # Owns Window, Renderer, InputSystem, EngineContext; Close()
│   │   ├── EditorLayer.h/.cpp      # Full editor UI layer (framebuffers, panels, gizmos, picking)
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
│   │   │   ├── Renderer.h/.cpp     # Instance renderer + SetSelectedEntity()
│   │   │   ├── Buffer.h            # VertexBuffer/IndexBuffer
│   │   │   ├── VertexArray.h       # VertexArray
│   │   │   ├── Shader.h            # Abstract shader (Create + CreateFromFile)
│   │   │   ├── ShaderLibrary.h/.cpp# Named shader cache
│   │   │   ├── Texture.h           # Texture base + Texture2D (Create from file or w/h)
│   │   │   ├── Material.h          # PBR-ready material struct
│   │   │   ├── Camera.h/.cpp       # Projection + FOV, view from Transform
│   │   │   ├── CameraController.h/.cpp # WASD + right-click mouse look
│   │   │   ├── Mesh.h/.cpp         # CreateSphere, CreateCube, CreatePlane (pos+normal+uv)
│   │   │   └── Framebuffer.h/.cpp  # Abstract framebuffer + ReadPixel + ClearAttachment
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
│       │   └── OpenGLFramebuffer    # Dual attachment: RGBA8 + R32I (entity ID)
│       └── Windows/
build.bat                           # One-click build: premake → MSBuild → asset copy
Sandbox/
├── src/
│   └── Sandbox.cpp               # ~65 lines: scene setup + EditorLayer push
└── assets/
    ├── shaders/
    │   ├── phong.vert             # Vertex shader (pos+normal+uv, MVP)
    │   └── phong.frag             # Fragment shader (Phong + entity ID output + highlight)
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

### EditorLayer (Engine-owned)
All editor UI lives in `Engine::EditorLayer`, NOT in Sandbox:
- Dual framebuffers (scene + game), each with RGBA8 color + R32I entity ID attachments
- Editor camera + CameraController (not an entity)
- Main menu bar (File, Build)
- Panel layout (Scene Hierarchy, Properties, Scene View, Game View)
- Gizmo handling via ImGuizmo
- **Mouse picking**: click in Scene View → `ReadPixel(1, x, y)` → entity ID → `SetSelectedEntity()`
- **Selection highlighting**: Renderer sets `u_Selected=true` for selected entity → shader adds additive glow

### Dual-View Rendering
- **Scene View** → Editor camera (owned by EditorLayer)
- **Game View** → Game Camera (Entity with `IsGameCamera=true`)
- Two separate framebuffers, two render passes
- Each view resizes independently

### Camera System
- `Camera` class: projection data only, view matrix computed from Transform
- `CameraComponent`: wraps Camera + `Primary` + `IsGameCamera` flags
- `CameraController`: always-on WASD + mouse look, modifies Transform directly
- Editor camera: owned by EditorLayer, not in scene
- Game camera: Entity in scene, `IsGameCamera=true`

### Entity Picking & Highlighting
- Framebuffer has dual color attachments: `GL_COLOR_ATTACHMENT0` (RGBA8 color) + `GL_COLOR_ATTACHMENT1` (R32I entity ID)
- Fragment shader outputs `o_EntityID = u_EntityID` to attachment 1
- `ReadPixel(1, x, y)` reads entity ID at mouse click position
- `ClearAttachment(1, -1)` resets ID buffer each frame
- Selected entity gets `u_Selected=true` → additive highlight in shader

### Rendering Flow
```
EditorLayer::OnUpdate():
  1. CameraController.OnUpdate(dt, input, editorTransform)
  2. SceneFramebuffer.Bind()
     → ClearAttachment(1, -1)  // reset entity IDs
     → SetSelectedEntity(selectedID)
     → Renderer.RenderSceneWithCamera(...)  // sets u_EntityID + u_Selected per entity
     SceneFramebuffer.Unbind()
  3. GameFramebuffer.Bind()
     → Renderer.RenderSceneWithCamera(...)
     GameFramebuffer.Unbind()
EditorLayer::OnImGuiRender():
  Scene View panel → SceneFramebuffer texture + mouse click → ReadPixel → select
  Game View panel → GameFramebuffer texture
```

### Instance-Based Systems
- `Renderer` is instance-based (no static state)
- `InputSystem` queries GLFW via Window reference
- `EngineContext` aggregates system pointers
- `RenderCommand` stays static (low-level GPU facade)

### Build Notes
- `build.bat` in project root: runs premake → MSBuild → asset copy (run with `.\build.bat`)
- ImGuizmo linked with `/WHOLEARCHIVE:ImGuizmo`
- `GLM_ENABLE_EXPERIMENTAL` for glm/gtx headers
- OneDrive may cause sharing violations — kill Sandbox.exe before rebuilding
- New Engine files require `premake5 vs2022` regeneration before MSBuild

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
- Lighting system (directional, point, spot lights)
- Shadow mapping
- Physics system integration
- Audio system
- Outline-based selection highlight (replace additive glow)
