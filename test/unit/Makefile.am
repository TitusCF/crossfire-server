CROSSEDIT = @CROSSEDIT@

SUBDIRS = common random_maps socket server $(CROSSEDIT)


CHECK_ROOT=../
CHECK_FOLDER=unit
CHECK_PARENT_NAME=Main
include $(top_srcdir)/check-fragment.am
check-local: check-html
clean-local: clean-unit