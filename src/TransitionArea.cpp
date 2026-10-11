/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include "TransitionArea.h"
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/Core/WorldService.h>

void TransitionArea::onTargetEnter(rhea::Item* item)
{
	LOG(LOG_DEBUG) << "TransitionArea: item entered transition area: " << item->getPathName();

	auto* worlds = m_app->getService<rhea::WorldService>();

	if (!worlds || worlds->getState() != rhea::WorldService::State::Ready)
		return;

	if (m_transition_pending || m_blocked_trigger)
		return;


    rhea::Scene* current = worlds->getPrimaryScene();
    if (!current)
        return;

    rhea::WorldService::TransitionRequest request;

    if (current->getPathName() == "harbour") {
        request.target_resource_id =
            "//novara/town/town.scene";
    }
    else if (current->getPathName() == "town") {
        request.target_resource_id =
            "//novara/harbour/harbour.scene";
    }
    else {
        return;
    }

    m_transition_pending = true;
	m_blocked_trigger = true;

    worlds->transitionToScene(
        request,
        [this](rhea::WorldService::OpenError error)
        {
            m_transition_pending = false;

            if (error != rhea::WorldService::OpenError::None) {
                LOG(LOG_INFO) << "Try again, transition failed";
                m_blocked_trigger = false;
            }
        }
    );

    return;
}

void TransitionArea::onTargetExit(rhea::Item* item)
{
    LOG(LOG_DEBUG) << "TransitionArea: item exited transition area: " << item->getPathName();
	m_blocked_trigger = false;
}
