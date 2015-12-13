EXTRA_DIST = README spell-info.ps spell-summary.ps \
	tome-of-magic.ps spell-info.txt	tome-of-magic.txt

PERL=@PERL@
CROSSBIN=../../server/crossfire-server

CLEANFILES=spell-list.txt spell-list.ps prayer-list.txt prayer-list.ps

spell-docs:
	$(PERL) spell-lists.pl "$(CROSSBIN) -m4" spell-list prayer-list
