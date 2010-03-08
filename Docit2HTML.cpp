#include "Docit2HTML.hpp"

namespace docit {

Docit2HTML::Docit2HTML() : File()
{}

Docit2HTML::~Docit2HTML()
{
	if (_d) {
		end();
	}
}

int Docit2HTML::init(const char *fout, const char *title)
{
	register int s;

	s = open_wo(fout);
	if (s < 0) {
		return s;
	}

	s = writes(HTML_HEADER, title);
	if (s < 0) {
		return s;
	}

	s = writes(GROUP_NAME, title);
	if (s < 0) {
		return s;
	}

	return 0;
}

int Docit2HTML::TOC_begin()
{
	return append_raw(TOC_HEADER, 0);
}

int Docit2HTML::TOC_add_item(const char *name, const char *ref)
{
	return writes(TOC_ITEM, ref, name);
}

int Docit2HTML::TOC_add_link(const char *name, const char *ref)
{
	return writes(TOC_ITEM_LINK, ref, name);
}

int Docit2HTML::TOC_end()
{
	return append_raw(TOC_FOOTER, 0);
}

int Docit2HTML::add_item(const char *name, const char *ref)
{
	return writes(CONTENT_ITEM, ref, name);
}

int Docit2HTML::add_subitem(const char *name, const char *ref)
{
	return writes(CONTENT_SUBITEM, ref, name);
}

int Docit2HTML::add_code(const char *code)
{
	return writes(CONTENT_CODE, code);
}

int Docit2HTML::add_desc(char *desc)
{
	if (!desc)
		return 0;

	const char	*b;
	char		*e = NULL;
	char		t;

	b = desc;
	while (*desc) {
		if (*desc == '\n') {
			e = (char *) desc;
			*desc++;

			t	= *e;
			*e	= '\0';

			append_raw(b, 0);
			add_newline();

			*e	= t;
			b	= desc;
			e	= NULL;
		} else if (*desc == '/') {
			e = desc;

			*desc++;
			if (!*desc) {
				break;
			}
			if (*desc != '-') {
				continue;
			}

			if (b != e) {
				t	= *e;
				*e	= '\0';
				append_raw(b, 0);
				*e	= t;
				e	= NULL;
			}
			*desc++;
			b = desc;
			while (*desc) {
				if (*desc == '-') {
					e = desc;
					*desc++;
					if (!*desc) {
						continue;
					}
					if (*desc != '/') {
						continue;
					}

					if (b != e) {
						t	= *e;
						*e	= '\0';
						add_code(b);
						*e	= t;
						e	= NULL;
					}

					*desc++;
					b = desc;
					break;
				} else {
					*desc++;
				}
			}
		} else {
			*desc++;
		}
	}

	append_raw(b, 0);

	return writes(CONTENT_DESC, desc);
}

int Docit2HTML::add_newline()
{
	return append_raw(CONTENT_NEWLINE, 0);
}

int Docit2HTML::table_begin()
{
	return append_raw(TABLE_HEADER, 0);
}

int Docit2HTML::table_add_row(const char *data, const char *data2)
{
	return writes(TABLE_ROW, data, data2);
}

int Docit2HTML::table_add_hrow(const char *header, const char *data,
				const char *ref)
{
	register int s;

	s = writes(TABLE_HEADER_ROW_BEGIN, header, ref);
	if (s < 0)
		return s;

	s = add_desc((char *) data);
	if (s < 0)
		return s;

	s = append_raw(TABLE_HEADER_ROW_END, 0);

	return s;
}

int Docit2HTML::table_add_hrow_span(const int ncol, const char *header,
					const char *data, const char *ref)
{
	return writes(TABLE_HEADER_ROW_SPAN, ncol, header, ref, data);
}

int Docit2HTML::table_end()
{
	return append_raw(TABLE_FOOTER, 0);
}

void Docit2HTML::end()
{
	append_raw(HTML_FOOTER, 0);
	close();
}

} /* namespace::docit */
