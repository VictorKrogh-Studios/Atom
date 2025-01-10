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

	std::cout << "Hello, world" << std::endl;
}