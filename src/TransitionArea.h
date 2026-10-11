/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef TRANSITION_AREA_H
#define TRANSITION_AREA_H

#include <Rhea/Core/Area.h>

class TransitionArea : public rhea::Area
{
	RECLASS(TransitionArea, rhea::Area, "//novara/transitionarea");
public:
	TransitionArea(std::string name, std::string type_name) :
		rhea::Area(name, type_name)
	{}

	void onTargetEnter(rhea::Item* item) override;
	void onTargetExit(rhea::Item* item) override;

protected:
	bool m_transition_pending = false;
	bool m_blocked_trigger = false;
};
#endif // TRANSITION_AREA_H	