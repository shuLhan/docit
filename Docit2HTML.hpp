#ifndef _DOCIT_2_HTML_HPP
#define	_DOCIT_2_HTML_HPP	1

#include "File.hpp"

namespace docit {

using vos::File;

#define	HTML_HEADER					\
	"<html>\n"					\
	"<head>\n"					\
		"<title>%s</title>\n"			\
		"<style type=\"text/css\">\n"		\
			"@import url(docit.css);\n"	\
		"</style>\n"				\
	"</head>\n"					\
	"<body>\n"

#define	GROUP_NAME	"<h1>%s</h1>\n"

#define	TOC_HEADER	"<hr />\n<ul>\n"
#define	TOC_ITEM	"<li><a href=\"#%s\">%s</a></li>\n"
#define	TOC_ITEM_LINK	"<li><a href=\"%s\">%s</a></li>\n"
#define	TOC_FOOTER	"</ul>\n<hr />\n"

#define	CONTENT_NEWLINE	"\n<br/><br/>\n"
#define	CONTENT_ITEM	"<h2><a name=\"%s\">%s</a></h2>\n"
#define	CONTENT_SUBITEM	"<h3><a name=\"%s\">%s</a></h2>\n"
#define	CONTENT_CODE	"<pre>%s</pre>\n"
#define	CONTENT_DESC	"<p>%s</p>\n"

#define	TABLE_HEADER	"<table>"

#define	TABLE_HEADER_ROW_BEGIN					\
	"<tr>\n"						\
	"	<th>%s</th>\n"					\
	"	<td colspan=\"2\" class=\"%s\">\n"

#define	TABLE_HEADER_ROW_END	"</td>\n</tr>\n"

#define	TABLE_HEADER_ROW_SPAN					\
	"<tr>\n"						\
	"	<th rowspan=\"%d\">%s</th>\n"			\
	"	<td><span class=\"code\">%s</span></td>\n"	\
	"	<td>%s</td>\n"					\
	"</tr>\n"

#define	TABLE_ROW						\
	"<tr>\n"						\
	"	<td><span class=\"code\">%s</span></td>\n"	\
	"	<td>%s</td>\n"					\
	"</tr>\n"

#define	TABLE_FOOTER	"</table>"

#define	HTML_FOOTER		\
	"</body>\n"		\
	"</html>\n"

class Docit2HTML : public File {
public:
	Docit2HTML();
	~Docit2HTML();

	int init(const char *fout, const char *title);
	int TOC_begin();
	int TOC_add_item(const char *name, const char *ref);
	int TOC_add_link(const char *name, const char *ref);
	int TOC_end();
	int add_item(const char *name, const char *ref);
	int add_subitem(const char *name, const char *ref);
	int add_code(const char *code);
	int add_desc(char *desc);
	int add_newline();

	int table_begin();
	int table_add_row(const char *data, const char *data2);
	int table_add_hrow(const char *header, const char *data,
				const char *ref);
	int table_add_hrow_span(const int ncol, const char *header,
				const char *data, const char *ref);
	int table_end();
	void end();
private:
	Docit2HTML(const Docit2HTML&);
	void operator=(const Docit2HTML&);
};

} /* namespace::docit */

#endif
