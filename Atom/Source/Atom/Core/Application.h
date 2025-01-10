#pragma once

namespace Atom
{

	class Application
	{
	public:
		Application();
		~Application();
	public:
		void Run();
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);

}