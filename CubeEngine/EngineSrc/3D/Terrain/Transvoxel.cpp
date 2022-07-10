#include "TransVoxel.h"

#include "MCTable.h"
#include "CubeGame/GameMap.h"
#include "transvoxel_tables.h"
#include "CubeGame/GameConfig.h"
#include <algorithm>
#include <array>
#include "MarchingCubes.h"



struct Vector3i
{
	static const unsigned int AXIS_X = 0;
	static const unsigned int AXIS_Y = 1;
	static const unsigned int AXIS_Z = 2;
	static const unsigned int AXIS_COUNT = 3;
	Vector3i():x(0),y(0),z(0){};
	explicit Vector3i(int xyz) :
			x(xyz),
			y(xyz),
			z(xyz) {}
	Vector3i(int _x, int _y, int _z):x(_x),y(_y),z(_z){};
	Vector3i operator *(int val) const{return Vector3i(x * val, y * val, z * val);};
	Vector3i operator+(const Vector3i &other) const
	{
	    return Vector3i(x + other.x, y + other.y, z +other.z);
	}

	void operator-=(const Vector3i &other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	Vector3i operator-() const {
		return Vector3i(-x, -y, -z);
	}
	Vector3i operator-(const Vector3i &b) const
	{
		return Vector3i(x - b.x, y - b.y, z - b.z);
	}
	Vector3i operator<<(int b) const {
		return Vector3i(x << b, y << b, z << b);
	}

	Vector3i operator>>(int b) {
		return Vector3i(x >> b, y >> b, z >> b);
	}
	union {
		struct {
			int x;
			int y;
			int z;
		};
		int coords[3];
	};
	tzw::vec3 to_vec3()
	{
		return tzw::vec3(x, y, z);
	}
	const int &operator[](unsigned int i) const
	{
		return coords[i];
	}
};


namespace Cube {

// Index convention used in some lookup tables
enum Side {
	SIDE_LEFT = 0,
	SIDE_RIGHT,
	SIDE_BOTTOM,
	SIDE_TOP,
	// TODO I would like to swap the Z ones because it's inconsistent with coordinates...
	SIDE_BACK,
	SIDE_FRONT,

	SIDE_COUNT
};
// Alias to the above for clarity, fixing some interpretation problems regarding the side_normals table...
enum SideAxis {
	SIDE_POSITIVE_X = 0,
	SIDE_NEGATIVE_X,
	SIDE_NEGATIVE_Y,
	SIDE_POSITIVE_Y,
	SIDE_NEGATIVE_Z,
	SIDE_POSITIVE_Z
};

// Index convention used in some lookup tables
enum Edge {
	EDGE_BOTTOM_BACK = 0,
	EDGE_BOTTOM_RIGHT,
	EDGE_BOTTOM_FRONT,
	EDGE_BOTTOM_LEFT,
	EDGE_BACK_LEFT,
	EDGE_BACK_RIGHT,
	EDGE_FRONT_RIGHT,
	EDGE_FRONT_LEFT,
	EDGE_TOP_BACK,
	EDGE_TOP_RIGHT,
	EDGE_TOP_FRONT,
	EDGE_TOP_LEFT,

	EDGE_COUNT
};

// Index convention used in some lookup tables
enum Corner {
	CORNER_BOTTOM_BACK_LEFT = 0,
	CORNER_BOTTOM_BACK_RIGHT,
	CORNER_BOTTOM_FRONT_RIGHT,
	CORNER_BOTTOM_FRONT_LEFT,
	CORNER_TOP_BACK_LEFT,
	CORNER_TOP_BACK_RIGHT,
	CORNER_TOP_FRONT_RIGHT,
	CORNER_TOP_FRONT_LEFT,

	CORNER_COUNT
};

}
static const float FIXED_FACTOR = 1.f / 256.f;

static const float TRANSITION_CELL_SCALE = 0.25;
constexpr int AXIS_COUNT = 3;
namespace tzw {


	vec3 get_border_offset(vec3 pos, const int lod_index, const Vector3i block_size) {
		// When transition meshes are inserted between blocks of different LOD, we need to make space for them.
		// Secondary vertex positions can be calculated by linearly transforming positions inside boundary cells
		// so that the full-size cell is scaled to a smaller size that allows space for between one and three
		// transition cells, as necessary, depending on the location with respect to the edges and corners of the
		// entire block. This can be accomplished by computing offsets (Δx, Δy, Δz) for the coordinates (x, y, z)
		// in any boundary cell.

		vec3 delta;

		const float p2k = 1 << lod_index; // 2 ^ lod
		const float p2mk = 1.f / p2k; // 2 ^ (-lod)

		const float wk = TRANSITION_CELL_SCALE * p2k; // 2 ^ (lod - 2), if scale is 0.25

		for (unsigned int i = 0; i < AXIS_COUNT; ++i) {
			const float p = pos[i];
			const float s = block_size[i];

			if (p < p2k) {
				// The vertex is inside the minimum cell.
				delta[i] = (1.0f - p2mk * p) * wk;

			} else if (p > (p2k * (s - 1))) {
				// The vertex is inside the maximum cell.
				delta[i] = ((p2k * s) - 1.0f - p) * wk;
			}
		}

		return delta;
	}

	inline vec3 project_border_offset(vec3 delta, vec3 normal) {
		// Secondary position can be obtained with the following formula:
		//
		// | x |   | 1 - nx²   ,  -nx * ny  ,  -nx * nz |   | Δx |
		// | y | + | -nx * ny  ,  1 - ny²   ,  -ny * nz | * | Δy |
		// | z |   | -nx * nz  ,  -ny * nz  ,  1 - nz²  |   | Δz |
		//
		return vec3((1 - normal.x * normal.x) * delta.x /**/ - normal.y * normal.x * delta.y /*     */ -
						normal.z * normal.x * delta.z,
				/**/ -normal.x * normal.y * delta.x + (1 - normal.y * normal.y) * delta.y /*    */ -
						normal.z * normal.y * delta.z,
				/**/ -normal.x * normal.z * delta.x /**/ - normal.y * normal.z * delta.y /**/ +
						(1 - normal.z * normal.z) * delta.z);
	}

