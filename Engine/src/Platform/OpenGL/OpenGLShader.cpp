#include "OpenGLShader.h"
#include "Engine/Log.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace Engine {

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
	}

	std::shared_ptr<Shader> Shader::CreateFromFile(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
	{
		auto readFile = [](const std::string& path) -> std::string {
			std::string absPath = std::filesystem::absolute(path).string();
			ENGINE_LOG_INFO("Loading shader file: {0}", absPath);
			std::ifstream file(path);
			if (!file.is_open())
			{
				ENGINE_LOG_ERROR("Failed to open shader file: {0} (absolute: {1})", path, absPath);
				ENGINE_LOG_ERROR("Current working directory: {0}", std::filesystem::current_path().string());
				return "";
			}
			std::stringstream ss;
			ss << file.rdbuf();
			return ss.str();
		};

		std::string vertexSrc = readFile(vertexPath);
		std::string fragmentSrc = readFile(fragmentPath);

		if (vertexSrc.empty() || fragmentSrc.empty())
		{
			ENGINE_LOG_ERROR("Shader creation failed for '{0}': could not read source files", name);
			return nullptr;
		}

		return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		// Vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const char* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "VERTEX");

		// Fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "FRAGMENT");

		// Program
		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glLinkProgram(m_RendererID);
		CheckCompileErrors(m_RendererID, "PROGRAM");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetBool(const std::string& name, bool value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value ? 1 : 0);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), value.x, value.y, value.z);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		glUniform4f(glGetUniformLocation(m_RendererID, name.c_str()), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::CheckCompileErrors(uint32_t shader, const std::string& type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				ENGINE_LOG_ERROR("Shader compilation error ({0}): {1}", type, infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				ENGINE_LOG_ERROR("Shader linking error: {0}", infoLog);
			}
		}
	}
}
