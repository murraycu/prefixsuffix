EXTRA_DIST = index.html Makefile_web.am_fragment

include $(top_srcdir)/docs/Makefile_web.am_fragment
html_docs = index.html screenshot.png

post-html: 
	rsync $(rsync_args) -r $(html_docs) $$USER@shell.sourceforge.net:$(web_path_docs)