	inline vec3 get_secondary_position(
			const vec3 primary, const vec3 normal, const int lod_index, const Vector3i block_size) {
		vec3 delta = get_border_offset(primary, lod_index, block_size);
		delta = project_border_offset(delta, normal);
		return primary + delta;
	}
	tzw::VertexData genVertexData(VoxelVertex v)
	{
		tzw::VertexData a(v.vertex);
		a.m_matIndex[0] = v.matInfo.matIndex1;
		a.m_matIndex[1] = v.matInfo.matIndex2;
		a.m_matIndex[2] = v.matInfo.matIndex3;
		float factor1 = HIGH_FOUR_BIT( v.matInfo.matBlendFactor) / 15.f;
		float factor2 = LOW_FOUR_BIT( v.matInfo.matBlendFactor) / 15.f;
		float factor3 = 1 - factor1 - factor2;
		a.m_matBlendFactor = vec3(factor1, factor2, factor3);
		a.m_texCoord = v.vertex.xz();
		return a;
	}

	VoxelVertex getVoxelVertex(vec3 v, vec3 basePoint, voxelInfo * vi0, voxelInfo * vi1)
	{
		VoxelVertex vertex;
		vertex.vertex =  vec3(v.x * BLOCK_SIZE, v.y * BLOCK_SIZE, v.z * BLOCK_SIZE) + basePoint;
		vertex.matInfo = vi0->matInfo;
		return vertex;
	}

inline uint8_t get_border_mask(const Vector3i &pos, const Vector3i &block_size) {

uint8_t mask = 0;

//  1: -X
//  2: +X
//  4: -Y
//  8: +Y
// 16: -Z
// 32: +Z

for (unsigned int i = 0; i < Vector3i::AXIS_COUNT; i++) {
	// Close to negative face.
	if (pos[i] == 0) {
		mask |= (1 << (i * 2));
	}
	// Close to positive face.
	if (pos[i] == block_size[i]) {
		mask |= (1 << (i * 2 + 1));
	}
}

return mask;
}
inline float tof(int8_t v) {
	return static_cast<float>(v) / 256.f;
}

inline float s8_to_snorm_noclamp(int8_t v) {
	return v / 127.f;
}

int8_t tos(voxelInfo * info, int THE_SIZE, int x, int y, int z)
{
	int theIndex = x * THE_SIZE * THE_SIZE + y * THE_SIZE + z;
	unsigned char w = info[theIndex].w;//std::clamp(info[theIndex].w, -1.f, 1.f);
	//to 0, 255
	//uint8_t a = 255 - w;//(w * 0.5f + 0.5f) * 255.f;
	return -(w - 127);
}
int8_t tos(voxelInfo * info, int BLOCKSIZE, Vector3i v)
{
	return tos(info, BLOCKSIZE, v.x, v.y, v.z);
}

voxelInfo * extractVoxel(voxelInfo * info, int VOXEL_SIZE, Vector3i v)
{
	int theIndex = v.x * VOXEL_SIZE * VOXEL_SIZE + v.y * VOXEL_SIZE + v.z;
	return info + theIndex;
}
static uint8_t sign(int8_t v)
{
	return (v >> 7) & 1;
}

vec3 getV3(vec3 inPoint, float blockSize)
{
	return vec3(inPoint.x * blockSize, inPoint.y * blockSize, -inPoint.z *blockSize);
}
struct voxelPosInfo
{
	Vector3i v;
	voxelInfo * info;
};

inline unsigned int get_zxy_index(const Vector3i &v, const Vector3i area_size) {
	//return v.y + area_size.y * (v.x + area_size.x * v.z); // ZXY
	return v.z + area_size.y * (v.y + area_size.x * v.x); // ZXY
}
static const float INV_0x7f = 1.f / 0x7f;
float sdf_as_float(voxelInfo & voxel)
{
	float snorm = (static_cast<float>(voxel.w) - 0x7f) * INV_0x7f;
	return -1.f * snorm;
}

// SDF values considered negative have a sign bit of 1 in this algorithm
inline uint8_t sign_f(float v) {
	return v < 0.f;
}
#pragma optimize("", off)
void TransVoxel::generateWithoutNormal(vec3 basePoint, Mesh* mesh, Mesh * transitionMesh, int VOXEL_SIZE,
	voxelInfo* srcData, float minValue, int lodLevel)
{
	const Vector3i block_size_with_padding = Vector3i(VOXEL_SIZE, VOXEL_SIZE, VOXEL_SIZE);
	const Vector3i block_size = block_size_with_padding - Vector3i(MIN_PADDING + MAX_PADDING);
	const vec3 block_size_scaled = vec3(int(block_size.x) << lodLevel, int(block_size.y) << lodLevel, int(block_size.z) << lodLevel);

	
	const Vector3i min_pos = Vector3i(MIN_PADDING);
	const Vector3i max_pos = block_size_with_padding - Vector3i(MAX_PADDING);
	
	vec3 corner_gradients[8];
	voxelInfo * info[8];

	// How much to advance in the data array to get neighbor voxels
	const unsigned int n010 = block_size_with_padding.y; // Y+1
	const unsigned int n100 = block_size_with_padding.y * block_size_with_padding.x; // X+1
	const unsigned int n001 = 1; // Z+1
	const unsigned int n110 = n010 + n100;
	const unsigned int n101 = n100 + n001;
	const unsigned int n011 = n010 + n001;
	const unsigned int n111 = n100 + n010 + n001;

	Vector3i pos;
	for (pos.z = min_pos.z; pos.z < max_pos.z; ++pos.z) {
		for (pos.y = min_pos.y; pos.y < max_pos.y; ++pos.y) {
			// TODO Optimization: change iteration to be ZXY? (Data is laid out with Y as deepest coordinate)
			
			for (pos.x = min_pos.x; pos.x < max_pos.x; ++pos.x) {
				unsigned int data_index = get_zxy_index(Vector3i(pos.x, pos.y, pos.z), block_size_with_padding);

				{
					// The chosen comparison here is very important. This relates to case selections where 4 samples
					// are equal to the isolevel and 4 others are above or below:
					// In one of these two cases, there has to be a surface to extract, otherwise no surface will be
					// allowed to appear if it happens to line up with integer coordinates.
					// If we used `<` instead of `>`, it would appear to work, but would break those edge cases.
					// `>` is chosen because it must match the comparison we do with case selection (in Transvoxel
					// it is inverted).
					const int isolevel = 128;
					const bool s = srcData[data_index].w >= isolevel;

					if ( //
							(srcData[data_index + n010].w >= isolevel) == s &&
							(srcData[data_index + n100].w >= isolevel) == s &&
							(srcData[data_index + n110].w >= isolevel) == s &&
							(srcData[data_index + n001].w >= isolevel) == s &&
							(srcData[data_index + n011].w >= isolevel) == s &&
							(srcData[data_index + n101].w >= isolevel) == s &&
							(srcData[data_index + n111].w >= isolevel) == s) {
						// Not crossing the isolevel, this cell won't produce any geometry.
						// We must figure this out as fast as possible, because it will happen a lot.
						continue;
					}
				}
				//    6-------7
				//   /|      /|
				//  / |     / |  Corners
				// 4-------5  |
				// |  2----|--3
				// | /     | /   z y
				// |/      |/    |/
				// 0-------1     o--x
				//
				std::array<unsigned int, 8> corner_data_indices;
				corner_data_indices[0] = data_index;
				corner_data_indices[1] = data_index + n100;
				corner_data_indices[2] = data_index + n010;
				corner_data_indices[3] = data_index + n110;
				corner_data_indices[4] = data_index + n001;
				corner_data_indices[5] = data_index + n101;
				corner_data_indices[6] = data_index + n011;
				corner_data_indices[7] = data_index + n111;

				std::array<float, 8> cell_samples_sdf;
				for (unsigned int i = 0; i < corner_data_indices.size(); ++i) {
					cell_samples_sdf[i] = sdf_as_float(srcData[corner_data_indices[i]]);
				}




				// Concatenate the sign of cell values to obtain the case code.
				// Index 0 is the less significant bit, and index 7 is the most significant bit.
				uint8_t case_code = sign_f(cell_samples_sdf[0]);
				case_code |= (sign_f(cell_samples_sdf[1]) << 1);
				case_code |= (sign_f(cell_samples_sdf[2]) << 2);
				case_code |= (sign_f(cell_samples_sdf[3]) << 3);
				case_code |= (sign_f(cell_samples_sdf[4]) << 4);
				case_code |= (sign_f(cell_samples_sdf[5]) << 5);
				case_code |= (sign_f(cell_samples_sdf[6]) << 6);
				case_code |= (sign_f(cell_samples_sdf[7]) << 7);


				// ReuseCell &current_reuse_cell = get_reuse_cell(pos);
				// Mark as unusable for now
				// current_reuse_cell.vertices[0] = -1;

				if (case_code == 0 || case_code == 255) {
					// If the case_code is 0 or 255, there is no triangulation to do
					continue;
				}

				std::array<Vector3i, 8> padded_corner_positions;
				padded_corner_positions[0] = Vector3i(pos.x, pos.y, pos.z);
				padded_corner_positions[1] = Vector3i(pos.x + 1, pos.y, pos.z);
				padded_corner_positions[2] = Vector3i(pos.x, pos.y + 1, pos.z);
				padded_corner_positions[3] = Vector3i(pos.x + 1, pos.y + 1, pos.z);
				padded_corner_positions[4] = Vector3i(pos.x, pos.y, pos.z + 1);
				padded_corner_positions[5] = Vector3i(pos.x + 1, pos.y, pos.z + 1);
				padded_corner_positions[6] = Vector3i(pos.x, pos.y + 1, pos.z + 1);
				padded_corner_positions[7] = Vector3i(pos.x + 1, pos.y + 1, pos.z + 1);


				// TODO We might not always need all of them
				// Compute normals
				for (unsigned int i = 0; i < 8; ++i) {
				
					auto p = padded_corner_positions[i];
					
					float nx = tof(tos(srcData, VOXEL_SIZE, p.x - 1, p.y, p.z));
					float ny = tof(tos(srcData, VOXEL_SIZE, p.x, p.y - 1, p.z));
					float nz = tof(tos(srcData, VOXEL_SIZE, p.x, p.y, p.z - 1));
					float px = tof(tos(srcData, VOXEL_SIZE, p.x + 1, p.y, p.z));
					float py = tof(tos(srcData, VOXEL_SIZE, p.x, p.y + 1, p.z));
					float pz = tof(tos(srcData, VOXEL_SIZE, p.x, p.y, p.z + 1));
					
					//get_gradient_normal(nx, px, ny, py, nz, pz, cell_samples[i]);
					corner_gradients[i] = vec3(nx - px, ny - py, nz - pz);
				}
				voxelPosInfo corner_positions[8];
				for (unsigned int i = 0; i < padded_corner_positions.size(); ++i) {
					const Vector3i p = padded_corner_positions[i];
					// Get the value of cells.
					// Negative values are "solid" and positive are "air".
					// Due to raw cells being unsigned 8-bit, they get converted to signed.
					corner_positions[i].info = extractVoxel(srcData, VOXEL_SIZE, p);
					// Undo padding here. From this point, corner positions are actual positions.
					corner_positions[i].v = (p - min_pos) << lodLevel;
				}


				// For cells occurring along the minimal boundaries of a block,
				// the preceding cells needed for vertex reuse may not exist.
				// In these cases, we allow new vertex creation on additional edges of a cell.
				// While iterating through the cells in a block, a 3-bit mask is maintained whose bits indicate
				// whether corresponding bits in a direction code are valid
				uint8_t direction_validity_mask =
						(pos.x > min_pos.x ? 1 : 0) |
						((pos.y > min_pos.y ? 1 : 0) << 1) |
						((pos.z > min_pos.z ? 1 : 0) << 2);

				uint8_t regular_cell_class_index = Transvoxel::get_regular_cell_class(case_code);
				const Transvoxel::RegularCellData &regular_cell_data = Transvoxel::get_regular_cell_data(regular_cell_class_index);
				uint8_t triangle_count = regular_cell_data.geometryCounts & 0x0f;
				uint8_t vertex_count = (regular_cell_data.geometryCounts & 0xf0) >> 4;

				int cell_vertex_indices[12] = {-1};
				float realBlockSize = BLOCK_SIZE;
				// uint8_t cell_border_mask = get_border_mask(pos - min_pos, block_size - Vector3i(1));

				// For each vertex in the case
				for (unsigned int i = 0; i < vertex_count; ++i) {

					// The case index maps to a list of 16-bit codes providing information about the edges on which the vertices lie.
					// The low byte of each 16-bit code contains the corner indexes of the edge’s endpoints in one nibble each,
					// and the high byte contains the mapping code shown in Figure 3.8(b)
					unsigned short rvd = Transvoxel::get_regular_vertex_data(case_code, i);
					uint8_t edge_code_low = rvd & 0xff;
					uint8_t edge_code_high = (rvd >> 8) & 0xff;

					// Get corner indexes in the low nibble (always ordered so the higher comes last)
					uint8_t v0 = (edge_code_low >> 4) & 0xf;
					uint8_t v1 = edge_code_low & 0xf;

					// ERR_FAIL_COND(v1 <= v0);

					// Get voxel values at the corners
					const float sample0 = cell_samples_sdf[v0]; // called d0 in the paper
					const float sample1 = cell_samples_sdf[v1]; // called d1 in the paper

					// TODO Zero-division is not mentionned in the paper??
					// ERR_FAIL_COND(sample1 == sample0);
					// ERR_FAIL_COND(sample1 == 0 && sample0 == 0);

					// Get interpolation position
					// We use an 8-bit fraction, allowing the new vertex to be located at one of 257 possible
					// positions  along  the  edge  when  both  endpoints  are included.
					//int t = (sample1 << 8) / (sample1 - sample0);
					const float t = sample1 / (sample1 - sample0);



					voxelPosInfo p0 = corner_positions[v0];
					voxelPosInfo p1 = corner_positions[v1];
					vec4 lodColor = vec4(1, 1, 1, 1);
					switch(lodLevel)
					{
					case 1:
						lodColor = vec4(0, 1, 0, 1);
						break;
					case 2:
						lodColor = vec4(1, 1, 0, 1);
						break;
					default:
						break;
					}
					if (t > 0.f && t < 1.f) {
						// Vertex is between p0 and p1 (inside the edge)

						// Each edge of a cell is assigned an 8-bit code, as shown in Figure 3.8(b),
						// that provides a mapping to a preceding cell and the coincident edge on that preceding cell
						// for which new vertex creation  was  allowed.
						// The high nibble of this code indicates which direction to go in order to reach the correct preceding cell.
						// The bit values 1, 2, and 4 in this nibble indicate that we must subtract one
						// from the x, y, and/or z coordinate, respectively.
						uint8_t reuse_dir = (edge_code_high >> 4) & 0xf;
						uint8_t reuse_vertex_index = edge_code_high & 0xf;

						// TODO Some re-use opportunities are missed on negative sides of the block,
						// but I don't really know how to fix it...
						// You can check by "shaking" every vertex randomly in a shader based on its index,
						// you will see vertices touching the -X, -Y or -Z sides of the block aren't connected

						bool present = false;//(reuse_dir & direction_validity_mask) == reuse_dir;

						// if (present) {
						// 	Vector3i cache_pos = pos + L::dir_to_prev_vec(reuse_dir);
						// 	ReuseCell &prev_cell = get_reuse_cell(cache_pos);
						// 	// Will reuse a previous vertice
						// 	cell_vertex_indices[i] = prev_cell.vertices[reuse_vertex_index];
						// }


						// Going to create a new vertice

						const float t0 = t; //static_cast<float>(t) / 256.f;
						const float t1 = 1.f - t; //static_cast<float>(0x100 - t) / 256.f;

						vec3 primaryf = p0.v.to_vec3() * t0 + p1.v.to_vec3() * t1;
						cell_vertex_indices[i] = mesh->getVerticesSize();
						auto vdata = genVertexData(getVoxelVertex(primaryf, basePoint,p0.info, p0.info));
						vdata.m_normal = (corner_gradients[v0] * t0 + corner_gradients[v1] * t1).normalized();
						vdata.m_color = lodColor;
						mesh->addVertex(vdata);
						


					} else if (t == 0 && v1 == 7) {
						// t == 0: the vertex is on p1
						// v1 == 7: p1 on the max corner of the cell
						// This cell owns the vertex, so it should be created.

						Vector3i primary = p1.v;
						vec3 primaryf = primary.to_vec3();
						cell_vertex_indices[i] = mesh->getVerticesSize();
						auto vdata = genVertexData(getVoxelVertex(primaryf, basePoint,p1.info, p1.info));
						vdata.m_color = lodColor;
						vdata.m_normal = corner_gradients[v1];
						mesh->addVertex(vdata);

					} else {
						// The vertex is either on p0 or p1
						// Always try to reuse previous vertices in these cases

						// A 3-bit direction code leading to the proper cell can easily be obtained by
						// inverting the 3-bit corner index (bitwise, by exclusive ORing with the number 7).
						// The corner index depends on the value of t, t = 0 means that we're at the higher
						// numbered endpoint.


						voxelPosInfo primaryP = t == 0 ? p1 : p0;
						vec3 primaryf = primaryP.v.to_vec3();
						cell_vertex_indices[i] = mesh->getVerticesSize();
						auto vdata = genVertexData(getVoxelVertex(primaryf, basePoint,primaryP.info, primaryP.info));
						vdata.m_normal = corner_gradients[t == 0 ? v1 : v0].normalized();
						vdata.m_color = lodColor;
						mesh->addVertex(vdata);
					}

				} // for each cell vertex
				
				for (int t = 0; t < triangle_count; ++t) {
					const int t0 = t * 3;
					mesh->addIndex(cell_vertex_indices[regular_cell_data.get_vertex_index(t0 + 2)]);
					mesh->addIndex(cell_vertex_indices[regular_cell_data.get_vertex_index(t0 + 1)]);
					mesh->addIndex(cell_vertex_indices[regular_cell_data.get_vertex_index(t0 + 0)]);
				}

			} // x
		} // y
	} // z
	//if(!lodLevel) return;
	
	for (int dir = 0; dir < Cube::SIDE_COUNT; ++dir) 
	{
		build_transition(basePoint, transitionMesh, VOXEL_SIZE, srcData, dir, lodLevel);
	}

	// printf("done %d, %d\n", mesh->getIndicesSize(), mesh->getVerticesSize());
}



#pragma optimize("", off)
void TransVoxel::build_transition(vec3 basePoint,Mesh * mesh, int VOXEL_SIZE, voxelInfo * srcData, int direction, int lodLevel)
{

	//    y            y
	//    |            | z
	//    |            |/     OpenGL axis convention
	//    o---x    x---o
	//   /
	//  z

	struct L {
		// Convert from face-space to block-space coordinates, considering which face we are working on.
		static inline Vector3i face_to_block(int x, int y, int z, int dir, const Vector3i &bs) {

			// There are several possible solutions to this, because we can rotate the axes.
			// We'll take configurations where XY map different axes at the same relative orientations,
			// so only Z is flipped in half cases.
			switch (dir) {

				case Cube::SIDE_NEGATIVE_X:
					return Vector3i(z, x, y);

				case Cube::SIDE_POSITIVE_X:
					return Vector3i(bs.x - 1 - z, y, x);

				case Cube::SIDE_NEGATIVE_Y:
					return Vector3i(y, z, x);

				case Cube::SIDE_POSITIVE_Y:
					return Vector3i(x, bs.y - 1 - z, y);

				case Cube::SIDE_NEGATIVE_Z:
					return Vector3i(x, y, z);

				case Cube::SIDE_POSITIVE_Z:
					return Vector3i(y, x, bs.z - 1 - z);

				default:
					CRASH_COND(true);
					return Vector3i();
			}
		}
		// I took the choice of supporting non-cubic area, so...
		static inline void get_face_axes(int &ax, int &ay, int dir) {
			switch (dir) {

				case Cube::SIDE_NEGATIVE_X:
					ax = Vector3i::AXIS_Y;
					ay = Vector3i::AXIS_Z;
					break;

				case Cube::SIDE_POSITIVE_X:
					ax = Vector3i::AXIS_Z;
					ay = Vector3i::AXIS_Y;
					break;

				case Cube::SIDE_NEGATIVE_Y:
					ax = Vector3i::AXIS_Z;
					ay = Vector3i::AXIS_X;
					break;

				case Cube::SIDE_POSITIVE_Y:
					ax = Vector3i::AXIS_X;
					ay = Vector3i::AXIS_Z;
					break;

				case Cube::SIDE_NEGATIVE_Z:
					ax = Vector3i::AXIS_X;
					ay = Vector3i::AXIS_Y;
					break;

				case Cube::SIDE_POSITIVE_Z:
					ax = Vector3i::AXIS_Y;
					ay = Vector3i::AXIS_X;
					break;

				default:
					CRASH_COND(true);
			}
		}
	};

	// if (p_voxels.is_uniform(channel)) {
	// 	// Nothing to extract, because constant isolevels never cross the threshold and describe no surface
	// 	return;
	// }

	const Vector3i block_size_with_padding = Vector3i(VOXEL_SIZE);
	const Vector3i block_size_without_padding = block_size_with_padding - Vector3i(MIN_PADDING + MAX_PADDING, MIN_PADDING + MAX_PADDING, MIN_PADDING + MAX_PADDING);
	const Vector3i block_size_scaled = block_size_without_padding << lodLevel;

	// ERR_FAIL_COND(block_size_with_padding.x < 3);
	// ERR_FAIL_COND(block_size_with_padding.y < 3);
	// ERR_FAIL_COND(block_size_with_padding.z < 3);

	// reset_reuse_cells_2d(block_size_with_padding);

	// This part works in "face space", which is 2D along local X and Y axes.
	// In this space, -Z points towards the half resolution cells, while +Z points towards full-resolution cells.
	// Conversion is used to map this space to block space using a direction enum.

	// Note: I made a few changes compared to the paper.
	// Instead of making transition meshes go from low-res blocks to high-res blocks,
	// I do the opposite, going from high-res to low-res. It's easier because half-res voxels are available for free,
	// if we compute the transition meshes right after the regular mesh, with the same voxel data.

	// This represents the actual box of voxels we are working on.
	// It also represents positions of the minimum and maximum vertices that can be generated.
	// Padding is present to allow reaching 1 voxel further for calculating normals
	const Vector3i min_pos = Vector3i(MIN_PADDING);
	const Vector3i max_pos = block_size_with_padding - Vector3i(MAX_PADDING);

	int axis_x, axis_y;
	L::get_face_axes(axis_x, axis_y, direction);
	const int min_fpos_x = min_pos[axis_x];
	const int min_fpos_y = min_pos[axis_y];
	const int max_fpos_x = max_pos[axis_x] - 1; // Another -1 here, because the 2D kernel is 3x3
	const int max_fpos_y = max_pos[axis_y] - 1;

	// How much to advance in the data array to get neighbor voxels
	const unsigned int n010 = block_size_with_padding.y;// Y+1
	const unsigned int n100 = block_size_with_padding.y * block_size_with_padding.x;// X+1
	const unsigned int n001 = 1; // Z+1

	// Using temporay locals otherwise clang-format makes it hard to read
	const Vector3i ftb_000 = L::face_to_block(0, 0, 0, direction, block_size_with_padding);
	const Vector3i ftb_x00 = L::face_to_block(1, 0, 0, direction, block_size_with_padding);
	const Vector3i ftb_0y0 = L::face_to_block(0, 1, 0, direction, block_size_with_padding);
	// How much to advance in the data array to get neighbor voxels, using face coordinates
	const int fn00 = get_zxy_index(ftb_000, block_size_with_padding);
	const int fn10 = get_zxy_index(ftb_x00, block_size_with_padding) - fn00;
	const int fn01 = get_zxy_index(ftb_0y0, block_size_with_padding) - fn00;
	const int fn11 = fn10 + fn01;
	const int fn21 = 2 * fn10 + fn01;
	const int fn22 = 2 * fn10 + 2 * fn01;
	const int fn12 = fn10 + 2 * fn01;
	const int fn20 = 2 * fn10;
	const int fn02 = 2 * fn01;

	
	voxelPosInfo cell_positions[13];
	
	const int fz = MIN_PADDING;

	// Iterating in face space
	for (int fy = min_fpos_y; fy < max_fpos_y; fy += 2) {
		for (int fx = min_fpos_x; fx < max_fpos_x; fx += 2) {


			// const VoxelBuffer &fvoxels = p_voxels;

			// Cell positions in block space
			// Warning: temporarily includes padding. It is undone later.
			cell_positions[0].v = L::face_to_block(fx, fy, fz, direction, block_size_with_padding);

			const int data_index = get_zxy_index(cell_positions[0].v, block_size_with_padding);


			
			const int isolevel = 128;
			{
				const bool s = srcData[data_index].w < isolevel;

				// `//` prevents clang-format from messing up
				if ( //
						(srcData[data_index + fn10].w < isolevel) == s && //
						(srcData[data_index + fn20].w < isolevel) == s && //
						(srcData[data_index + fn01].w < isolevel) == s && //
						(srcData[data_index + fn11].w < isolevel) == s && //
						(srcData[data_index + fn21].w < isolevel) == s && //
						(srcData[data_index + fn02].w < isolevel) == s && //
						(srcData[data_index + fn12].w < isolevel) == s && //
						(srcData[data_index + fn22].w < isolevel) == s) {
					// Not crossing the isolevel, this cell won't produce any geometry.
					// We must figure this out as fast as possible, because it will happen a lot.
					continue;
				}
			}
			


			std::array<unsigned int, 9> cell_data_indices;
			cell_data_indices[0] = data_index;
			cell_data_indices[1] = data_index + fn10;
			cell_data_indices[2] = data_index + fn20;
			cell_data_indices[3] = data_index + fn01;
			cell_data_indices[4] = data_index + fn11;
			cell_data_indices[5] = data_index + fn21;
			cell_data_indices[6] = data_index + fn02;
			cell_data_indices[7] = data_index + fn12;
			cell_data_indices[8] = data_index + fn22;

			//  6---7---8
			//  |   |   |
			//  3---4---5
			//  |   |   |
			//  0---1---2
			std::array<float, 13> cell_samples;
			// Full-resolution samples 0..8
			for (unsigned int i = 0; i < 9; ++i) {
				cell_samples[i] = sdf_as_float(srcData[cell_data_indices[i]]);//s8_to_snorm_noclamp(tos(srcData, VOXEL_SIZE, cell_positions[i].v));
			}

			//  B-------C
			//  |       |
			//  |       |
			//  |       |
			//  9-------A

			// Half-resolution samples 9..C: they are the same
			cell_samples[0x9] = cell_samples[0];
			cell_samples[0xA] = cell_samples[2];
			cell_samples[0xB] = cell_samples[6];
			cell_samples[0xC] = cell_samples[8];


			uint16_t case_code = sign_f(cell_samples[0]);
			case_code |= (sign_f(cell_samples[1]) << 1);
			case_code |= (sign_f(cell_samples[2]) << 2);
			case_code |= (sign_f(cell_samples[5]) << 3);
			case_code |= (sign_f(cell_samples[8]) << 4);
			case_code |= (sign_f(cell_samples[7]) << 5);
			case_code |= (sign_f(cell_samples[6]) << 6);
			case_code |= (sign_f(cell_samples[3]) << 7);
			case_code |= (sign_f(cell_samples[4]) << 8);

			if (case_code == 0 || case_code == 511) {
				// The cell contains no triangles.
				continue;
			}
			CRASH_COND(case_code > 511);

			vec3 cell_gradients[13];
			for (unsigned int i = 0; i < 9; ++i) {

				const unsigned int di = cell_data_indices[i];

				const float nx = sdf_as_float(srcData[di - n100]);
				const float ny = sdf_as_float(srcData[di - n010]);
				const float nz = sdf_as_float(srcData[di - n001]);
				const float px = sdf_as_float(srcData[di + n100]);
				const float py = sdf_as_float(srcData[di + n010]);
				const float pz = sdf_as_float(srcData[di + n001]);

				cell_gradients[i] = vec3(nx - px, ny - py, nz - pz);
			}
			cell_gradients[0x9] = cell_gradients[0];
			cell_gradients[0xA] = cell_gradients[2];
			cell_gradients[0xB] = cell_gradients[6];
			cell_gradients[0xC] = cell_gradients[8];

			// TODO Optimization: get rid of conditionals involved in face_to_block
			cell_positions[1].v = L::face_to_block(fx + 1, fy + 0, fz, direction, block_size_with_padding);
			cell_positions[2].v = L::face_to_block(fx + 2, fy + 0, fz, direction, block_size_with_padding);
			cell_positions[3].v = L::face_to_block(fx + 0, fy + 1, fz, direction, block_size_with_padding);
			cell_positions[4].v = L::face_to_block(fx + 1, fy + 1, fz, direction, block_size_with_padding);
			cell_positions[5].v = L::face_to_block(fx + 2, fy + 1, fz, direction, block_size_with_padding);
			cell_positions[6].v = L::face_to_block(fx + 0, fy + 2, fz, direction, block_size_with_padding);
			cell_positions[7].v = L::face_to_block(fx + 1, fy + 2, fz, direction, block_size_with_padding);
			cell_positions[8].v = L::face_to_block(fx + 2, fy + 2, fz, direction, block_size_with_padding);

			cell_positions[0x9] = cell_positions[0];
			cell_positions[0xA] = cell_positions[2];
			cell_positions[0xB] = cell_positions[6];
			cell_positions[0xC] = cell_positions[8];

			//填充真的信息
			for(int i = 0; i < 13; i++)
			{
				cell_positions[i].info = extractVoxel(srcData, VOXEL_SIZE, cell_positions[i].v);
			}
			for (unsigned int i = 0; i < 13; ++i) {
				cell_positions[i].v = (cell_positions[i].v - min_pos) << lodLevel;
			}
			const uint8_t cell_class = Transvoxel::get_transition_cell_class(case_code);

			CRASH_COND((cell_class & 0x7f) > 55);

			const Transvoxel::TransitionCellData cell_data = Transvoxel::get_transition_cell_data(cell_class & 0x7f);
			const bool flip_triangles = ((cell_class & 128) != 0);

			unsigned int vertex_count = cell_data.GetVertexCount();
			// FixedArray<int, 12> cell_vertex_indices(-1);
			int cell_vertex_indices[12];
			memset(cell_vertex_indices, -1, 12);
			CRASH_COND(vertex_count > 12);

			uint8_t direction_validity_mask = (fx > min_fpos_x ? 1 : 0) | ((fy > min_fpos_y ? 1 : 0) << 1);

			uint8_t cell_border_mask = get_border_mask(cell_positions[0].v, block_size_scaled);

			for (unsigned int vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {

				const uint16_t edge_code = Transvoxel::get_transition_vertex_data(case_code, vertex_index);
				const uint8_t index_vertex_a = (edge_code >> 4) & 0xf;
				const uint8_t index_vertex_b = (edge_code & 0xf);

				const float sample_a = cell_samples[index_vertex_a]; // d0 and d1 in the paper
				const float sample_b = cell_samples[index_vertex_b];
				// TODO Zero-division is not mentionned in the paper??
				// ERR_FAIL_COND(sample_a == sample_b);
				// ERR_FAIL_COND(sample_a == 0 && sample_b == 0);

				// Get interpolation position
				// We use an 8-bit fraction, allowing the new vertex to be located at one of 257 possible
				// positions  along  the  edge  when  both  endpoints  are included.
				//const int t = (sample_b << 8) / (sample_b - sample_a);
				const float t = sample_b / (sample_b - sample_a);

				const float t0 = t; //static_cast<float>(t) / 256.f;
				const float t1 = 1.f - t; //static_cast<float>(0x100 - t) / 256.f;
				// const int ti0 = t;
				// const int ti1 = 0x100 - t;

				vec3 testOffset = vec3(0, 0.0, 0);
				if (t > 0.f && t < 1.f) {
					// Vertex lies in the interior of the edge.
					// (i.e t is either 0 or 257, meaning it's either directly on vertex a or vertex b)

					uint8_t vertex_index_to_reuse_or_create = (edge_code >> 8) & 0xf;

					// The bit values 1 and 2 in this nibble indicate that we must subtract one from the x or y coordinate, respectively,
					// and these two bits are never simultaneously set. The bit value 4 indicates that a new vertex is to be created on an interior edge
					// where it cannot be reused, and the bit value 8 indicates that a new vertex is to be created on a  maximal  edge  where  it  can  be  reused.
					//
					// Bit 0 (0x1): need to subtract one to X
					// Bit 1 (0x2): need to subtract one to Y
					// Bit 2 (0x4): vertex is on an interior edge, won't be reused
					// Bit 3 (0x8): vertex is on a maximal edge, it can be reused
					uint8_t reuse_direction = (edge_code >> 12);
					
					// bool present = (reuse_direction & direction_validity_mask) == reuse_direction;
					//
					// if (present) {
					// 	// The previous cell is available. Retrieve the cached cell
					// 	// from which to retrieve the reused vertex index from.
					// 	const ReuseTransitionCell &prev = get_reuse_cell_2d(fx - (reuse_direction & 1), fy - ((reuse_direction >> 1) & 1));
					// 	// Reuse the vertex index from the previous cell.
					// 	cell_vertex_indices[i] = prev.vertices[vertex_index_to_reuse_or_create];
					// }

					if (1/*cell_vertex_indices[i] == -1*/) {
						// Going to create a new vertex

						voxelPosInfo p0 = cell_positions[index_vertex_a];
						voxelPosInfo p1 = cell_positions[index_vertex_b];

						const vec3 n0 = cell_gradients[index_vertex_a];
						const vec3 n1 = cell_gradients[index_vertex_b];

						//Vector3i primary = p0.v * ti0 + p1.v * ti1;
						vec3 primaryf = p0.v.to_vec3() * t0 + p1.v.to_vec3() * t1;//primary.to_vec3() * FIXED_FACTOR;
						vec3 normal = (n0 * t0 + n1 * t1).normalized();

						bool fullres_side = (index_vertex_a < 9 || index_vertex_b < 9);
						uint16_t border_mask = cell_border_mask;

						vec3 secondary;
						if (fullres_side) {

							secondary = get_secondary_position(primaryf, normal, 0, block_size_scaled);
							border_mask |= (get_border_mask(p0.v, block_size_scaled) & get_border_mask(p1.v, block_size_scaled)) << 6;

						} else {
							// If the vertex is on the half-res side (in our implementation, it's the side of the block),
							// then we make the mask 0 so that the vertex is never moved. We only move the full-res side to
							// connect with the regular mesh, which will also be moved by the same amount to fit the transition mesh.
							border_mask = 0;
						}

						cell_vertex_indices[vertex_index] = mesh->getVerticesSize();
						auto vdata = genVertexData(getVoxelVertex(primaryf, basePoint + testOffset,p0.info, p0.info));
						//vdata.m_pos = vec3(primaryf.x * BLOCK_SIZE, primaryf.y * BLOCK_SIZE, -1 * primaryf.z * BLOCK_SIZE) + basePoint;
						vdata.m_normal = (n0 * t0 + n1 * t1).normalized();
						vdata.m_color = vec4(5, 0, 0, 1);
						mesh->addVertex(vdata);
						// if (reuse_direction & 0x8) {
						// 	// The vertex can be re-used later
						// 	ReuseTransitionCell &r = get_reuse_cell_2d(fx, fy);
						// 	r.vertices[vertex_index_to_reuse_or_create] = cell_vertex_indices[i];
						// }
					}

				} 
				else {
					// The vertex is exactly on one of the edge endpoints.
					// Try to reuse corner vertex from a preceding cell.
					// Use the reuse information in transitionCornerData.

					const uint8_t cell_index = (t == 0 ? index_vertex_b : index_vertex_a);
					CRASH_COND(cell_index >= 13);
					uint8_t corner_data = Transvoxel::get_transition_corner_data(cell_index);
					uint8_t vertex_index_to_reuse_or_create = (corner_data & 0xf);
					uint8_t reuse_direction = ((corner_data >> 4) & 0xf);

					// bool present = (reuse_direction & direction_validity_mask) == reuse_direction;

					// if (present) {
					// 	// The previous cell is available. Retrieve the cached cell
					// 	// from which to retrieve the reused vertex index from.
					// 	const ReuseTransitionCell &prev = get_reuse_cell_2d(fx - (reuse_direction & 1), fy - ((reuse_direction >> 1) & 1));
					// 	// Reuse the vertex index from the previous cell.
					// 	cell_vertex_indices[i] = prev.vertices[vertex_index_to_reuse_or_create];
					// }

					if ( 1/*cell_vertex_indices[i] == -1*/) {
						// Going to create a new vertex

						voxelPosInfo primary = cell_positions[cell_index];
						vec3 primaryf = primary.v.to_vec3();
						vec3 normal = (cell_gradients[cell_index]).normalized();

						bool fullres_side = (cell_index < 9);
						uint16_t border_mask = cell_border_mask;

						vec3 secondary;
						if (fullres_side) {

							secondary = get_secondary_position(primaryf, normal, 0, block_size_scaled);
							border_mask |= get_border_mask(primary.v, block_size_scaled) << 6;

						} else {
							border_mask = 0;
						}

						cell_vertex_indices[vertex_index] = mesh->getVerticesSize();
						auto vdata = genVertexData(getVoxelVertex(primaryf, basePoint + testOffset, primary.info, primary.info));
						vdata.m_normal = cell_gradients[cell_index];
						//vdata.m_pos = vec3(primaryf.x * BLOCK_SIZE, primaryf.y * BLOCK_SIZE , -1 * primaryf.z * BLOCK_SIZE) + basePoint;
						vdata.m_color = vec4(5, 0, 0, 1);
						mesh->addVertex(vdata);

						// // We are on a corner so the vertex will be re-usable later
						// ReuseTransitionCell &r = get_reuse_cell_2d(fx, fy);
						// r.vertices[vertex_index_to_reuse_or_create] = cell_vertex_indices[i];
					}
				}

			} // for vertex

			unsigned int triangle_count = cell_data.GetTriangleCount();

			for (unsigned int ti = 0; ti < triangle_count; ++ti) 
			{
				if (!flip_triangles) //or double side for test
				{
					mesh->addIndex(cell_vertex_indices[cell_data.get_vertex_index(ti * 3)]);
					mesh->addIndex(cell_vertex_indices[cell_data.get_vertex_index(ti * 3 + 1)]);
					mesh->addIndex(cell_vertex_indices[cell_data.get_vertex_index(ti * 3 + 2)]);
				} 
				else 
				{
					mesh->addIndex(cell_vertex_indices[cell_data.get_vertex_index(ti * 3 + 2)]);
					mesh->addIndex(cell_vertex_indices[cell_data.get_vertex_index(ti * 3 + 1)]);
					mesh->addIndex(cell_vertex_indices[cell_data.get_vertex_index(ti * 3)]);
				}

			}

		} // for x
	} // for y
}

TransVoxel::TransVoxel()
{
	
}
} // namespace tzw
