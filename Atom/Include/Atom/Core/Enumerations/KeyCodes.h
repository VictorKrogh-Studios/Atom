#pragma once

namespace Atom::Enumerations
{

	enum class KeyCode
	{
		Space = 32,
		Apostrophe = 39,  /* ' */
		Comma = 44,  /* , */
		Minus = 45,  /* - */
		Period = 46,  /* . */
		Slash = 47,  /* / */
		Number0 = 48,
		Number1 = 49,
		Number2 = 50,
		Number3 = 51,
		Number4 = 52,
		Number5 = 53,
		Number6 = 54,
		Number7 = 55,
		Number8 = 56,
		Number9 = 57,
		Semicolon = 59,  /* ; */
		Equal = 61,  /* = */
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */
		World1 = 161, /* non-us #1 */
		World2 = 162, /* non-us #2 */

		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		Caps = CapsLock,
		ScrollLock = 281,
		ScrLk = ScrollLock,
		NumLock = 282,
		PrintScreen = 283,
		PrtSc = PrintScreen,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		KeyPad0 = 320,
		KeyPad1 = 321,
		KeyPad2 = 322,
		KeyPad3 = 323,
		KeyPad4 = 324,
		KeyPad5 = 325,
		KeyPad6 = 326,
		KeyPad7 = 327,
		KeyPad8 = 328,
		KeyPad9 = 329,
		KeyPadDecimal = 330,
		KeyPadDivide = 331,
		KeyPadMultiply = 332,
		KeyPadSubtract = 333,
		KeyPadAdd = 334,
		KeyPadEnter = 335,
		KeyPadEqual = 336,
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348,

		KeyLast = Menu
	};

}

#define AT_KEY_SPACE			::Atom::Enumerations::KeyCode::Space
#define AT_KEY_APOSTROPHE		::Atom::Enumerations::KeyCode::Apostrophe
#define AT_KEY_COMMA			::Atom::Enumerations::KeyCode::Comma
#define AT_KEY_MINUS			::Atom::Enumerations::KeyCode::Minus
#define AT_KEY_PERIOD			::Atom::Enumerations::KeyCode::Period
#define AT_KEY_SLASH			::Atom::Enumerations::KeyCode::Slash
#define AT_KEY_0				::Atom::Enumerations::KeyCode::Number0
#define AT_KEY_1				::Atom::Enumerations::KeyCode::Number1
#define AT_KEY_2				::Atom::Enumerations::KeyCode::Number2
#define AT_KEY_3				::Atom::Enumerations::KeyCode::Number3
#define AT_KEY_4				::Atom::Enumerations::KeyCode::Number4
#define AT_KEY_5				::Atom::Enumerations::KeyCode::Number5
#define AT_KEY_6				::Atom::Enumerations::KeyCode::Number6
#define AT_KEY_7				::Atom::Enumerations::KeyCode::Number7
#define AT_KEY_8				::Atom::Enumerations::KeyCode::Number8
#define AT_KEY_9				::Atom::Enumerations::KeyCode::Number9
#define AT_KEY_SEMICOLON		::Atom::Enumerations::KeyCode::Semicolon
#define AT_KEY_EQUAL			::Atom::Enumerations::KeyCode::Equal
#define AT_KEY_A				::Atom::Enumerations::KeyCode::A
#define AT_KEY_B				::Atom::Enumerations::KeyCode::B
#define AT_KEY_C				::Atom::Enumerations::KeyCode::C
#define AT_KEY_D				::Atom::Enumerations::KeyCode::D
#define AT_KEY_E				::Atom::Enumerations::KeyCode::E
#define AT_KEY_F				::Atom::Enumerations::KeyCode::F
#define AT_KEY_G				::Atom::Enumerations::KeyCode::G
#define AT_KEY_H				::Atom::Enumerations::KeyCode::H
#define AT_KEY_I				::Atom::Enumerations::KeyCode::I
#define AT_KEY_J				::Atom::Enumerations::KeyCode::J
#define AT_KEY_K				::Atom::Enumerations::KeyCode::K
#define AT_KEY_L				::Atom::Enumerations::KeyCode::L
#define AT_KEY_M				::Atom::Enumerations::KeyCode::M
#define AT_KEY_N				::Atom::Enumerations::KeyCode::N
#define AT_KEY_O				::Atom::Enumerations::KeyCode::O
#define AT_KEY_P				::Atom::Enumerations::KeyCode::P
#define AT_KEY_Q				::Atom::Enumerations::KeyCode::Q
#define AT_KEY_R				::Atom::Enumerations::KeyCode::R
#define AT_KEY_S				::Atom::Enumerations::KeyCode::S
#define AT_KEY_T				::Atom::Enumerations::KeyCode::T
#define AT_KEY_U				::Atom::Enumerations::KeyCode::U
#define AT_KEY_V				::Atom::Enumerations::KeyCode::V
#define AT_KEY_W				::Atom::Enumerations::KeyCode::W
#define AT_KEY_X				::Atom::Enumerations::KeyCode::X
#define AT_KEY_Y				::Atom::Enumerations::KeyCode::Y
#define AT_KEY_Z				::Atom::Enumerations::KeyCode::Z
#define AT_KEY_LEFT_BRACKET		::Atom::Enumerations::KeyCode::LeftBracket
#define AT_KEY_BACKSLASH		::Atom::Enumerations::KeyCode::Backslash
#define AT_KEY_RIGHT_BRACKET	::Atom::Enumerations::KeyCode::RightBracket
#define AT_KEY_GRAVE_ACCENT		::Atom::Enumerations::KeyCode::GraveAccent
#define AT_KEY_WORLD_1			::Atom::Enumerations::KeyCode::World1
#define AT_KEY_WORLD_2			::Atom::Enumerations::KeyCode::World2

