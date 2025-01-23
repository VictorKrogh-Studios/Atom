#include <Atom.h>

#include "Layers/SandboxLayer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Examples
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Layers/Examples/FractalTreeLayer.h"

class SandboxApplication : public Atom::Application
{
public:
	SandboxApplication(const Atom::ApplicationCreateInfo& applicationCreateInfo) : Atom::Application(applicationCreateInfo)
	{
		//PushLayer(new SandboxLayer());
		PushLayer(new FractalTreeLayer());
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
	app->Run();
	delete app;
}