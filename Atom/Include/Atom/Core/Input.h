#pragma once

#include "Atom/Core/Enumerations/KeyCodes.h"
#include "Atom/Core/Enumerations/MouseButtons.h"

#include <glm/vec2.hpp>

namespace Atom
{

	class Input
	{
	public:
		static bool IsKeyPressed(Enumerations::KeyCode keyCode);
		static bool IsKeyReleased(Enumerations::KeyCode keyCode);

		static bool IsMouseButtonPressed(Enumerations::MouseButton button);
		static bool IsMouseButtonReleased(Enumerations::MouseButton button);

		static glm::vec2 GetMousePosition();
	};

}
