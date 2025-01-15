#pragma once

namespace Atom
{

	class Layer
	{
	public:
		Layer(const std::string& debugName);
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnImGui() {}
	private:
		std::string m_DebugName;
	};

}