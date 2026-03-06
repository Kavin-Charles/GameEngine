#include "AssetManager.h"
#include "Engine/Log.h"

namespace Engine {

	std::shared_ptr<Shader> AssetManager::LoadShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		auto shader = Shader::Create(name, vertexSrc, fragmentSrc);
		m_ShaderLibrary.Add(name, shader);
		return shader;
	}

	std::shared_ptr<Shader> AssetManager::GetShader(const std::string& name)
	{
		return m_ShaderLibrary.Get(name);
	}

	std::shared_ptr<Texture2D> AssetManager::LoadTexture(const std::string& name, const std::string& path)
	{
		auto texture = Texture2D::Create(path);
		m_Textures[name] = texture;
		return texture;
	}

	std::shared_ptr<Texture2D> AssetManager::GetTexture(const std::string& name)
	{
		auto it = m_Textures.find(name);
		if (it == m_Textures.end())
		{
			ENGINE_LOG_ERROR("Texture '{0}' not found!", name);
			return nullptr;
		}
		return it->second;
	}

	void AssetManager::RegisterMesh(const std::string& name, const std::shared_ptr<Mesh>& mesh)
	{
		m_Meshes[name] = mesh;
	}

	std::shared_ptr<Mesh> AssetManager::GetMesh(const std::string& name)
	{
		auto it = m_Meshes.find(name);
		if (it == m_Meshes.end())
		{
			ENGINE_LOG_ERROR("Mesh '{0}' not found!", name);
			return nullptr;
		}
		return it->second;
	}
}
