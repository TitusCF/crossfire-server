#!/bin/bash

#
# Encodes the txt files into PostScript documents using enscript.
#
# Author: Daniel Hawkins
#

# Check for enscript
# If enscript version check returned 0, we have enscript.
if enscript --version
then
	enscript -2 -b "Crossfire Spells" -p "spell-list.ps" -f "ZapfChancery-MediumItalic12" -F "ZapfChancery-MediumItalic13" spell-list.txt
	enscript -2 -b "Crossfire Prayers" -p "prayer-list.ps" -f "ZapfChancery-MediumItalic12" -F "ZapfChancery-MediumItalic13" prayer-list.txt
	enscript -B -p "spell-info.ps" -f "Courier-BoldOblique12" --margins=40:40:30:30 spell-info.txt
        enscript -B -p "tome-of-magic.ps" -f "Palatino-Italic12" -T 14 --margins=40:14:30:30 tome-of-magic.txt
	enscript -B -p "spell-summary.ps" -f "Courier8" --margins=40:18:34:30 spell-summary.txt
else
	echo "Enscript not found, PostScript generation will be skipped.";
fi
