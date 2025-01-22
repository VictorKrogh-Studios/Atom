#pragma once

namespace Atom::Enumerations
{

	enum class MouseButton
	{
		MouseButton1 = 0,
		MouseButton2 = 1,
		MouseButton3 = 2,
		MouseButton4 = 3,
		MouseButton5 = 4,
		MouseButton6 = 5,
		MouseButton7 = 6,
		MouseButton8 = 7,

		MouseButtonLeft = MouseButton1,
		MouseButtonRight = MouseButton2,
		MouseButtonMiddle = MouseButton3,
	};

}

#define AT_MOUSE_BUTTON_1         ::Atom::Enumerations::MouseButton::MouseButton1
#define AT_MOUSE_BUTTON_2         ::Atom::Enumerations::MouseButton::MouseButton2
#define AT_MOUSE_BUTTON_3         ::Atom::Enumerations::MouseButton::MouseButton3
#define AT_MOUSE_BUTTON_4         ::Atom::Enumerations::MouseButton::MouseButton4
#define AT_MOUSE_BUTTON_5         ::Atom::Enumerations::MouseButton::MouseButton5
#define AT_MOUSE_BUTTON_6         ::Atom::Enumerations::MouseButton::MouseButton6
#define AT_MOUSE_BUTTON_7         ::Atom::Enumerations::MouseButton::MouseButton7
#define AT_MOUSE_BUTTON_8         ::Atom::Enumerations::MouseButton::MouseButton8
#define AT_MOUSE_BUTTON_LAST      ::Atom::Enumerations::MouseButton::MouseButton8
#define AT_MOUSE_BUTTON_LEFT      ::Atom::Enumerations::MouseButton::MouseButtonLeft
#define AT_MOUSE_BUTTON_RIGHT     ::Atom::Enumerations::MouseButton::MouseButtonRight
#define AT_MOUSE_BUTTON_MIDDLE    ::Atom::Enumerations::MouseButton::MouseButtonMiddle
