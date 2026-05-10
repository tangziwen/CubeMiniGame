#pragma once

#include "../../Engine/EngineDef.h"
#include "../../Mesh/Mesh.h"

namespace tzw {
	struct voxelInfo;

	enum class SurfaceNetsFace
	{
		NegativeX = 0,
		PositiveX,
		NegativeY,
		PositiveY,
		NegativeZ,
		PositiveZ,
		Count
	};

	// Boundary configuration for LOD-aware seam handling.
	//
	// `stitchFace[6]` (Negative/Positive X/Y/Z, see SurfaceNetsFace):
	//   For faces where THIS chunk is the *finer* side and the neighbour is
	//   coarser, dual vertices in the boundary cells (and the -side padding
	//   row) are snapped onto the coarse-cell dual vertex on the neighbour
	//   side of that face. The 4 fine cells covering one coarse cell collapse
	//   to a single point, producing degenerate (zero-area) triangles. The
	//   coarser neighbour is responsible for actually drawing the seam quad.
	//
	// `extendPositive[3]` (X / Y / Z):
	//   For axes where THIS chunk is the *coarser* side and the +side
	//   neighbour is finer, generate dual vertices for one extra row of cells
	//   in the +X / +Y / +Z padding so Pass 2 can emit the +side boundary
	//   quad. The fine neighbour's -side face is degenerate, so this side
	//   draws the seam.
	//
	// Same-LOD neighbours: +side chunk owns the -side boundary face (Pass 1
	// fills -X/-Y/-Z padding unconditionally; the +side boundary face is not
	// drawn unless extendPositive[axis] is set).
	//
	// `coarseData` / `coarseVoxelSize` / `coarseLodLevel` are only required
	// when any stitchFace[*] is true.
	struct SurfaceNetsStitchConfig
	{
		bool stitchFace[6] = {false, false, false, false, false, false};
		bool extendPositive[3] = {false, false, false};
		// `skipPositive[axis]` (X / Y / Z):
		//   When true, the +side boundary quad along that axis is NOT emitted
		//   here because the +side neighbour chunk is known to draw it (same
		//   LOD: neighbour emits from its -side padding row; coarser neighbour:
		//   emits via its own extendPositive on the opposite axis).
		//   Defaults to false so chunks at the world edge / streaming front
		//   (no neighbour loaded) still close their own boundary.
		bool skipPositive[3] = {false, false, false};
		voxelInfo* coarseData = nullptr;
		int coarseVoxelSize = 0;
		int coarseLodLevel = 0;
	};

	class SurfaceNets : public Singleton<SurfaceNets>
	{
	public:
		SurfaceNets();
		void generate(vec3 basePoint, Mesh* mesh, int voxelSize, voxelInfo* srcData,
			float minValue = -1, int lodLevel = 0,
			const SurfaceNetsStitchConfig* stitchConfig = nullptr);
	};
}