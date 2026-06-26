#ifndef TZW_RAIL_INSPECT_TARGET_H
#define TZW_RAIL_INSPECT_TARGET_H

#include "RailPoint.h"
#include "RailTrain.h"

namespace tzw {

enum class RailInspectTargetKind
{
	None,
	Train,
	Station,
	RoutePoint,
};

struct RailInspectTarget
{
	RailInspectTargetKind kind = RailInspectTargetKind::None;
	RailTrainId trainId = InvalidRailTrainId;
	RailStationId stationId = InvalidRailStationId;
	RailRoutePointId routePointId = InvalidRailRoutePointId;

	static RailInspectTarget train(RailTrainId trainId);
	static RailInspectTarget station(RailStationId stationId);
	static RailInspectTarget routePoint(RailRoutePointId routePointId);
	bool isValid() const;
};

} // namespace tzw

#endif // TZW_RAIL_INSPECT_TARGET_H
