#pragma once

// Core
#include "Engine/Core.h"
#include "Engine/Application.h"
#include "Engine/Layer.h"
#include "Engine/Log.h"
#include "Engine/Timestep.h"
#include "Engine/Input.h"
#include "Engine/InputSystem.h"
#include "Engine/EngineContext.h"

// Events
#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

// ImGui
#include "Engine/ImGui/ImGuiLayer.h"

// Renderer
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/RendererAPI.h"
#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/ShaderLibrary.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/CameraController.h"
#include "Engine/Renderer/Mesh.h"

// Scene / ECS
#include "Engine/Scene/Transform.h"
#include "Engine/Scene/Registry.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.inl"

// Editor
#include "Engine/EditorLayer.h"

// Entry Point (included by client)
#include "Engine/EntryPoint.h"
