#include "LineGrid.h"

namespace tzw {

LineGrid::LineGrid(int width, int depth)
{
	for(int i = 0; i <= width; i++)
	{
		append(vec3(i, 0 ,0),vec3(i, 0, depth));
	}
	for(int i = 0; i <= depth; i++)
	{
		append(vec3(0, 0, i),vec3(width, 0, i));
	}
	initBuffer();
}

} // namespace tzw
