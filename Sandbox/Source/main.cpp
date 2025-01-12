#include <Atom.h>

class SandboxApplication : public Atom::Application
{
public:
	SandboxApplication()
		: Application()
	{
		m_Shader = Atom::Shader::CreateFromFile("Assets/Shaders/static_vert.spv", "Assets/Shaders/static_frag.spv");
	}

	~SandboxApplication()
	{
		delete m_Shader;
		m_Shader = nullptr;
	}
private:
	Atom::Shader* m_Shader = nullptr;
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