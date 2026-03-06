#pragma once

#include "Transform.h"
#include "CameraComponent.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>

namespace Engine {

	// --- TagComponent: name string ---
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
		TagComponent(const TagComponent&) = default;
	};

	// --- TransformComponent: wraps Transform struct ---
	struct TransformComponent
	{
		Transform TransformData;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position) { TransformData.Position = position; }

		glm::mat4 GetMatrix() const { return TransformData.GetMatrix(); }
	};

	// --- MeshRendererComponent: mesh + material ---
	struct MeshRendererComponent
	{
		std::shared_ptr<Mesh> MeshAsset;
		Material MaterialData;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const std::shared_ptr<Mesh>& mesh)
			: MeshAsset(mesh) {}
		MeshRendererComponent(const std::shared_ptr<Mesh>& mesh, const glm::vec4& color)
			: MeshAsset(mesh) { MaterialData.Color = color; }
	};

	// CameraComponent is already defined in CameraComponent.h
	// (Camera CameraData + bool Primary + bool IsGameCamera)
}
