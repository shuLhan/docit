#ifndef _DOCIT_CONTENT_HPP
#define	_DOCIT_CONTENT_HPP	1

#include "Buffer.hpp"
#include "main.hpp"

using vos::Buffer;

namespace docit {

enum _content_id {
	IS_NONE		= 0,
	IS_STRUCT,
	IS_FUNCTION,
	IS_CLASS,
	IS_METHOD,
	IS_ATTR,
	IS_DESC,
	IS_PARAM,
	IS_RETURN,
	IS_CODE,
	N_CONTENT
};

struct _content_type {
	int		_id;
	const char	*_name;
	const char	*_alias;
};
#define	CONTENT_IDX_NAME	1
#define	CONTENT_IDX_ALIAS	2

extern struct _content_type __content[N_CONTENT];

class Content {
public:
	Content();
	~Content();

	int init();
	void dump();

	static int GET_TYPE(const char *name);
	static void ADD(Content **cnt, Content *new_cnt);
	static void ADD_PARAM(Content **cnt, Content *par);
	static void ADD_RETURN(Content **cnt, Content *ret);

	int	_type;
	int	_n_param;
	int	_n_return;
	Buffer	*_name;
	Buffer	*_desc;
	Buffer	*_snip;
	Content	*_param;
	Content	*_return;
	Content	*_next;
private:
	Content(const Content&);
	void operator=(const Content&);
};

} /* namespace::docit */

#endif
