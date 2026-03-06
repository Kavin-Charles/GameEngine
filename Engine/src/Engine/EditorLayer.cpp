#include "EditorLayer.h"
#include "Application.h"
#include "Log.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		auto& app = Application::Get();
		m_Renderer = &app.GetRenderer();
		m_InputSystem = &app.GetInput();

		// Framebuffers
		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_SceneFramebuffer = Framebuffer::Create(fbSpec);
		m_GameFramebuffer = Framebuffer::Create(fbSpec);

		// Editor Camera
		m_EditorCamera.SetPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
		m_EditorTransform.Position = { 0.0f, 1.0f, 4.0f };
		m_EditorTransform.Rotation = { -10.0f, -90.0f, 0.0f };
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::SetScene(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Shader>& shader)
	{
		m_Scene = scene;
		m_Shader = shader;
		m_SceneHierarchyPanel.SetContext(m_Scene);
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		if (!m_Scene || !m_Shader) return;

		// --- Scene View resize ---
		if (m_SceneViewSize.x > 0.0f && m_SceneViewSize.y > 0.0f)
		{
			auto spec = m_SceneFramebuffer->GetSpecification();
			if (spec.Width != (uint32_t)m_SceneViewSize.x || spec.Height != (uint32_t)m_SceneViewSize.y)
			{
				m_SceneFramebuffer->Resize((uint32_t)m_SceneViewSize.x, (uint32_t)m_SceneViewSize.y);
				m_EditorCamera.SetViewportSize((uint32_t)m_SceneViewSize.x, (uint32_t)m_SceneViewSize.y);
			}
		}

		// --- Game View resize ---
		if (m_GameViewSize.x > 0.0f && m_GameViewSize.y > 0.0f)
		{
			auto spec = m_GameFramebuffer->GetSpecification();
			if (spec.Width != (uint32_t)m_GameViewSize.x || spec.Height != (uint32_t)m_GameViewSize.y)
			{
				m_GameFramebuffer->Resize((uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
				m_Scene->OnGameViewResize((uint32_t)m_GameViewSize.x, (uint32_t)m_GameViewSize.y);
			}
		}

		// --- Update editor camera ---
		m_CameraController.OnUpdate(ts, *m_InputSystem, m_EditorTransform);

		// --- Scene logic ---
		m_Scene->OnUpdate(ts);

		// --- Pass 1: Scene View (editor camera) ---
		m_SceneFramebuffer->Bind();
		m_Renderer->BeginFrame({ 0.1f, 0.1f, 0.1f, 1.0f });

		// Clear entity ID attachment to -1
		m_SceneFramebuffer->ClearAttachment(1, -1);
		
		uint32_t selectedID = m_SceneHierarchyPanel.GetSelectedEntity() ? (uint32_t)m_SceneHierarchyPanel.GetSelectedEntity() : 0;
		m_Renderer->SetSelectedEntity(selectedID);

		m_Renderer->RenderSceneWithCamera(*m_Scene, *m_Shader, m_EditorCamera, m_EditorTransform);
		m_Renderer->EndFrame();
		m_SceneFramebuffer->Unbind();

		// --- Pass 2: Game View ---
		uint32_t gameCamID = m_Scene->FindGameCameraID();
		if (gameCamID != 0)
		{
			auto& reg = m_Scene->GetRegistry();
			auto& cam = reg.GetComponent<CameraComponent>(gameCamID);
			auto& transform = reg.GetComponent<TransformComponent>(gameCamID);

			m_GameFramebuffer->Bind();
			m_Renderer->BeginFrame({ 0.05f, 0.05f, 0.05f, 1.0f });
			m_Renderer->RenderSceneWithCamera(*m_Scene, *m_Shader,
				cam.CameraData, transform.TransformData);
			m_Renderer->EndFrame();
			m_GameFramebuffer->Unbind();
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		// --- Main Menu Bar ---
		float menuBarHeight = 0.0f;
		if (ImGui::BeginMainMenuBar())
		{
			menuBarHeight = ImGui::GetWindowHeight();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				{
					ENGINE_LOG_INFO("New Scene");
				}
				if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
				{
					ENGINE_LOG_INFO("Open Scene");
				}
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				{
					ENGINE_LOG_INFO("Save Scene");
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Alt+F4"))
				{
					Application::Get().Close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Build"))
			{
				if (ImGui::MenuItem("Build Game", "Ctrl+B"))
				{
					ENGINE_LOG_INFO("Build Game triggered!");
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 workPos = { viewport->Pos.x, viewport->Pos.y + menuBarHeight };
		ImVec2 workSize = { viewport->Size.x, viewport->Size.y - menuBarHeight };

		float leftPanelWidth = workSize.x * 0.18f;
		float rightPanelWidth = workSize.x * 0.18f;
		float centerWidth = workSize.x - leftPanelWidth - rightPanelWidth;
		float sceneViewHeight = workSize.y * 0.55f;
		float gameViewHeight = workSize.y * 0.45f;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// 1. Scene Hierarchy (Left)
		ImGui::SetNextWindowPos(workPos);
		ImGui::SetNextWindowSize({ leftPanelWidth, workSize.y });
		ImGui::Begin("Scene Hierarchy", nullptr, window_flags);
		m_SceneHierarchyPanel.DrawHierarchy();
		ImGui::End();

		// 2. Properties (Right)
		ImGui::SetNextWindowPos({ workPos.x + workSize.x - rightPanelWidth, workPos.y });
		ImGui::SetNextWindowSize({ rightPanelWidth, workSize.y });
		ImGui::Begin("Properties", nullptr, window_flags);
		m_SceneHierarchyPanel.DrawProperties();
		ImGui::End();

		// 3. Scene View (Center Top)
		ImGui::SetNextWindowPos({ workPos.x + leftPanelWidth, workPos.y });
		ImGui::SetNextWindowSize({ centerWidth, sceneViewHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene View", nullptr, window_flags);
		{
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();

			glm::vec2 viewportBounds[2];
			viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			ImVec2 panelSize = ImGui::GetContentRegionAvail();
			m_SceneViewSize = { panelSize.x, panelSize.y };

			uint32_t texID = m_SceneFramebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)(uint64_t)texID, ImVec2{ m_SceneViewSize.x, m_SceneViewSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			// Editor Mouse Picking logic
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGuizmo::IsOver())
			{
				auto mousePos = ImGui::GetMousePos();
				mousePos.x -= viewportBounds[0].x;
				mousePos.y -= viewportBounds[0].y;
				glm::vec2 viewportSize = viewportBounds[1] - viewportBounds[0];

				// Flip Y because OpenGL's origin is bottom-left but ImGui's is top-left
				mousePos.y = viewportSize.y - mousePos.y;

				if (mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < (int)viewportSize.x && mousePos.y < (int)viewportSize.y)
				{
					m_SceneFramebuffer->Bind();
					int pixelData = m_SceneFramebuffer->ReadPixel(1, (int)mousePos.x, (int)mousePos.y);
					m_SceneFramebuffer->Unbind();

					if (pixelData != -1)
					{
						Entity e((uint32_t)pixelData, m_Scene.get());
						m_SceneHierarchyPanel.SetSelectedEntity(e);
					}
					else
					{
						m_SceneHierarchyPanel.SetSelectedEntity({});
					}
				}
			}

			// Gizmos in Scene View
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && m_SceneHierarchyPanel.GetGizmoType() != -1)
			{
				auto& reg = m_Scene->GetRegistry();
				if (reg.HasComponent<TransformComponent>(selectedEntity.GetID()))
				{
					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();

					float windowWidth = (float)ImGui::GetWindowWidth();
					float windowHeight = (float)ImGui::GetWindowHeight();
					ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

					glm::mat4 cameraProjection = m_EditorCamera.GetProjectionMatrix();
					glm::mat4 cameraView = m_EditorCamera.GetViewMatrix(m_EditorTransform);

					auto& tc = reg.GetComponent<TransformComponent>(selectedEntity.GetID());
					glm::mat4 transform = tc.GetMatrix();

					bool snap = ImGui::GetIO().KeyCtrl;
					float snapValue = 0.5f;
					if (m_SceneHierarchyPanel.GetGizmoType() == ImGuizmo::OPERATION::ROTATE)
						snapValue = 45.0f;
					float snapValues[3] = { snapValue, snapValue, snapValue };

					ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
						(ImGuizmo::OPERATION)m_SceneHierarchyPanel.GetGizmoType(), ImGuizmo::LOCAL, glm::value_ptr(transform),
						nullptr, snap ? snapValues : nullptr);

					if (ImGuizmo::IsUsing())
					{
						glm::vec3 translation, rotation, scale;
						ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

						tc.TransformData.Position = translation;
						tc.TransformData.Rotation = rotation;
						tc.TransformData.Scale = scale;
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();

		// 4. Game View (Center Bottom)
		ImGui::SetNextWindowPos({ workPos.x + leftPanelWidth, workPos.y + sceneViewHeight });
		ImGui::SetNextWindowSize({ centerWidth, gameViewHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Game View", nullptr, window_flags);
		{
			ImVec2 panelSize = ImGui::GetContentRegionAvail();
			m_GameViewSize = { panelSize.x, panelSize.y };

			uint32_t texID = m_GameFramebuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)(uint64_t)texID, ImVec2{ m_GameViewSize.x, m_GameViewSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

}
