#pragma once

#include "Engine/Core.h"
#include "Engine/Renderer/ShaderLibrary.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/Mesh.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace Engine {

	class ENGINE_API AssetManager
	{
	public:
		static AssetManager& Get()
		{
			static AssetManager s_Instance;
			return s_Instance;
		}

		// Shaders
		std::shared_ptr<Shader> LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		std::shared_ptr<Shader> GetShader(const std::string& name);

		// Textures
		std::shared_ptr<Texture2D> LoadTexture(const std::string& name, const std::string& path);
		std::shared_ptr<Texture2D> GetTexture(const std::string& name);

		// Meshes
		void RegisterMesh(const std::string& name, const std::shared_ptr<Mesh>& mesh);
		std::shared_ptr<Mesh> GetMesh(const std::string& name);

	private:
		AssetManager() = default;

		ShaderLibrary m_ShaderLibrary;
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures;
		std::unordered_map<std::string, std::shared_ptr<Mesh>> m_Meshes;
	};
}
