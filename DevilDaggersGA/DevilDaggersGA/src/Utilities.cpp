#include "Utilities.hpp"

bool operator==(DXCam::Region const& a, DXCam::Region const& b)
{
	return (a.top == b.top) && (a.left == b.left) && (a.bottom == b.bottom) && (a.right == b.right);
}

bool operator!=(DXCam::Region const& a, DXCam::Region const& b)
{
	return !(a == b);
}
