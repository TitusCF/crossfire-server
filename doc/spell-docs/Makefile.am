EXTRA_DIST = README spell-summary.ps

PERL=@PERL@
CROSSBIN=../../server/crossfire-server

CLEANFILES=spell-list.txt spell-list.ps prayer-list.txt prayer-list.ps

spell-docs: spell-info.txt tome-of-magic.txt prayer-list.txt spell-list.txt
	$(SHELL) create_ps.sh

spell-list.txt prayer-list.txt: spell-lists.pl
	$(PERL) spell-lists.pl "$(CROSSBIN) -m4" spell-list.txt prayer-list.txt
