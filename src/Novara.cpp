#include "Novara.h"
#include <Rhea/Core/WorldService.h>
#include <Psio/Log.h>
#include <Rhea/Input/InputEvent.h>
#include <Rhea/EventInput.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/IPhysicsDebugDraw.h>
#include <Rhea/Core/Controller.h>
#include <Rhea/Controller/BasicController.h>
#include <Math/Math.h>
#include <Rhea/GUI/Label.h>
#include <Rhea/Controller/ControllerService.h>
#include "TransitionArea.h"
#include <Rhea/Character/KinematicCharacter.h>

void Novara::setup()
{
	addDefaultWindow();
	m_windows[0].views[0].layers.push_back({ "HUD",0,0,m_windows[0].width, m_windows[0].height });

	// register contoller service
	if (!getService("controller")) {
		log("Ceto: Registering controller service");
		rhea::ControllerService* srv = new rhea::ControllerService("controllerservice");
		srv->setup(this);
		registerService("controller", srv);
	}

	Application::setup();

	TransitionArea::_register_TransitionArea();

	if (auto* controllers = getService<rhea::ControllerService>()) {
		using PlacementResult = rhea::ControllerService::PlacementResult;

		controllers->addPlacementHandler([](rhea::ControllerService::PlacementContext& context) -> PlacementResult {
			const auto* transition = context.transition;
			if (!transition || transition->starts_new_session || !transition->source_scene || !transition->target_scene) {
				return PlacementResult::Unhandled;
			}

			const auto& source = transition->source_scene->getPathName();
			const auto& target = transition->target_scene->getPathName();
			const bool harbour_town_transition =
				(source == "harbour" && target == "town") ||
				(source == "town" && target == "harbour");

			if (!harbour_town_transition)
				return PlacementResult::Unhandled;

			if (!context.source_character || !context.target_character)
				return PlacementResult::Failed;

			const auto position = context.source_character->getTransform()->getWorldPosition();
			context.target_character->getTransform()->setWorldPosition(position);

			return PlacementResult::Placed;
			});
	}
}

int Novara::openWorld(std::string world_rid)
{

	auto* worlds = doCast<rhea::WorldService>(getService("world"));
	if (!worlds) {
		LOG(LOG_ERROR) << "WorldService not found";
		return -1;	
	}
	
	worlds->registerEventHandler("primary_scene_changed", this, [this](rhea::Event* ev, rhea::Unit* unit) -> bool {
		rhea::WorldService::PrimarySceneChangedEvent* e = static_cast<rhea::WorldService::PrimarySceneChangedEvent*>(ev);
		
		if (e->current_scene) 			
			LOG(LOG_INFO) << "Scene activated: " << e->current_scene->getPathName();
		else
			LOG(LOG_INFO) << "Empty scene activated";

		m_current_scene = e->current_scene;
		auto* controllers = getService<rhea::ControllerService>();
		m_controller = controllers ? controllers->getPrimaryController() : nullptr;
		
		return false;
		});

	worlds->addSceneActivateHandler([this](rhea::WorldService::ScenePrepareContext* context) {
		if (!context->scene) {
			LOG(LOG_ERROR) << "World has no start scene";
		}
		if (context->scene->getPathName() == "novaragui") {
			LOG(LOG_INFO) << "GUI scene prepare handler called";
			m_debug_text = doCast<rhea::Label>(context->scene->find("gui/debug_text"));
		}
		});			


	worlds->openWorld(world_rid, [this, worlds](rhea::ErrorReport error) {
		if (error.isError()) {
			LOG(LOG_ERROR) << "Failed to open world: " << error.errors[0].getMessage();
			return;
		}
		LOG(LOG_INFO) << "World opened successfully";

		// also start gui
		std::string gui_scene_rid = "//novara/gui/gui.scene";
		worlds->activateScene(gui_scene_rid, rhea::WorldService::SceneRole::Overlay, nullptr, [this](rhea::ErrorReport error) {
			if (error.isError()) {
				LOG(LOG_ERROR) << "Failed to activate GUI scene: " << error.errors[0].getMessage();
				return;
			}
			LOG(LOG_INFO) << "GUI scene activated successfully";
			});

		// start environment
		std::string env_scene_rid = "//novara/environment/environment.scene";
		worlds->activateScene(env_scene_rid, rhea::WorldService::SceneRole::Overlay, nullptr, [this](rhea::ErrorReport error) {
			if (error.isError()) {
				LOG(LOG_ERROR) << "Failed to activate environment scene: " << error.errors[0].getMessage();
				return;
			}
			LOG(LOG_INFO) << "Environment scene activated successfully";
			});

		// start harbour gate as partition scene
		std::string partition_scene_rid = "//novara/harbourgate/harbourgate.scene";
		worlds->activateScene(partition_scene_rid, rhea::WorldService::SceneRole::Partition, nullptr, [this](rhea::ErrorReport error) {
			if (error.isError()) {
				LOG(LOG_ERROR) << "Failed to activate partition scene: " << error.errors[0].getMessage();
				return;
			}
			LOG(LOG_INFO) << "Partition scene activated successfully";
			});

		});

	return 0;
}

void Novara::onScene(rhea::Scene* scene)
{
	
}

bool Novara::onInput(rhea::EventInput* e, rhea::Unit* unit)
{
	rhea::InputEvent evt = e->getEvent();
	if (evt.isKeyPressed(rhea::Key::Num0)) {
		auto* mgr = doCast<rhea::PhysicsManagerService>(getService("physics_mgr"));
		auto* physics_srv = mgr ? mgr->getPhysicsService() : nullptr;
		static rhea::IPhysicsDebugDraw* debug_draw = nullptr;
		if (!debug_draw) {
			debug_draw = physics_srv->getDebugDrawer();
			if (m_current_scene) {
				m_current_scene->addChild(debug_draw);
				debug_draw->setEnabled(true);
			}
		}
		else
			debug_draw->toggle();
	}
	return false;
}

bool Novara::onFrameEnd()
{
	if (!m_controller) {
		auto* controller_service = doCast<rhea::ControllerService>(getService("controller"));
		if (controller_service) {
			m_controller = controller_service->getPrimaryController();
		}
	}
	if (m_controller) {
		math::Vector3 pos = static_cast<rhea::BasicController*>(m_controller)->getWorldPosition();
		math::Quaternion rot = static_cast<rhea::BasicController*>(m_controller)->getOrientation();
		float angle;
		math::Vector3 axis;
		rot.getAsAngleAxis(angle, axis);
		std::string debug = "Welcome to novara\nController: " + m_controller->getPathName();
		debug += "\nPosition: " + std::to_string(pos.x) + " / " + std::to_string(pos.y) + " / " + std::to_string(pos.z);
		debug += "\nRotation angle=" + std::to_string(angle * math::radToDeg);

		if (m_debug_text) {
			m_debug_text->setText(debug);
		}
	}
	return false;
}
