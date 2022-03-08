#pragma once
#include "Core/Core.h"

class ComponentProcessor {
public:
	ComponentProcessor() = default;
	~ComponentProcessor() = default;

	template<typename Type>
	void process()
	{
		LOG("Unimplemented!");
	}
};

template<class Processor, typename ...Types>
void processComponents(Processor& processor)
{
	static_assert(std::is_base_of<ComponentProcessor, Processor>(), "Processor is not derived from ComponentProcessor");

	using Expander = int[];
	(void)Expander { 0, ((void)(processor.template process<Types>()), 0)... };
}

template<class Processor>
void processAllComponents(Processor& processor)
{
	processComponents<Processor, RenderingComponent, TextureComponent, LightComponent, TagComponent, ScriptingComponent, TransformComponent, SkyboxComponent>(processor);
}