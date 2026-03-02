#include "Mesh.h"

namespace Engine {

	Mesh::Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const BufferLayout& layout)
	{
		m_VertexArray.reset(VertexArray::Create());

		std::shared_ptr<VertexBuffer> vbo;
		vbo.reset(VertexBuffer::Create(const_cast<float*>(vertices.data()), static_cast<uint32_t>(vertices.size() * sizeof(float))));
		vbo->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vbo);

		std::shared_ptr<IndexBuffer> ibo;
		ibo.reset(IndexBuffer::Create(const_cast<uint32_t*>(indices.data()), static_cast<uint32_t>(indices.size())));
		m_VertexArray->SetIndexBuffer(ibo);
	}

	std::shared_ptr<Mesh> Mesh::CreateSphere(float radius, uint32_t sectorCount, uint32_t stackCount)
	{
		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		float sectorStep = 2.0f * (float)M_PI / sectorCount;
		float stackStep = (float)M_PI / stackCount;

		// Generate vertices: position (3) + normal (3)
		for (uint32_t i = 0; i <= stackCount; ++i)
		{
			float stackAngle = (float)M_PI / 2.0f - i * stackStep;  // from pi/2 to -pi/2
			float xy = radius * cosf(stackAngle);
			float z = radius * sinf(stackAngle);

			for (uint32_t j = 0; j <= sectorCount; ++j)
			{
				float sectorAngle = j * sectorStep;  // from 0 to 2pi

				float x = xy * cosf(sectorAngle);
				float y = xy * sinf(sectorAngle);

				// Position
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);

				// Normal (normalized position for a unit sphere)
				float nx = x / radius;
				float ny = y / radius;
				float nz = z / radius;
				vertices.push_back(nx);
				vertices.push_back(ny);
				vertices.push_back(nz);
			}
		}

		// Generate indices
		for (uint32_t i = 0; i < stackCount; ++i)
		{
			uint32_t k1 = i * (sectorCount + 1);      // beginning of current stack
			uint32_t k2 = k1 + sectorCount + 1;       // beginning of next stack

			for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// Two triangles per sector (except first and last stacks)
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != (stackCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" }
		};

		return std::make_shared<Mesh>(vertices, indices, layout);
	}
}
