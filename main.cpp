#include "Reader.hpp"
#include "Docit.hpp"

using vos::File;
using vos::RecordMD;
using vos::Record;
using vos::Reader;
using docit::Docit;
using docit::Docit2HTML;

int DOCIT_DEBUG = (getenv("DOCIT_DEBUG") == NULL)
			? 0
			: atoi(getenv("DOCIT_DEBUG"));
			
static Buffer	src_dir;
static Buffer	doc_dir;
static Docit	*doc_idx	= NULL;
static Docit	*doc_grp	= NULL;

static int docit_usage(const char *prog_name)
{
	fprintf(stderr, "Usage:\n"\
		" %s -src-dir [/path/to/source/dir] "\
		" -doc-dir [/path/to/doc/dir]\n", prog_name);

	return -1;
}

/**
 * @return	:
 *	< 0	: success.
 *	< <0	: fail.
 * @desc	: get source and document directory.
 */
static int docit_init(int argc, char *argv[])
{
	if (argc < 4) {
		return docit_usage(argv[0]);
	}

	register int i;

	for (i = 1; i < argc; ++i) {
		if (strcasecmp(argv[i], "-src-dir") == 0) {
			src_dir.init_raw(argv[++i], 0);
		} else if (strcasecmp(argv[i], "-doc-dir") == 0) {
			doc_dir.init_raw(argv[++i], 0);
		} else {
			return docit_usage(argv[0]);
		}
	}

	if (src_dir.is_empty() || doc_dir.is_empty())
		return docit_usage(argv[0]);

	if (DBG_LVL_IS_1) {
		fprintf(stderr, " >> src dir    : %s\n", src_dir._v);
		fprintf(stderr, " >> doc dir    : %s\n", doc_dir._v);
	}

	return 0;
}

/**
 * @return	:
 *	< 0	: success.
 *	< <0	: fail.
 * @desc	: get list of file to be parsed.
 */
static int docit_load_index_file()
{
	int		s;
	Reader		reader;
	Buffer		fidx;
	RecordMD	*rmd		= NULL;
	Record		*row		= NULL;
	Docit		*doc_idx_new	= NULL;

	s = fidx.concat(doc_dir._v, "/", DOCIT_FILE_IDX, NULL);
	if (s != 0) {
		return s;
	}

	if (DBG_LVL_IS_1) {
		fprintf(stderr, " >> file index : %s\n", fidx._v);
	}

	s = reader.open_ro(fidx._v);
	if (s != 0) {
		fprintf(stderr, " [DOCIT] Cannot open '%s'\n", fidx._v);
		goto err;
	}

	s = RecordMD::INIT(&rmd, DOCIT_MD);
	if (s != 0) {
		return s;
	}

	s = Record::INIT_ROW(&row, rmd->_n_md, Buffer::DFLT_SIZE);
	if (s != 0) {
		goto err;
	}

	if (DBG_LVL_IS_2) {
		rmd->dump();
	}

	while (1) {
		s = reader.read(row, rmd);
		if (s <= 0) {
			break;
		}

		s = Docit::INIT(&doc_idx_new, src_dir._v, doc_dir._v,
					row->get_column(0)->_v,
					row->get_column(1)->_v,
					row->get_column(2)->_v);
		if (s != 0) {
			break;
		}

		Docit::INSERT(&doc_idx, doc_idx_new);

		row->columns_reset();
	}
	s = 0;

err:
	if (DBG_LVL_IS_1) {
		if (doc_idx) {
			doc_idx->dump();
		}
	}
	if (row) {
		delete row;
	}
	if (rmd) {
		delete rmd;
	}

	return s;
}

static int docit_parsing()
{
	Docit *p = doc_idx;

	while (p) {
		if (p->_type == docit::DOCIT_IS_SRC) {
			p->grok_src();
		} else {
			p->grok_doc();
		}

		p = p->_next;
	}

	return 0;
}

static int docit_merge_content()
{
	int	s	= 0;
	Docit	*x	= doc_idx;

	while (x) {
		s = Docit::INSERT_GROUP(&doc_grp, x->_group, x->_content);
		if (s < 0)
			return s;
		/* group not found */
		if (s == 0) {
			x->_group = NULL;
		}
		x->_content = NULL;

		x = x->_next;
	}

	return 0;
}

static int docit_to_html()
{
	int		s	= 0;
	Buffer		fname;
	Docit2HTML	html;
	File		file;
	File		fcss;
	Docit		*p	= doc_grp;

	while (p) {
		if (!p->_path) {
			p->_path = new Buffer();
			if (!p->_path)
				return -1;
		} else {
			p->_path->reset();
		}

		s = p->_path->concat(doc_dir._v, "/", p->_group->_v, ".html",
					NULL);
		if (s != 0)
			return s;

		p->dump_html();

		p = p->_next;
	}

	/* create index.html */
	s = fname.concat(doc_dir._v, "/", DOCIT_INDEX_HTML, NULL);
	if (s < 0) {
		return s;
	}

	s = html.init(fname._v, "Index");
	if (s) {
		return s;
	}
	s = html.table_begin();
	if (s < 0) {
		return s;
	}

	p = doc_grp;
	while (p) {
		fname.reset();
		fname.concat("./", p->_group->_v, ".html", NULL);

		s = html.TOC_add_link(p->_group->_v, fname._v);
		if (s < 0) {
			break;
		}

		p = p->_next;
	}

	html.table_end();
	html.end();

	if (s < 0) {
		return s;
	}

	/* copy stylesheet, docit.css */
	fname.reset();
	fname.concat(doc_dir._v, "/", DOCIT_STYLE_CSS, NULL);

	s = file.open_ro(DOCIT_STYLE_CSS);
	if (s)
		return s;
	s = fcss.open_wo(fname._v);
	if (s)
		return s;

	while (file.read())
		fcss.write(&file);

	fcss.close();
	file.close();

	return 0;
} 

void docit_exit()
{
	if (doc_idx) {
		delete doc_idx;
	}
	if (doc_grp) {
		delete doc_grp;
	}
}

int main(int argc, char *argv[])
{
	int s;

	s = docit_init(argc, argv);
	if (s < 0)
		goto err;

	s = docit_load_index_file();
	if (s != 0) {
		goto err;
	}

	s = docit_parsing();
	if (s)
		goto err;

	s = docit_merge_content();
	if (s)
		goto err;

	if (DBG_LVL_IS_1) {
		doc_grp->dump();
	}

	s = docit_to_html();
err:
	docit_exit();
	if (s != 0) {
		if (errno)
			perror(NULL);
	}
	return s;
}
