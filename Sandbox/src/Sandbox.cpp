#include "Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/SceneHierarchyPanel.h"
#include "Engine/Renderer/Framebuffer.h"

#include <imgui.h>
#include <ImGuizmo.h> // Relies on include path now, should work at global scope if paths correct
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shaders...
static const std::string vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 v_FragPos;
out vec3 v_Normal;

void main()
{
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
}
)";

static const std::string fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

in vec3 v_FragPos;
in vec3 v_Normal;

uniform vec3 u_ObjectColor;

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(norm, lightDir), 0.2);
    vec3 result = diff * u_ObjectColor;
    FragColor = vec4(result, 1.0);
}
)";

class SandboxLayer : public Engine::Layer
{
public:
	SandboxLayer()
		: Layer("SandboxLayer"),
		m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f)
	{
	}

	void OnAttach() override
	{
		m_Scene = std::make_shared<Engine::Scene>();

		// Framebuffer
		Engine::FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Engine::Framebuffer::Create(fbSpec);

		// Create scene content
		std::shared_ptr<Engine::Mesh> sphereMesh = Engine::Mesh::CreateSphere(1.0f);

		auto* greenSphere = m_Scene->CreateEntity("Green Sphere");
		greenSphere->GetTransform().Translation = { -1.5f, 0.0f, 0.0f };
		greenSphere->GetMesh().Mesh = sphereMesh;
		greenSphere->GetMesh().Color = { 0.2f, 0.8f, 0.2f, 1.0f };

		auto* redSphere = m_Scene->CreateEntity("Red Sphere");
		redSphere->GetTransform().Translation = { 1.5f, 0.0f, 0.0f };
		redSphere->GetMesh().Mesh = sphereMesh;
		redSphere->GetMesh().Color = { 0.8f, 0.2f, 0.2f, 1.0f };

		auto* child = m_Scene->CreateEntity("Child Sphere");
		child->GetTransform().Translation = { 0.0f, 1.5f, 0.0f };
		child->GetTransform().Scale = { 0.5f, 0.5f, 0.5f };
		child->GetMesh().Mesh = sphereMesh;
		child->GetMesh().Color = { 0.2f, 0.2f, 0.8f, 1.0f };
		greenSphere->AddChild(child);

		m_SceneHierarchyPanel.SetContext(m_Scene);
		m_Shader.reset(Engine::Shader::Create(vertexShaderSource, fragmentShaderSource));
	}

	void OnDetach() override
	{
	}

	void OnUpdate(float deltaTime) override
	{
		// Resize
		if (Engine::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.GetCamera().SetProjection(45.0f, m_ViewportSize.x / m_ViewportSize.y, 0.1f, 1000.0f);
		}

		// Update Camera (only if focused or right click held)
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(deltaTime);

		// Render
		m_Framebuffer->Bind();
		Engine::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::Renderer::Clear();

		auto& camera = m_CameraController.GetCamera();
		m_Scene->OnUpdateEditor(deltaTime, m_Shader, camera.GetViewMatrix(), camera.GetProjectionMatrix());
		
		m_Framebuffer->Unbind();
	}



	void OnImGuiRender() override
	{
		// Static Layout Calculation
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 workPos = viewport->Pos;
		ImVec2 workSize = viewport->Size;
		ImVec2 workCenter = viewport->GetCenter();

		// Configuration
		float leftPanelWidth = workSize.x * 0.2f;
		float rightPanelWidth = workSize.x * 0.2f;
		float bottomPanelHeight = workSize.y * 0.3f;
		float centerPanelWidth = workSize.x - leftPanelWidth - rightPanelWidth;
		float centerPanelHeight = workSize.y - bottomPanelHeight;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// 1. Scene Hierarchy (Left)
		ImGui::SetNextWindowPos(workPos);
		ImGui::SetNextWindowSize({ leftPanelWidth, workSize.y }); // Full height? Or stop at bottom? Let's do full height for sidebars
		ImGui::Begin("Scene Hierarchy", nullptr, window_flags);
		m_SceneHierarchyPanel.DrawHierarchy();
		ImGui::End();

		// 2. Properties (Right)
		ImGui::SetNextWindowPos({ workPos.x + workSize.x - rightPanelWidth, workPos.y });
		ImGui::SetNextWindowSize({ rightPanelWidth, workSize.y });
		ImGui::Begin("Properties", nullptr, window_flags);
		m_SceneHierarchyPanel.DrawProperties();
		ImGui::End();

		// 3. Viewport (Center Top)
		ImGui::SetNextWindowPos({ workPos.x + leftPanelWidth, workPos.y });
		ImGui::SetNextWindowSize({ centerPanelWidth, centerPanelHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", nullptr, window_flags);

		m_ViewportFocused = ImGui::IsWindowFocused() || ImGui::IsWindowHovered();
		
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)(uint64_t)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		// Gizmos
		Engine::Entity* selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_SceneHierarchyPanel.GetGizmoType() != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera
			const auto& camera = m_CameraController.GetCamera();
			const glm::mat4& cameraProjection = camera.GetProjectionMatrix();
			glm::mat4 cameraView = camera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity->GetTransform();
			glm::mat4 transform = tc.GetTransform();

			// Snap
			bool snap = ImGui::GetIO().KeyCtrl;
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
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

				tc.Translation = translation;
				tc.Rotation = rotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		// 4. Content Browser (Center Bottom)
		ImGui::SetNextWindowPos({ workPos.x + leftPanelWidth, workPos.y + centerPanelHeight });
		ImGui::SetNextWindowSize({ centerPanelWidth, bottomPanelHeight });
		ImGui::Begin("Content Browser", nullptr, window_flags);
		ImGui::Text("Assets / Scenes");
		ImGui::Separator();
		
		// Dummy content
		float padding = 16.0f;
		float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (int i = 0; i < 10; i++)
		{
			ImGui::Button("File", { thumbnailSize, thumbnailSize });
			ImGui::Text("Asset %d", i);
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::End();
	}

private:
	Engine::CameraController m_CameraController;
	std::shared_ptr<Engine::Scene> m_Scene;
	Engine::SceneHierarchyPanel m_SceneHierarchyPanel;
	std::shared_ptr<Engine::Shader> m_Shader;
	std::shared_ptr<Engine::Framebuffer> m_Framebuffer;
	glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
	bool m_ViewportFocused = false;
};

class Sandbox : public Engine::Application {
public:
	Sandbox()
	{
		PushLayer(new SandboxLayer());
	}
};

Engine::Application* Engine::createApplication() {
	return new Sandbox();
}
