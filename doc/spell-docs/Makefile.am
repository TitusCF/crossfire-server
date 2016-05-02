EXTRA_DIST = README

PERL=@PERL@
CROSSBIN=../../server/crossfire-server

CLEANFILES=spell-list.txt spell-list.ps prayer-list.txt prayer-list.ps tome-of-magic.ps \
	    spell-summary.ps spell-info.ps

spell-docs: spell-info.txt tome-of-magic.txt prayer-list.txt spell-list.txt
	$(SHELL) create_ps.sh

spell-list.txt prayer-list.txt: spell-lists.pl
	$(PERL) spell-lists.pl "$(CROSSBIN) -m4" spell-list.txt prayer-list.txt
