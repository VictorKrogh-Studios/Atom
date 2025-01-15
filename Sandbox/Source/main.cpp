#include <Atom.h>

#include "Layers/SandboxLayer.h"

class SandboxApplication : public Atom::Application
{
public:
	SandboxApplication(const Atom::ApplicationCreateInfo& applicationCreateInfo) : Atom::Application(applicationCreateInfo)
	{
		PushLayer(new SandboxLayer());
	}
};

extern Atom::Application* Atom::CreateApplication(const Atom::CommandLineArgs& args)
{
	Atom::ApplicationCreateInfo applicationCreateInfo{};
	applicationCreateInfo.Args = args;

	return new SandboxApplication(applicationCreateInfo);
}

int main(int argc, char** argv)
{
	Atom::Application* app = Atom::CreateApplication({ argc, argv });

	AT_TRACE("TRACE");
	AT_INFO("INFO");
	AT_WARN("WARNING");
	AT_ERROR("ERROR");
	AT_CRITICAL("CRITICAL");

	app->Run();
	delete app;
}