#ifndef TZW_INSPECT_PANEL_H
#define TZW_INSPECT_PANEL_H

#include "CubeGame/Rail/RailInspectTarget.h"

#include <array>
#include <string>

namespace tzw {

class RailSystem;

class InspectPanel
{
public:
	void draw(RailSystem* railSystem);
	void open();
	void close();
	bool isOpen() const;
	void inspect(const RailInspectTarget& target, RailSystem* railSystem);
	const RailInspectTarget& target() const;
	bool isInspecting(const RailInspectTarget& target) const;
	void clearTarget(RailSystem* railSystem);

private:
	void drawEmpty();
	void drawTrain(RailSystem* railSystem);
	void drawStation(RailSystem* railSystem);
	void drawRoutePoint(RailSystem* railSystem);
	void applyTargetOutline(RailSystem* railSystem, const RailInspectTarget& target, bool enabled);
	void clearTargetOutline(RailSystem* railSystem);
	void syncNameBuffer(const std::string& name);
	bool commitStationName(RailSystem* railSystem);
	bool commitRoutePointName(RailSystem* railSystem);
	bool targetExists(RailSystem* railSystem) const;

	RailInspectTarget m_target;
	RailInspectTarget m_outlinedTarget;
	bool m_isOpen = false;
	std::array<char, 128> m_nameBuffer = {};
};

} // namespace tzw

#endif // TZW_INSPECT_PANEL_H
