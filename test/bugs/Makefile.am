CROSSEDIT = @CROSSEDIT@

SUBDIRS = bugtrack

CHECK_ROOT=../
CHECK_FOLDER=bugs
CHECK_PARENT_NAME=Main

include $(top_srcdir)/check-fragment.am

clean-local:
	$(RM) .autorun.xml

check-local: check-html
clean-local: clean-unit
