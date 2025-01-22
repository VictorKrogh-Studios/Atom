#pragma once

namespace Atom::Enumerations
{

	enum class CommandBufferUsageFlags
	{
		None = 0,
		ONE_TIME_SUBMIT_BIT = 0x00000001,
		RENDER_PASS_CONTINUE_BIT = 0x00000002,
		SIMULTANEOUS_USE_BIT = 0x00000004,
	};

}
