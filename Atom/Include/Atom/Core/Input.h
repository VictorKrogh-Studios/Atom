#pragma once

#include "Atom/Core/Enumerations/KeyCodes.h"

namespace Atom
{

	class Input
	{
	public:
		static bool IsKeyPressed(Enumerations::KeyCode keyCode);
	};

}
