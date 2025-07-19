#include "Skull.hpp"

Skull::Skull()
{
}

Skull::Skull(cv::Point const& _pos, int _area, SkullType _skullType) :
	pos{ _pos }, area{ _area }, skullType{ _skullType }
{
}
