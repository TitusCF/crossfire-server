
SUBDIRS = toolkit unit

clean-local:
	rm -rf logs

CHECK_ROOT=./
CHECK_FOLDER=.
CHECK_PARENT_NAME=Site index
include $(top_srcdir)/check-fragment.am
check-local: check-html check-text
clean-local: clean-unit