#ifndef TZW_EDITOR_STATE_H
#define TZW_EDITOR_STATE_H

namespace tzw {

enum class EditorModuleId
{
	None,
	Terrain,
	Rail,
	Placement,
};

enum class TerrainToolAction
{
	None,
	CarveSphere,
	RaiseSphere,
	CarveBox,
	RaiseBox,
	PaintSphere,
};

enum class RailEditDomain
{
	None,
	Track,
	Point,
	Line,
	Train,
};

enum class RailTrackAction
{
	None,
	AddTrack,
	DeleteTrack,
};

enum class RailPointAction
{
	None,
	AddStation,
	DeleteStation,
	AddRoutePoint,
	DeleteRoutePoint,
};

enum class RailLineAction
{
	None,
	AddControlPoint,
	RemoveControlPoint,
};

struct RailEditorState
{
	RailEditDomain domain = RailEditDomain::None;
	RailTrackAction trackAction = RailTrackAction::None;
	RailPointAction pointAction = RailPointAction::None;
	RailLineAction lineAction = RailLineAction::None;

	bool isActive() const
	{
		return domain != RailEditDomain::None;
	}

	bool isTrackEditState() const
	{
		return domain == RailEditDomain::Track && trackAction != RailTrackAction::None;
	}

	bool isPointEditState() const
	{
		return domain == RailEditDomain::Point && pointAction != RailPointAction::None;
	}

	bool isLineControlEditState() const
	{
		return domain == RailEditDomain::Line && lineAction != RailLineAction::None;
	}
};

inline bool operator==(const RailEditorState& left, const RailEditorState& right)
{
	return left.domain == right.domain
		&& left.trackAction == right.trackAction
		&& left.pointAction == right.pointAction
		&& left.lineAction == right.lineAction;
}

inline bool operator!=(const RailEditorState& left, const RailEditorState& right)
{
	return !(left == right);
}

struct EditorState
{
	EditorModuleId module = EditorModuleId::None;
	TerrainToolAction terrainAction = TerrainToolAction::None;
	RailEditorState rail;

	void clear()
	{
		module = EditorModuleId::None;
		terrainAction = TerrainToolAction::None;
		rail = RailEditorState();
	}

	bool isTerrainActive() const
	{
		return module == EditorModuleId::Terrain && terrainAction != TerrainToolAction::None;
	}

	bool isRailActive() const
	{
		return module == EditorModuleId::Rail && rail.isActive();
	}
};

} // namespace tzw

#endif // TZW_EDITOR_STATE_H
