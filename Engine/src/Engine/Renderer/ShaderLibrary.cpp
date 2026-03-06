#include "ShaderLibrary.h"
#include "Engine/Log.h"

namespace Engine {

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		if (Exists(name))
		{
			ENGINE_LOG_WARN("Shader '{0}' already exists in the library!", name);
			return;
		}
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
	{
		const auto& name = shader->GetName();
		Add(name, shader);
	}

	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		if (!Exists(name))
		{
			ENGINE_LOG_ERROR("Shader '{0}' not found in the library!", name);
			return nullptr;
		}
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}
