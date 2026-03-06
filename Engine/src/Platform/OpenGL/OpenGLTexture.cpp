#include "OpenGLTexture.h"
#include "Engine/Log.h"
#include <glad/gl.h>
#include <stb_image.h>

namespace Engine {

	// --- Factory methods ---
	std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		return std::make_shared<OpenGLTexture2D>(width, height);
	}

	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
	{
		return std::make_shared<OpenGLTexture2D>(path);
	}

	// --- Blank texture (solid white by default) ---
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height),
		  m_InternalFormat(GL_RGBA8), m_DataFormat(GL_RGBA)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);
	}

	// --- File-based texture ---
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Width(0), m_Height(0), m_RendererID(0),
		  m_InternalFormat(0), m_DataFormat(0)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (data)
		{
			m_Width = width;
			m_Height = height;

			if (channels == 4)
			{
				m_InternalFormat = GL_RGBA8;
				m_DataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				m_InternalFormat = GL_RGB8;
				m_DataFormat = GL_RGB;
			}

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
		else
		{
			ENGINE_LOG_ERROR("Failed to load texture file: {0}", path);
			
			// Create a 1x1 magenta texture as a fallback indicator for missing textures
			m_Width = 1;
			m_Height = 1;
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;

			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_2D, m_RendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			uint32_t magentaData = 0xFFFF00FF; // A B G R = FF FF 00 FF -> Magenta
			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, &magentaData);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
}
