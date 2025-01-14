#pragma once

namespace Atom
{

	class SwapChain;

	struct GraphicsContextOptions
	{
		bool EnableValidation = true;

		uint32_t GlfwExtensionsCount = 0;		// Only used for Vulkan!
		const char** GlfwExtensions = nullptr;	// Only used for Vulkan!
	};

	class GraphicsContext
	{
	public:
		static GraphicsContext* Create(const GraphicsContextOptions& options);
	public:
		GraphicsContext(const GraphicsContextOptions& options);
		virtual ~GraphicsContext() = default;
	protected:
		GraphicsContextOptions m_Options;
	};

}