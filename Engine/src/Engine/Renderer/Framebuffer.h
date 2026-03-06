#pragma once

#include "Engine/Core.h"

namespace Engine {

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		// FramebufferFormat Format = ... 
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class ENGINE_API Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& spec);
	};

}
