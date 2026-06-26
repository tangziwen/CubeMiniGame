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

private:
	void drawEmpty();
	void drawTrain(RailSystem* railSystem);
	void drawStation(RailSystem* railSystem);
	void drawRoutePoint(RailSystem* railSystem);
	void syncNameBuffer(const std::string& name);
	bool commitStationName(RailSystem* railSystem);
	bool commitRoutePointName(RailSystem* railSystem);
	bool targetExists(RailSystem* railSystem) const;

	RailInspectTarget m_target;
	bool m_isOpen = false;
	std::array<char, 128> m_nameBuffer = {};
};

} // namespace tzw

#endif // TZW_INSPECT_PANEL_H
