#include "Docit.hpp"

namespace docit {

enum _parsing_todo {
	GET_START		= 1,
	GET_TAG,
	GET_TAG_VALUE,
	GET_ATTR_START,
	GET_ATTR_NAME,
	GET_ATTR_DESC,
	GET_PARAM_START,
	GET_PARAM_NAME,
	GET_PARAM_DESC,
	GET_RETURN_START,
	GET_RETURN_NAME,
	GET_RETURN_DESC,
	GET_END
};

Docit::Docit() :
	_type(0),
	_group(NULL),
	_path(NULL),
	_content(NULL),
	_next(NULL)
{}

Docit::~Docit()
{
	if (_group) {
		delete _group;
	}
	if (_path) {
		delete _path;
	}
	if (_content) {
		delete _content;
	}
	if (_next) {
		delete _next;
	}
}

/**
 * @return	:
 *	< >=0	: tag type.
 *	< <0	: fail.
 */
static int parsing_get_tag(char **_cmnt, int *lines,
				int c_start, int c_end)
{
	int	c	= 0;
	int	type	= 0;
	char	*str	= NULL;
	char	*cmnt	= (*_cmnt);

	while (*cmnt) {
		if (*cmnt == '\n')
			(*lines)++;
		else if (*cmnt == c_start) {

			/* read until space or ':' */
			*cmnt++;
			str = cmnt;
			while (*cmnt && isalpha(*cmnt)) {
				*cmnt++;
			}

			c	= *cmnt;
			*cmnt	= '\0';

			type = Content::GET_TYPE(str);

			*cmnt = c;

			while (*cmnt && isspace(*cmnt)) {
				if (*cmnt == '\n') {
					(*lines)++;
				}
				*cmnt++;
			}

			if (*cmnt != c_end)
				return -1;

			*cmnt++;
			break;
		} else if (*cmnt == '*') {
			*cmnt++;
			if (*cmnt && *cmnt == '/') {
				(*_cmnt) = cmnt;
				return -1;
			}
		}
		*cmnt++;
	}

	(*_cmnt) = cmnt;

	return type;
}

/**
 * @return	:
 *	> 1	: end of comment.
 *	< 0	: success.
 *	< <0	: fail.
 */
static int parsing_get_tag_name(Buffer *name, char **_cmnt, int *lines,
				int c_start, int c_end)
{
	register char *cmnt = (*_cmnt);

	while (*cmnt) {
		if (*cmnt == '\n') {
			(*lines)++;
			*cmnt++;
		} else if (*cmnt == c_start) {
			*cmnt++;
			while (*cmnt && isspace(*cmnt)) {
				if (*cmnt == '\n') {
					(*lines)++;
				}
				*cmnt++;
			}
			while (*cmnt && isgraph(*cmnt)) {
				if (*cmnt == c_end)
					break;
				name->appendc(*cmnt);
				*cmnt++;
			}
			while (*cmnt && isspace(*cmnt)) {
				if (*cmnt == '\n') {
					(*lines)++;
				}
				*cmnt++;
			}
			if (*cmnt != c_end)
				return -1;
			*cmnt++;
			break;
		} else if (*cmnt == '*') {
			*cmnt++;
			if (*cmnt && *cmnt == '/') {
				(*_cmnt) = cmnt;
				return 1;
			}
		} else {
			*cmnt++;
		}
	}

	(*_cmnt) = cmnt;
	return 0;
}

/**
 * @return	:
 *	> >0	: end of comment.
 *	< 0	: success.
 *	< <0	: fail.
 */
static int parsing_get_tag_value(Buffer *bfr_cnt, char **_cmnt, int *lines,
					int c_end, int c_end2)
{
	char	last_ch = 0;
	char	*cmnt = (*_cmnt);

	while (*cmnt && isspace(*cmnt)) {
		if (*cmnt == '\n')
			(*lines)++;
		*cmnt++;
	}

	while (*cmnt) {
		if (*cmnt == c_end || *cmnt == c_end2) {
			if (last_ch == '*' || last_ch == '\n')
				break;
			bfr_cnt->appendc(*cmnt);
			*cmnt++;
		}
		if (*cmnt == '*') {
			last_ch = *cmnt;
			*cmnt++;
			if (*cmnt && *cmnt == '/') {
				(*_cmnt) = cmnt;
				return 1;
			}
			while (*cmnt && isspace(*cmnt)) {
				if (*cmnt == '\n') {
					(*lines)++;
					last_ch = *cmnt;
					bfr_cnt->appendc(*cmnt);
				}
				*cmnt++;
			}
		} else if (*cmnt == '\n') {
			(*lines)++;
			if (last_ch == '\n')
				bfr_cnt->appendc('\n');
			last_ch = *cmnt;
			*cmnt++;
		} else {
			if (*cmnt == '\t' || *cmnt == '\r'
			|| *cmnt == '\v' || *cmnt == '\b') {
				bfr_cnt->appendc(' ');
			} else {
				last_ch = *cmnt;
				bfr_cnt->appendc(*cmnt);
			}
			*cmnt++;
		}
	}

	(*_cmnt) = cmnt;
	return 0;
}

/**
 * @return	:
 *	> >0	: end of comment.
 *	< 0	: success.
 *	< <0	: fail.
 */
static int parsing_get_desc(Buffer *bfr, char **_cmnt, int *lines,
					int c_end, int c_end2)
{
	int	in_code = 0;
	char	last_c	= 0;
	char	*p = (*_cmnt);

	while (*p) {
		if (in_code) {
			bfr->appendc(*p);
			if (*p == '-') {
				last_c = *p;
				*p++;
				if (!*p) {
					break;
				}
				bfr->appendc(*p);

				if (*p == '/') {
					in_code = 0;
				}
			} else if (*p == '\n') {
				(*lines)++;
			}
		} else if (*p == c_end || *p == c_end2) {
			if (last_c == '*' || last_c == '\n')
				break;

			bfr->appendc(*p);
		} else if (*p == '*') {
			last_c = *p;
			*p++;
			if (!*p) {
				break;
			}

			if (*p == '/') {
				(*_cmnt) = p;
				return 1;
			}

			if (last_c != '\n') {
				bfr->appendc(*p);
			}
		} else if (*p == '\n') {
			(*lines)++;

			if (last_c == '\n') {
				bfr->appendc(*p);
			}
		} else if (*p == '/') {
			bfr->appendc(*p);
			last_c = *p;
			*p++;
			if (!*p) {
				break;
			}
			bfr->appendc(*p);

			if (*p == '-') {
				in_code = 1;
			}
		} else {
			bfr->appendc(*p);
		}
		last_c = *p;
		*p++;
	}

	(*_cmnt) = p;
	return 0;
}

/**
 * @return:
 *	< 0	: success.
 *	< <0	: fail.
 */
static int parsing_comment(Content **content, char *cmnt, int *lines)
{
	int	s		= 0;
	int	tag_type	= 0;
	int	todo		= GET_START;
	Content	*ls		= NULL;
	Content *cntn		= NULL;

	while (*cmnt) {
		switch (todo) {
		case GET_START:
			cntn = new Content();
			if (!cntn)
				return -1;

			s = cntn->init();
			if (s < 0) {
				delete cntn;
				return s;
			}

			todo = GET_TAG;
			break;

		case GET_TAG:
			tag_type = parsing_get_tag(&cmnt, lines, '@', ':');
			if (tag_type <= 0) {
				s = -1;
				goto out;
			}

			if (!cntn->_type)
				cntn->_type = tag_type;

			todo = GET_TAG_VALUE;
			break;

		case GET_TAG_VALUE:
			switch (tag_type) {
			case IS_STRUCT:
			case IS_FUNCTION:
			case IS_CLASS:
			case IS_METHOD:
				s = parsing_get_tag_value(cntn->_name, &cmnt,
								lines, '@',
								'@');
				if (s < 0) {
					goto out;
				}

				cntn->_name->trim();

				if (s > 0)
					todo = GET_END;
				else
					todo = GET_TAG;
				break;

			case IS_DESC:
				s = parsing_get_desc(cntn->_desc, &cmnt,
							lines, '@', '@');
				if (s < 0) {
					goto out;
				}
				if (s > 0) {
					todo = GET_END;
				} else {
					todo = GET_TAG;
				}
				break;

			case IS_ATTR:
				todo = GET_ATTR_START;
				break;

			case IS_PARAM:
				todo = GET_PARAM_START;
				break;

			case IS_RETURN:
				todo = GET_RETURN_START;
				break;
			}
			break;

		case GET_ATTR_START:
			ls = new Content();
			if (!ls) {
				s = -1;
				goto out;
			}
			s = ls->init();
			if (s) {
				goto out;
			}

			todo = GET_ATTR_NAME;
			break;

		case GET_ATTR_NAME:
			s = parsing_get_tag_name(ls->_name, &cmnt, lines,
							'-', ':');
			if (s < 0) {
				goto out;
			} else if (s > 0) {
				todo = GET_END;
			} else {
				todo = GET_ATTR_DESC;
			}
			break;

		case GET_ATTR_DESC:
			s = parsing_get_tag_value(ls->_desc, &cmnt, lines,
							'-', '@');
			if (s < 0) {
				goto out;
			}

			Content::ADD_PARAM(&cntn, ls);
			ls = NULL;

			if (s > 0)
				todo = GET_END;
			else {
				if (*cmnt == '@') {
					todo = GET_TAG;
				} else {
					todo = GET_ATTR_START;
				}
			}
			break;

		case GET_PARAM_START:
			ls = new Content();
			if (!ls) {
				s = -1;
				goto out;
			}
			s = ls->init();
			if (s) {
				goto out;
			}

			todo = GET_PARAM_NAME;
			break;

		case GET_PARAM_NAME:
			s = parsing_get_tag_name(ls->_name, &cmnt, lines,
							'>', ':');
			if (s < 0) {
				goto out;
			} else if (s > 0) {
				todo = GET_END;
			} else {
				todo = GET_PARAM_DESC;
			}
			break;

		case GET_PARAM_DESC:
			s = parsing_get_tag_value(ls->_desc, &cmnt, lines,
							'>', '@');
			if (s < 0) {
				goto out;
			}

			Content::ADD_PARAM(&cntn, ls);
			ls = NULL;

			if (s > 0)
				todo = GET_END;
			else {
				if (*cmnt == '@') {
					todo = GET_TAG;
				} else {
					todo = GET_PARAM_START;
				}
			}
			break;

		case GET_RETURN_START:
			ls = new Content();
			if (!ls) {
				s = -1;
				goto out;
			}

			s = ls->init();
			if (s != 0) {
				goto out;
			}

			todo = GET_RETURN_NAME;
			break;

		case GET_RETURN_NAME:
			if (DBG_LVL_IS_2) {
				printf(" >> get return name\n");
			}
			s = parsing_get_tag_name(ls->_name, &cmnt, lines,
							'<', ':');
			if (s < 0) {
				goto out;
			} else if (s > 0) {
				todo = GET_END;
			} else {
				todo = GET_RETURN_DESC;
			}
			break;

		case GET_RETURN_DESC:
			if (DBG_LVL_IS_2) {
				printf(" >> get return desc\n");
			}

			s = parsing_get_tag_value(ls->_desc, &cmnt, lines,
							'<', '@');
			if (s < 0) {
				goto out;
			}

			Content::ADD_RETURN(&cntn, ls);
			ls = NULL;

			if (s > 0) {
				todo = GET_END;
			} else {
				if (*cmnt == '@') {
					todo = GET_TAG;
				} else {
					todo = GET_RETURN_START;
				}
			}
			break;

		case GET_END:
			if (ls) {
				Content::ADD_RETURN(&cntn, ls);
				ls = NULL;
			}

			(*content) = cntn;
			s = 0;
			goto out;
		}
	}
out:
	if (s) {
		if (cntn)
			delete cntn;
		if (ls)
			delete ls;

		fprintf(stderr, "\n-------------------------------------\n");
		fprintf(stderr, "\n >> line %d : bad command syntax!\n", *lines);
	}
	return s;
}

/**
 * @return	:
 *	< 0	: success.
 *	< <0	: fail.
 */
static int parsing_get_code_snippet(Content *content, char **_src, int *lines)
{
	int	ocnt	= 0;
	int	bcnt	= 0;
	char	*src	= (*_src);
	Buffer	*snip	= NULL;

	while (*src && isspace(*src)) {
		if (*src == '\n')
			(*lines)++;
		*src++;
	}
	if (*src) {
		content->_snip = new Buffer();
		if (!content->_snip)
			return -1;
		snip = content->_snip;
	}
	while (*src) {
		if (*src == '\n') {
			(*lines)++;
			snip->appendc(*src);
		} else {
			if (*src == '(')
				++ocnt;
			else if (*src == '{')
				++bcnt;
			else if (*src == ')') {
				--ocnt;
				if (ocnt == 0) {
					if (content->_type == IS_FUNCTION
					||  content->_type == IS_METHOD)
						break;
				}
			} else if (*src == '}') {
				--bcnt;
				if (bcnt == 0) {
					if (content->_type == IS_STRUCT
					||  content->_type == IS_CLASS)
						break;
				}
			}
			snip->appendc(*src);
		}
		*src++;
	}
	if (*src) {
		snip->appendc(*src);
		*src++;
	}
	(*_src) = src;

	return 0;
}

/**
 * @return	:
 *	< 0	: success.
 *	< <0	: fail.
 */
int Docit::grok_src(const int get_code_snippet)
{
	int	s	= 0;
	int	end	= 0;
	int	lines	= 1;
	File	f;
	char	*c	= NULL;
	char	*start	= NULL;
	Content	*cntn	= NULL;

	printf(" >> processing : %s\n", _path->_v);

	s = f.open_ro(_path->_v);
	if (s != 0)
		return s;

	s = f.get_size();
	if (s < 0)
		return s;

	s = f.readn(s);
	if (s < 0)
		return s;

	c = f._v;
	while (*c) {
		if (*c == '\n') {
			lines++;
		}
		if (*c != '/') {
			*c++;
			continue;
		}

		start = c;
		*c++;
		if (!*c)
			break;
		if (*c != '*')
			continue;
		*c++;
		if (!*c)
			break;
		if (*c != '*') {
			continue;
		}

		*c++;
		end = 0;
		while (*c) {
			if (*c == '*') {
				*c++;
				if (*c && *c == '/')
					break;
			} else {
				*c++;
			}
		}

		*c++;
		end = *c;
		*c = '\0';
		s = parsing_comment(&cntn, start, &lines);
		if (s != 0) {
			if (cntn) {
				delete cntn;
			}
			goto out;
		}

		if (end) {
			*c = end;
		}

		if (get_code_snippet && cntn 
		&& (cntn->_type == IS_METHOD || cntn->_type == IS_FUNCTION
		||  cntn->_type == IS_STRUCT || cntn->_type == IS_CLASS)) {
			if (end) {
				parsing_get_code_snippet(cntn, &c, &lines);
			}
		}

		Content::ADD(&_content, cntn);
		cntn = NULL;

		if (!*c)
			break;
	}
out:
	return s;
}

int Docit::grok_doc()
{
	register int s = grok_src(0);
	dump();
	return s;
}

int Docit::dump_html()
{
	int		s;
	Docit2HTML	html;
	Content		*cntn	= NULL;
	Content		*ls	= NULL;

	s = html.init(_path->_v, _group->_v);
	if (s < 0)
		return s;

	/* writes TOC */
	s = html.TOC_begin();
	if (s < 0) {
		return s;
	}
	if (_content->_type == IS_CLASS) {
		html.TOC_add_item(__content[IS_CODE]._alias,
					__content[IS_CODE]._name);
		html.TOC_add_item(__content[IS_ATTR]._alias,
					__content[IS_ATTR]._name);
		html.TOC_add_item(__content[IS_DESC]._alias,
					__content[IS_DESC]._name);
		cntn = _content->_next;
	} else {
		cntn = _content;
	}

	while (cntn) {
		if (cntn->_name && cntn->_name->_v) {
			cntn->_name->trim();
		}
		s = html.TOC_add_item(cntn->_name->_v, cntn->_name->_v);
		if (s < 0) {
			return s;
		}
		cntn = cntn->_next;
	}

	s = html.TOC_end();
	if (s < 0) {
		return s;
	}

	/* write group description */
	if (_content->_type == IS_CLASS) {
		/* class code */
		s = html.add_subitem(__content[IS_CODE]._alias,
					__content[IS_CODE]._name);
		if (s < 0) {
			return s;
		}

		s = html.add_code(_content->_snip->_v);
		if (s < 0) {
			return s;
		}

		/* class attributes */
		s = html.add_subitem(__content[IS_ATTR]._alias,
					__content[IS_ATTR]._name);
		if (s < 0) {
			return s;
		}

		s = html.table_begin();
		if (s < 0) {
			return s;
		}
		cntn = _content->_param;
		while (cntn) {
			s = html.table_add_row(cntn->_name->_v, cntn->_desc->_v);
			if (s < 0) {
				return s;
			}
			cntn = cntn->_next;
		}
		s = html.table_end();
		if (s < 0) {
			return s;
		}

		/* class description */
		s = html.add_subitem(__content[IS_DESC]._alias,
					__content[IS_DESC]._name);
		if (s < 0) {
			return s;
		}

		s = html.add_desc(_content->_desc->_v);
		if (s < 0) {
			return s;
		}
	}

	/* write else */
	if (_content->_type == IS_CLASS)
		cntn = _content->_next;
	else
		cntn = _content;

	while (cntn) {
		s = html.add_item(cntn->_name->_v, cntn->_name->_v);
		if (s < 0) {
			return s;
		}

		s = html.table_begin();
		if (s < 0) {
			return s;
		}

		/* name */
		s = html.table_add_hrow(__content[cntn->_type]._alias,
					cntn->_name->_v,
					__content[cntn->_type]._name);
		if (s < 0) {
			return s;
		}

		/* code snippet */
		if (cntn->_snip && cntn->_snip->_v) {
			s = html.table_add_hrow(__content[IS_CODE]._alias,
						cntn->_snip->_v, "code");
			if (s < 0) {
				return s;
			}
		}
		/* function arguments */
		if (cntn->_param) {
			s = html.table_add_hrow_span(cntn->_n_param,
						__content[IS_PARAM]._alias,
						cntn->_param->_desc->_v,
						cntn->_param->_name->_v);
			if (s < 0) {
				return s;
			}

			ls = cntn->_param->_next;
			while (ls) {
				s = html.table_add_row(ls->_name->_v,
							ls->_desc->_v);
				if (s < 0) {
					return s;
				}

				ls = ls->_next;
			}
		}
		/* return values */
		if (cntn->_return) {
			s = html.table_add_hrow_span(cntn->_n_return,
						__content[IS_RETURN]._alias,
						cntn->_return->_desc->_v,
						cntn->_return->_name->_v);
			if (s < 0) {
				return s;
			}

			ls = cntn->_return->_next;
			while (ls) {
				s = html.table_add_row(ls->_name->_v,
							ls->_desc->_v);
				if (s < 0) {
					return s;
				}

				ls = ls->_next;
			}
		}
		/* description */
		if (cntn->_desc && cntn->_desc->_v) {
			s = html.table_add_hrow(__content[IS_DESC]._alias,
						cntn->_desc->_v,
						__content[IS_DESC]._name);
			if (s < 0) {
				return s;
			}
		}
		s = html.table_end();
		if (s < 0) {
			return s;
		}

		cntn = cntn->_next;
	}

	html.end();

	return 0;
}

void Docit::dump()
{
	Docit *p = this;

	while (p) {
		printf("%d|%s|%s\n", p->_type,
			p->_group ? p->_group->_v : "",
			p->_path ? p->_path->_v : "");
		if (_content) {
			_content->dump();
		}
		p = p->_next;
	}
}

/**
 * @return	:
 *	< 0	: success.
 *	< <0	: fail.
 */
int Docit::INIT(Docit **o, const char *srcd, const char *docd,
			const char *str_type, const char *group,
			const char *path)
{
	int s;

	(*o) = new Docit();
	if (! (*o)) {
		return -1;
	}

	if (strcasecmp(str_type, "src") == 0) {
		(*o)->_type = DOCIT_IS_SRC;
		s = Buffer::INIT_RAW(&(*o)->_path, srcd);
	} else if (strcasecmp(str_type, "doc") == 0) {
		(*o)->_type = DOCIT_IS_DOC;
		s = Buffer::INIT_RAW(&(*o)->_path, docd);
	} else {
		s = -1;
	}
	if (s != 0) {
		goto err;
	}

	s = Buffer::INIT_RAW(&(*o)->_group, group);
	if (s) {
		goto err;
	}

	s = (*o)->_path->concat("/", path, NULL);
	if (s) {
		goto err;
	}
	return 0;
err:
	delete ((*o));
	(*o) = NULL;

	return s;
}

void Docit::INSERT(Docit **head, Docit *node)
{
	Docit *p;

	if (!(*head)) {
		(*head) = node;
	} else {
		p = (*head);
		while (p->_next) {
			p = p->_next;
		}
		p->_next = node;
	}
}

void Docit::INSERT_CONTENT(Docit *group, Content *content)
{
	int	s	= 0;
	Content *pgc	= NULL;
	Content *next	= NULL;

	if (!group->_content) {
		group->_content = content;
		return;
	}

	while (content) {
		pgc = group->_content;
		while (pgc) {
			if (pgc->_type != content->_type)
				goto next;

			s = pgc->_name->cmp(content->_name);
			if (s)
				goto next;

			/* insert desc, snip, param, and return to
			 * parent group */
			pgc->_desc->append(content->_desc);
			pgc->_snip->append(content->_snip);

			if (pgc->_type == IS_FUNCTION
			||  pgc->_type == IS_METHOD) {
				Content::ADD_PARAM(&group->_content,
							content->_param);
				Content::ADD_RETURN(&group->_content,
							content->_return);

				content->_param	= NULL;
				content->_return= NULL;
			}
			break;
next:
			pgc = pgc->_next;
		}

		next		= content->_next;
		content->_next	= NULL;

		/* content is new */
		if (!pgc) {
			Content::ADD(&group->_content, content);
		} else {
			delete content;
		}
		content = next;
	}
}

/**
 * @desc	: insert a new group with name is 'name' and content is
 *                'content' into list of 'group'.
 * @return	:
 *	< 1	: success, group found.
 *	< 0	: success, no group found.
 *	< <0	: fail.
 */
int Docit::INSERT_GROUP(Docit **group, Buffer *name, Content *content)
{
	int	s	= 0;
	Docit	*p	= NULL;
	Docit	*last	= NULL;
	Docit	*newgrp	= NULL;

	p = (*group);
	while (p) {
		last = p;
		s = p->_group->like(name);
		if (s == 0) {
			INSERT_CONTENT(p, content);
			return 1;
		}

		p = p->_next;
	}

	newgrp = new Docit();
	if (!newgrp)
		return -1;

	newgrp->_group	= name;
	newgrp->_content= content;

	if (!last)
		(*group) = newgrp;
	else
		last->_next = newgrp;

	return 0;
}

} /* namespace::docit */
