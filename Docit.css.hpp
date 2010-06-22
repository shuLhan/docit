#ifndef _DOCIT_CSS_HPP
#define	_DOCIT_CSS_HPP	1

namespace docit {

#define	CSS \
"body { \n"\
"	font-family		: serif;\n"\
"	background-color	: white;\n"\
"	color			: black;\n"\
"	width			: 760px;\n"\
"	margin			: 10px auto 10px auto;\n"\
"	padding			: 8px;\n"\
"	border			: solid black 1px;\n"\
"}\n"\
"\n"\
"h1, h2 {\n"\
"	background-color	: black;\n"\
"	color			: white;\n"\
"	border-bottom		: 2px solid;\n"\
"	border-color		: gray;\n"\
"	padding			: 4px 0px 4px 16px;\n"\
"}\n"\
"\n"\
"h2 {\n"\
"	font-size		: 16px;\n"\
"}\n"\
"\n"\
"h3 {\n"\
"	background-color	: gray;\n"\
"	color			: white;\n"\
"	border-bottom		: 2px solid;\n"\
"	border-color		: silver;\n"\
"	padding			: 4px 0px 4px 16px;\n"\
"}\n"\
"\n"\
"h2 {\n"\
"	margin-top	: 2em;\n"\
"}\n"\
"\n"\
"table {\n"\
"	margin		: 8px 0px 8px 0px;\n"\
"	border-collapse	: collapse;\n"\
"}\n"\
"\n"\
"th {\n"\
"	background-color	: silver;\n"\
"	text-align		: left;\n"\
"	vertical-align		: text-top;\n"\
"	padding			: 4px;\n"\
"	border			: 1px solid gray;\n"\
"	width			: auto;\n"\
"}\n"\
"\n"\
"td {\n"\
"	padding		: 8px;\n"\
"	vertical-align	: top;\n"\
"	border		: 1px solid gray;\n"\
"}\n"\
"\n"\
"a {\n"\
"	text-decoration	: none;\n"\
"}\n"\
"\n"\
"pre {\n"\
"	background-color	: silver;\n"\
"	font-family		: monospace;\n"\
"	border			: 1px solid;\n"\
"	border-color		: gray;\n"\
"	padding			: 4px 4px 4px 16px;\n"\
"}\n"\
"\n"\
".code {\n"\
"	white-space	: pre;\n"\
"	font-family	: monospace;\n"\
"}"

} /* namespace::docit */

#endif
