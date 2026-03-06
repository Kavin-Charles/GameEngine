#pragma once

#include "Engine/Core.h"
#include "Engine/Layer.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/CameraController.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Transform.h"
#include "Engine/Scene/SceneHierarchyPanel.h"
#include "Engine/InputSystem.h"

#include <memory>
#include <glm/glm.hpp>

namespace Engine {

	class ENGINE_API EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;

		// Set the active scene and shader for editing
		void SetScene(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Shader>& shader);

	private:
		Renderer* m_Renderer = nullptr;
		InputSystem* m_InputSystem = nullptr;

		// Editor camera (NOT an entity)
		Camera m_EditorCamera;
		Transform m_EditorTransform;
		CameraController m_CameraController;

		// Scene + panels
		std::shared_ptr<Scene> m_Scene;
		SceneHierarchyPanel m_SceneHierarchyPanel;
		std::shared_ptr<Shader> m_Shader;

		// Dual framebuffers
		std::shared_ptr<Framebuffer> m_SceneFramebuffer;
		std::shared_ptr<Framebuffer> m_GameFramebuffer;

		glm::vec2 m_SceneViewSize = { 1280.0f, 720.0f };
		glm::vec2 m_GameViewSize = { 1280.0f, 720.0f };
	};

}
