#ifndef _DOCIT_MAPLIST_HPP
#define	_DOCIT_MAPLIST_HPP	1

#include "Buffer.hpp"

using vos::Buffer;

namespace docit {

class MapList {
public:
	MapList();
	~MapList();

	Buffer	*_key;
	Buffer	*_val;
	MapList	*_next;
private:
	DISALLOW_COPY_AND_ASSIGN(MapList);
};

} /* namespace::docit */

#endif
