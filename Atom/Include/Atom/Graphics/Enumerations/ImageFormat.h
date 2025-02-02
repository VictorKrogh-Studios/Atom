#pragma once

namespace Atom::Enumerations
{

	enum class ImageFormat
	{
		Unknown = 0,
		B8G8R8A8_UNORM,

		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8,
	};

}
