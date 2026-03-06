#pragma once

#include "Scene.h"
#include "Entity.h"
#include <memory>
#include "Engine/Core.h"

namespace Engine {

	class ENGINE_API SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene>& context);

		void SetContext(const std::shared_ptr<Scene>& context);

		void DrawHierarchy();
		void DrawProperties();

		Entity GetSelectedEntity() const { return m_Selection; }
		void SetSelectedEntity(Entity entity) { m_Selection = entity; }

		int GetGizmoType() const { return m_GizmoType; }
		void SetGizmoType(int type) { m_GizmoType = type; }

	private:
		std::shared_ptr<Scene> m_Context;
		Entity m_Selection;
		int m_GizmoType = -1;
	};
}
