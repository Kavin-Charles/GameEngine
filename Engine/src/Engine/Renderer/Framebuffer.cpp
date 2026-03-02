#include "Framebuffer.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Engine {

	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		// Hardcoded for now, ideally check Renderer::GetAPI()
		return std::make_shared<OpenGLFramebuffer>(spec);
	}

}
