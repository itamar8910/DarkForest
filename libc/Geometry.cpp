#include "Geometry.h"

bool Rectangle::intersects(const Point& p) const
{
    return (   p.x >= x 
            && p.y >= y 
            && p.x <= static_cast<int>(x + width)
            && p.y <= static_cast<int>(y + height)
            );
}
