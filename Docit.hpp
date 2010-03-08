#ifndef _DOCIT_HPP
#define	_DOCIT_HPP	1

#include "Docit2HTML.hpp"
#include "Content.hpp"

namespace docit {

enum _docit_type {
	DOCIT_IS_SRC	= 1,
	DOCIT_IS_DOC	= 2
};

class Docit {
public:
	Docit();
	~Docit();

	int grok_src(const int get_code_snippet = 1);
	int grok_doc();
	int dump_html();
	void dump();

	static int INIT(Docit **o, const char *srcd, const char *docd,
				const char *str_type, const char *group,
				const char *path);
	static void INSERT(Docit **head, Docit *node);
	static void INSERT_CONTENT(Docit *group, Content *content);
	static int INSERT_GROUP(Docit **group, Buffer *name,
				Content *content);

	int	_type;
	Buffer	*_group;
	Buffer	*_path;
	Content	*_content;
	Docit	*_next;
private:
	Docit(const Docit&);
	void operator=(const Docit&);
};

} /* namespace::docit */

#endif
