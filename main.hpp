#ifndef _DOCIT_MAIN_HPP
#define	_DOCIT_MAIN_HPP	1

extern int DOCIT_DEBUG;

#define	DBG_LVL_IS_1	(DOCIT_DEBUG == 1)
#define	DBG_LVL_IS_2	(DOCIT_DEBUG == 2)

#define	DOCIT_FILE_IDX		"index.docit"
#define	DOCIT_INDEX_HTML	"index.html"
#define	DOCIT_STYLE_CSS		"docit.css"
#define	DOCIT_MD		":type:::'|',:group:::'|',:filename:::"

#endif
