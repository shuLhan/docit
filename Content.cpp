#include "Content.hpp"

namespace docit {

struct _content_type __content[N_CONTENT] = {
	{ IS_NONE	, ""		, "" },
	{ IS_STRUCT	, "struct"	, "struct" },
	{ IS_FUNCTION	, "function"	, "function" },
	{ IS_CLASS	, "class"	, "class" },
	{ IS_METHOD	, "method"	, "method" },
	{ IS_ATTR	, "attr"	, "attributes" },
	{ IS_DESC	, "desc"	, "description" },
	{ IS_PARAM	, "param"	, "parameter" },
	{ IS_RETURN	, "return"	, "return value" },
	{ IS_CODE	, "code"	, "code" }
};

Content::Content() :
	_type(0),
	_n_param(0),
	_n_return(0),
	_name(NULL),
	_desc(NULL),
	_snip(NULL),
	_param(NULL),
	_return(NULL),
	_next(NULL)
{}

Content::~Content()
{
	if (_name)
		delete _name;
	if (_desc)
		delete _desc;
	if (_snip)
		delete _snip;
	if (_param)
		delete _param;
	if (_return)
		delete _return;
	if (_next)
		delete _next;
}

/**
 * @return	:
 *	< 0	: success.
 *	< <0	: fail.
 */
int Content::init()
{
	_name = new Buffer();
	if (!_name)
		return -1;

	_desc = new Buffer();
	if (!_desc)
		return -1;

	return 0;
}

void Content::dump()
{
	Content *cnt	= this;
	Content *p	= NULL;

	while (cnt) {
		printf("\n---------------------------------------------------\n");
		printf(" %d : [%s]\n", cnt->_type, cnt->_name ?
							cnt->_name->_v : "");
		printf(" desc : %s\n", cnt->_desc ? cnt->_desc->_v : "");
		printf(" snip : %s\n", cnt->_snip ? cnt->_snip->_v : "");

		p = cnt->_param;
		while (p) {
			printf("\t> %s\t: %s\n", p->_name->_v, p->_desc->_v);
			p = p->_next;
		}

		if (cnt->_return)
			printf("\n");
		p = cnt->_return;
		while (p) {
			printf("\t< %s\t: %s\n", p->_name->_v, p->_desc->_v);
			p = p->_next;
		}

		cnt = cnt->_next;
	}
}

/**
 * @return	:
 *	< >0	: success.
 *	< 0	: fail.
 */
int Content::GET_TYPE(const char *name)
{
	register int s = 0;
	register int i = 0;

	for (; i < N_CONTENT; ++i) {
		s = strcasecmp(name, __content[i]._name);
		if (s == 0)
			return __content[i]._id;
	}

	return 0;
}

void Content::ADD(Content **cnt, Content *new_cnt)
{
	Content *p = NULL;

	if (!(*cnt))
		(*cnt) = new_cnt;
	else {
		p = (*cnt);
		while (p->_next) {
			p = p->_next;
		}
		p->_next = new_cnt;
	}
}

void Content::ADD_PARAM(Content **cnt, Content *par)
{
	if (!(*cnt)) {
		(*cnt) = par;
	} else {
		ADD(&(*cnt)->_param, par);
	}
	(*cnt)->_n_param++;
}

void Content::ADD_RETURN(Content **cnt, Content *ret)
{
	if (!(*cnt)) {
		(*cnt) = ret;
	} else {
		ADD(&(*cnt)->_return, ret);
	}
	(*cnt)->_n_return++;
}

} /* namespace::docit */
