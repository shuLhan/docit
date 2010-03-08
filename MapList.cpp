#include "MapList.hpp"

namespace docit {

MapList::MapList() :
	_key(NULL),
	_val(NULL),
	_next(NULL)
{}

MapList::~MapList()
{
	if (_key)
		delete _key;
	if (_val)
		delete _val;
	if (_next)
		delete _next;
}

} /* namespace::docit */
