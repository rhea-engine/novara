#ifndef NOVARA_H
#define NOVARA_H

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>

namespace rhea
{
	class Scene;
	class Controller;
	class Label;
}

class Novara : public rhea::Application
{
public:
	Novara() {}
	void setup() override;

	int openWorld(std::string world_name);

	void onScene(rhea::Scene* scene) override;

	bool onInput(rhea::EventInput* e, rhea::Unit* unit) override;
	bool onFrameEnd() override;
private:
	rhea::Scene* m_current_scene = nullptr;
	rhea::Controller* m_controller = nullptr;
	rhea::Label* m_debug_text = nullptr;
};

#endif // !NOVARA_H