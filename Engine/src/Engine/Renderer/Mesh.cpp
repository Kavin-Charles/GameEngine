#include "Mesh.h"

namespace Engine {

	Mesh::Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const BufferLayout& layout)
	{
		m_VertexArray = VertexArray::Create();

		auto vbo = VertexBuffer::Create(const_cast<float*>(vertices.data()), static_cast<uint32_t>(vertices.size() * sizeof(float)));
		vbo->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vbo);

		auto ibo = IndexBuffer::Create(const_cast<uint32_t*>(indices.data()), static_cast<uint32_t>(indices.size()));
		m_VertexArray->SetIndexBuffer(ibo);
	}

	std::shared_ptr<Mesh> Mesh::CreateSphere(float radius, uint32_t sectorCount, uint32_t stackCount)
	{
		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		float sectorStep = 2.0f * (float)M_PI / sectorCount;
		float stackStep = (float)M_PI / stackCount;

		// Generate vertices: position (3) + normal (3) + texCoord (2)
		for (uint32_t i = 0; i <= stackCount; ++i)
		{
			float stackAngle = (float)M_PI / 2.0f - i * stackStep;
			float xy = radius * cosf(stackAngle);
			float z = radius * sinf(stackAngle);

			for (uint32_t j = 0; j <= sectorCount; ++j)
			{
				float sectorAngle = j * sectorStep;

				float x = xy * cosf(sectorAngle);
				float y = xy * sinf(sectorAngle);

				// Position
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);

				// Normal
				float nx = x / radius;
				float ny = y / radius;
				float nz = z / radius;
				vertices.push_back(nx);
				vertices.push_back(ny);
				vertices.push_back(nz);

				// TexCoord
				float u = (float)j / sectorCount;
				float v = (float)i / stackCount;
				vertices.push_back(u);
				vertices.push_back(v);
			}
		}

		// Generate indices
		for (uint32_t i = 0; i < stackCount; ++i)
		{
			uint32_t k1 = i * (sectorCount + 1);
			uint32_t k2 = k1 + sectorCount + 1;

			for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
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
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};

		return std::make_shared<Mesh>(vertices, indices, layout);
	}

	std::shared_ptr<Mesh> Mesh::CreateCube(float size)
	{
		float h = size * 0.5f;

		// 24 vertices: 4 per face, each with unique normal for flat shading
		// Format: pos(3) + normal(3) + uv(2)
		std::vector<float> vertices = {
			// Front face (Z+)
			-h, -h,  h,   0, 0, 1,   0, 0,
			 h, -h,  h,   0, 0, 1,   1, 0,
			 h,  h,  h,   0, 0, 1,   1, 1,
			-h,  h,  h,   0, 0, 1,   0, 1,
			// Back face (Z-)
			 h, -h, -h,   0, 0,-1,   0, 0,
			-h, -h, -h,   0, 0,-1,   1, 0,
			-h,  h, -h,   0, 0,-1,   1, 1,
			 h,  h, -h,   0, 0,-1,   0, 1,
			// Top face (Y+)
			-h,  h,  h,   0, 1, 0,   0, 0,
			 h,  h,  h,   0, 1, 0,   1, 0,
			 h,  h, -h,   0, 1, 0,   1, 1,
			-h,  h, -h,   0, 1, 0,   0, 1,
			// Bottom face (Y-)
			-h, -h, -h,   0,-1, 0,   0, 0,
			 h, -h, -h,   0,-1, 0,   1, 0,
			 h, -h,  h,   0,-1, 0,   1, 1,
			-h, -h,  h,   0,-1, 0,   0, 1,
			// Right face (X+)
			 h, -h,  h,   1, 0, 0,   0, 0,
			 h, -h, -h,   1, 0, 0,   1, 0,
			 h,  h, -h,   1, 0, 0,   1, 1,
			 h,  h,  h,   1, 0, 0,   0, 1,
			// Left face (X-)
			-h, -h, -h,  -1, 0, 0,   0, 0,
			-h, -h,  h,  -1, 0, 0,   1, 0,
			-h,  h,  h,  -1, 0, 0,   1, 1,
			-h,  h, -h,  -1, 0, 0,   0, 1,
		};

		std::vector<uint32_t> indices = {
			 0,  1,  2,   2,  3,  0,  // front
			 4,  5,  6,   6,  7,  4,  // back
			 8,  9, 10,  10, 11,  8,  // top
			12, 13, 14,  14, 15, 12,  // bottom
			16, 17, 18,  18, 19, 16,  // right
			20, 21, 22,  22, 23, 20,  // left
		};

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};

		return std::make_shared<Mesh>(vertices, indices, layout);
	}

	std::shared_ptr<Mesh> Mesh::CreatePlane(float size)
	{
		float h = size * 0.5f;

		// 4 vertices on XZ plane, Y-up normal
		std::vector<float> vertices = {
			// pos(3) + normal(3) + uv(2)
			-h, 0,  h,   0, 1, 0,   0, 0,
			 h, 0,  h,   0, 1, 0,   1, 0,
			 h, 0, -h,   0, 1, 0,   1, 1,
			-h, 0, -h,   0, 1, 0,   0, 1,
		};

		std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};

		return std::make_shared<Mesh>(vertices, indices, layout);
	}
}
