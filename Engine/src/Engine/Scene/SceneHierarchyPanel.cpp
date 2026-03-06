#include "SceneHierarchyPanel.h"
#include "Components.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& context)
	{
		m_Context = context;
		m_Selection = {};
	}

	void SceneHierarchyPanel::DrawHierarchy()
	{
		if (!m_Context) return;

		auto& reg = m_Context->GetRegistry();
		for (uint32_t id : reg.GetEntities())
		{
			// Hide game camera from hierarchy
			if (reg.HasComponent<CameraComponent>(id))
			{
				auto& cam = reg.GetComponent<CameraComponent>(id);
				if (cam.IsGameCamera) continue;
			}

			std::string name = "Entity";
			if (reg.HasComponent<TagComponent>(id))
				name = reg.GetComponent<TagComponent>(id).Tag;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (m_Selection.GetID() == id)
				flags |= ImGuiTreeNodeFlags_Selected;

			bool opened = ImGui::TreeNodeEx((void*)(intptr_t)id, flags, name.c_str());
			if (ImGui::IsItemClicked())
			{
				m_Selection = Entity(id, m_Context.get());
			}

			if (opened)
				ImGui::TreePop();
		}

		// Right-click context menu
		if (ImGui::BeginPopupContextWindow(0, 1))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize)) values.x = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawProperties()
	{
		if (!m_Selection) return;

		auto& reg = m_Context->GetRegistry();
		uint32_t id = m_Selection.GetID();

		if (!reg.IsValid(id)) { m_Selection = {}; return; }

		// Name
		if (reg.HasComponent<TagComponent>(id))
		{
			auto& tag = reg.GetComponent<TagComponent>(id);
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.Tag.c_str());
			if (ImGui::InputText("Name", buffer, sizeof(buffer)))
			{
				tag.Tag = std::string(buffer);
			}
		}

		// Transform
		if (reg.HasComponent<TransformComponent>(id))
		{
			auto& tc = reg.GetComponent<TransformComponent>(id);
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				DrawVec3Control("Position", tc.TransformData.Position);
				DrawVec3Control("Rotation", tc.TransformData.Rotation);
				DrawVec3Control("Scale", tc.TransformData.Scale, 1.0f);
			}
		}

		// MeshRenderer
		if (reg.HasComponent<MeshRendererComponent>(id))
		{
			auto& mr = reg.GetComponent<MeshRendererComponent>(id);
			if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(mr.MaterialData.Color));
				ImGui::DragFloat("Metallic", &mr.MaterialData.Metallic, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Roughness", &mr.MaterialData.Roughness, 0.01f, 0.0f, 1.0f);

				ImGui::Separator();
				ImGui::Text("Albedo Texture");

				if (mr.MaterialData.AlbedoTexture)
				{
					uint32_t texID = mr.MaterialData.AlbedoTexture->GetRendererID();
					ImGui::Image((void*)(uint64_t)texID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::SameLine();
					if (ImGui::Button("Remove"))
					{
						mr.MaterialData.AlbedoTexture = nullptr;
					}
				}
				else
				{
					ImGui::Button("None", ImVec2(64, 64));
				}

				static char texturePath[256] = "assets/textures/checkerboard.png";
				ImGui::InputText("##TexturePath", texturePath, sizeof(texturePath));
				ImGui::SameLine();
				if (ImGui::Button("Load"))
				{
					if (strlen(texturePath) > 0)
					{
						mr.MaterialData.AlbedoTexture = Texture2D::Create(texturePath);
					}
				}
			}
		}

		// Camera
		if (reg.HasComponent<CameraComponent>(id))
		{
			auto& cam = reg.GetComponent<CameraComponent>(id);
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Primary", &cam.Primary);
				float fov = cam.CameraData.GetFOV();
				if (ImGui::DragFloat("FOV", &fov, 0.1f, 1.0f, 179.0f))
					cam.CameraData.SetFOV(fov);
			}
		}

		// Gizmo mode
		ImGui::Separator();
		ImGui::Text("Gizmo Mode");
		if (ImGui::RadioButton("None", m_GizmoType == -1)) m_GizmoType = -1;
		ImGui::SameLine();
		if (ImGui::RadioButton("Translate", m_GizmoType == ImGuizmo::TRANSLATE)) m_GizmoType = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", m_GizmoType == ImGuizmo::ROTATE)) m_GizmoType = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", m_GizmoType == ImGuizmo::SCALE)) m_GizmoType = ImGuizmo::SCALE;
	}
}
