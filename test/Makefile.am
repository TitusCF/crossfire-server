CHECK_ROOT=./
CHECK_FOLDER=.
CHECK_PARENT_NAME=Site index

include $(top_srcdir)/check-fragment.am

SUBDIRS = include templates toolkit unit bugs

clean-local:
	$(RM) -rf logs
	$(RM) .autorun.xml

check-local: check-html check-text
clean-local: clean-unit
