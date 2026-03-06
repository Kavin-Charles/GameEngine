#pragma once

#include "Engine/Core.h"
#include "VertexArray.h"
#include "Buffer.h"

#include <memory>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Engine {

	class ENGINE_API Mesh
	{
	public:
		Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const BufferLayout& layout);
		~Mesh() = default;

		const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }

		// Factory: Creates a UV sphere
		static std::shared_ptr<Mesh> CreateSphere(float radius = 1.0f, uint32_t sectorCount = 36, uint32_t stackCount = 18);

		// Factory: Creates a cube with proper per-face normals
		static std::shared_ptr<Mesh> CreateCube(float size = 1.0f);

		// Factory: Creates a flat XZ plane with Y-up normal
		static std::shared_ptr<Mesh> CreatePlane(float size = 1.0f);

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
	};
}