#define AT_KEY_ESCAPE			::Atom::Enumerations::KeyCode::Escape
#define AT_KEY_ENTER			::Atom::Enumerations::KeyCode::Enter
#define AT_KEY_TAB				::Atom::Enumerations::KeyCode::Tab
#define AT_KEY_BACKSPACE		::Atom::Enumerations::KeyCode::Backspace
#define AT_KEY_INSERT			::Atom::Enumerations::KeyCode::Insert
#define AT_KEY_DELETE			::Atom::Enumerations::KeyCode::Delete
#define AT_KEY_RIGHT			::Atom::Enumerations::KeyCode::Right
#define AT_KEY_LEFT				::Atom::Enumerations::KeyCode::Left
#define AT_KEY_DOWN				::Atom::Enumerations::KeyCode::Down
#define AT_KEY_UP				::Atom::Enumerations::KeyCode::Up
#define AT_KEY_PAGEUP			::Atom::Enumerations::KeyCode::PageUp
#define AT_KEY_PAGEDOWN			::Atom::Enumerations::KeyCode::PageDown
#define AT_KEY_HOME				::Atom::Enumerations::KeyCode::Home
#define AT_KEY_END				::Atom::Enumerations::KeyCode::End
#define AT_KEY_CAPS_LOCK		::Atom::Enumerations::KeyCode::CapsLock
#define AT_KEY_SCROLL_LOCK		::Atom::Enumerations::KeyCode::ScrollLock
#define AT_KEY_NUM_LOCK			::Atom::Enumerations::KeyCode::NumLock
#define AT_KEY_PRINT_SCREEN		::Atom::Enumerations::KeyCode::PrintScreen
#define AT_KEY_PAUSE			::Atom::Enumerations::KeyCode::Pause
#define AT_KEY_F1				::Atom::Enumerations::KeyCode::F1
#define AT_KEY_F2				::Atom::Enumerations::KeyCode::F2
#define AT_KEY_F3				::Atom::Enumerations::KeyCode::F3
#define AT_KEY_F4				::Atom::Enumerations::KeyCode::F4
#define AT_KEY_F5				::Atom::Enumerations::KeyCode::F5
#define AT_KEY_F6				::Atom::Enumerations::KeyCode::F6
#define AT_KEY_F7				::Atom::Enumerations::KeyCode::F7
#define AT_KEY_F8				::Atom::Enumerations::KeyCode::F8
#define AT_KEY_F9				::Atom::Enumerations::KeyCode::F9
#define AT_KEY_F10				::Atom::Enumerations::KeyCode::F10
#define AT_KEY_F11				::Atom::Enumerations::KeyCode::F11
#define AT_KEY_F12				::Atom::Enumerations::KeyCode::F12
#define AT_KEY_F13				::Atom::Enumerations::KeyCode::F13
#define AT_KEY_F14				::Atom::Enumerations::KeyCode::F14
#define AT_KEY_F15				::Atom::Enumerations::KeyCode::F15
#define AT_KEY_F16				::Atom::Enumerations::KeyCode::F16
#define AT_KEY_F17				::Atom::Enumerations::KeyCode::F17
#define AT_KEY_F18				::Atom::Enumerations::KeyCode::F18
#define AT_KEY_F19				::Atom::Enumerations::KeyCode::F19
#define AT_KEY_F20				::Atom::Enumerations::KeyCode::F20
#define AT_KEY_F21				::Atom::Enumerations::KeyCode::F21
#define AT_KEY_F22				::Atom::Enumerations::KeyCode::F22
#define AT_KEY_F23				::Atom::Enumerations::KeyCode::F23
#define AT_KEY_F24				::Atom::Enumerations::KeyCode::F24
#define AT_KEY_F25				::Atom::Enumerations::KeyCode::F25
#define AT_KEY_KEYPAD_0			::Atom::Enumerations::KeyCode::KeyPad0
#define AT_KEY_KEYPAD_1			::Atom::Enumerations::KeyCode::KeyPad1
#define AT_KEY_KEYPAD_2			::Atom::Enumerations::KeyCode::KeyPad2
#define AT_KEY_KEYPAD_3			::Atom::Enumerations::KeyCode::KeyPad3
#define AT_KEY_KEYPAD_4			::Atom::Enumerations::KeyCode::KeyPad4
#define AT_KEY_KEYPAD_5			::Atom::Enumerations::KeyCode::KeyPad5
#define AT_KEY_KEYPAD_6			::Atom::Enumerations::KeyCode::KeyPad6
#define AT_KEY_KEYPAD_7			::Atom::Enumerations::KeyCode::KeyPad7
#define AT_KEY_KEYPAD_8			::Atom::Enumerations::KeyCode::KeyPad8
#define AT_KEY_KEYPAD_9			::Atom::Enumerations::KeyCode::KeyPad9
#define AT_KEY_KEYPAD_DECIMAL	::Atom::Enumerations::KeyCode::KeyPadDecimal
#define AT_KEY_KEYPAD_DIVIDE	::Atom::Enumerations::KeyCode::KeyPadDivide
#define AT_KEY_KEYPAD_MULTIPLy	::Atom::Enumerations::KeyCode::KeyPadMultiply
#define AT_KEY_KEYPAD_SUBTRACt	::Atom::Enumerations::KeyCode::KeyPadSubtract
#define AT_KEY_KEYPAD_ADD		::Atom::Enumerations::KeyCode::KeyPadAdd
#define AT_KEY_KEYPAD_ENTER		::Atom::Enumerations::KeyCode::KeyPadEnter
#define AT_KEY_KEYPAD_EQUAL		::Atom::Enumerations::KeyCode::KeyPadEqual
#define AT_KEY_LEFT_SHIFT		::Atom::Enumerations::KeyCode::LeftShift
#define AT_KEY_LEFT_CONTROL		::Atom::Enumerations::KeyCode::LeftControl
#define AT_KEY_LEFT_ALT			::Atom::Enumerations::KeyCode::LeftAlt
#define AT_KEY_LEFT_SUPER		::Atom::Enumerations::KeyCode::LeftSuper
#define AT_KEY_RIGHT_SHIFT		::Atom::Enumerations::KeyCode::RightShift
#define AT_KEY_RIGHT_CONTROL	::Atom::Enumerations::KeyCode::RightControl
#define AT_KEY_RIGHT_ALT		::Atom::Enumerations::KeyCode::RightAlt
#define AT_KEY_RIGHT_SUPER		::Atom::Enumerations::KeyCode::RightSuper
#define AT_KEY_MENU				::Atom::Enumerations::KeyCode::Menu

#define AT_KEY_KEYLAST			::Atom::Enumerations::KeyCode::KeyLast
