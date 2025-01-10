#pragma once
#include <memory>

namespace Atom
{

	class Window
	{
	public:
		static Window* Create();
	public:
		virtual ~Window() = default;

		virtual void Update() = 0;
	};

}