#include <Atom.h>

#include <iostream>

class SandboxApplication : public Atom::Application
{
};

extern Atom::Application* Atom::CreateApplication(int argc, char** argv)
{
	return new SandboxApplication();
}

int main(int argc, char** argv)
{
	Atom::Application* app = Atom::CreateApplication(argc, argv);

	AT_TRACE("TRACE");
	AT_INFO("INFO");
	AT_WARN("WARNING");
	AT_ERROR("ERROR");
	AT_CRITICAL("CRITICAL");

	app->Run();
	delete app;
}