#pragma once

#include "Scene.h"
#include "Entity.h"
#include <memory>
#include <glm/glm.hpp>
#include "Engine/Core.h"

namespace Engine {

	class ENGINE_API SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene>& context);

		void SetContext(const std::shared_ptr<Scene>& context);

		void OnImGuiRender() { DrawHierarchy(); DrawProperties(); } // Backwards compatibility if needed, but we will call individually
		void DrawHierarchy();
		void DrawProperties();

		Entity* GetSelectedEntity() const { return m_Selection; }
		void SetSelectedEntity(Entity* entity) { m_Selection = entity; }

		int GetGizmoType() const { return m_GizmoType; }
		void SetGizmoType(int type) { m_GizmoType = type; }

	private:
		void DrawEntityNode(Entity* entity);
		void DrawComponents(Entity* entity);

		std::shared_ptr<Scene> m_Context;
		Entity* m_Selection = nullptr;
		
		// Gizmo settings
		int m_GizmoType = -1; // -1 = None, 0 = Translate, 1 = Rotate, 2 = Scale
	};

}
