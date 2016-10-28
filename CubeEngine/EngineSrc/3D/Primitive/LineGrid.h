#ifndef TZW_LINEGRID_H
#define TZW_LINEGRID_H
#include "LinePrimitive.h"

namespace tzw {

class LineGrid : public LinePrimitive
{
public:
	LineGrid(int width, int depth);
};

} // namespace tzw

#endif // TZW_LINEGRID_H
