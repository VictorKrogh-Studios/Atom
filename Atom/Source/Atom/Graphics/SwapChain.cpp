#include "ATPCH.h"
#include "SwapChain.h"

#include "Vulkan/VulkanSwapChain.h"

namespace Atom
{

	SwapChain* SwapChain::Create(const SwapChainOptions& options)
	{
		return new VulkanSwapChain(options);
	}

	SwapChain::SwapChain(const SwapChainOptions& options)
		: m_Options(options)
	{
	}

}