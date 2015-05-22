#arch2xml - Todd Mitchell - work in progress - use if you like
#usage : python arch2xml.py <directory> <xml file name>
#FYI - the Walk function is very useful for other python programs as well

import fnmatch, os, string

#Flexable Directory Walker from the Python CookBook

def Walk( root, recurse=0, pattern='*', return_folders=0 ):


	# initialize
	result = []

	# must have at least root folder
	try:
		names = os.listdir(root)
	except os.error:
		return result

	# expand pattern
	pattern = pattern or '*'
	pat_list = string.splitfields( pattern , ';' )

	# check each file
	for name in names:
		fullname = os.path.normpath(os.path.join(root, name))

		# grab if it matches our pattern and entry type
		for pat in pat_list:
			if fnmatch.fnmatch(name, pat):
				if os.path.isfile(fullname) or (return_folders and os.path.isdir(fullname)):
					result.append(fullname)
				continue

		# recursively scan other folders, appending results
		if recurse:
			if os.path.isdir(fullname) and not os.path.islink(fullname):
				result = result + Walk( fullname, recurse, pattern, return_folders )

	return result

	
#
# Function escape(string)
#
# Creates HTML/XML escaped characters for display in a browser to work correctly
#
# @param esc_me
# The string to escape characters in.
#
# @return
# The equivalent string with escaped versions of important characters.
#
# @todo
# Add more escaped characters as the need arises
#
def escape(esc_me):
    # Converting ampersand must be first, since escaping other characters generates ampersands.
    esc_me = esc_me.replace('&', '&amp;', 4)
    esc_me = esc_me.replace('<', '&lt;', 3)
    esc_me = esc_me.replace('>', '&gt;', 3)
    return esc_me


def arch2xml(root,filename,xsl_file='cfarches.xsl'):
    files = Walk(root, 1, '*.arc', 1)
    print 'searching for arch files in %s' %root
    xml = open(filename,'w')
    xml.write('<?xml version="1.0"?>\n<?xml-stylesheet type="text/xsl" href="%s"?>\n<ARCHES>'%xsl_file)
    for file in files:
            arc = open(file,'r')
            contents = arc.read().split('\n')
            xml.write('<arch>\n')
            mess = 0
            # Need_new_arch is set to 1 when an arch end is found in the middle of a file,
            # otherwise it is set to 0.
            need_new_arch = 0
            for line in contents:
                    xp = line.split()
                    if mess == 1 and len(xp)>1:
                            str = escape(string.join(xp[0:]))
                            xml.write('%s\n' %str)
                    elif len(xp) == 1:
                            tag = string.lower(xp[0])
                            # if an empty comment line, ignore it
                            if tag == '#':
                                continue
                            if tag == 'end':
                                    tag = '     <END />'
                                    # We reached the end of the arch
                                    need_new_arch = 1
                            elif tag == 'more':
                                    tag = '     <MORE />'
                                    # A quick hack to ensure the arch tags
                                    # don't split up separate parts of the
                                    # same archetype.
                                    need_new_arch = 0
                            elif tag =='msg':
                                    tag = '     <message>'
                                    mess = 1
                            elif tag =='endmsg':
                                    tag = '     </message>'
                                    mess = 0
                            elif tag == 'anim':
                                    tag = '     <anim>'
                            elif tag =='mina':
                                    tag = '     </anim>'
                            else:
                                    tag = '[%s]'%(tag)
                            xml.write('%s\n' %(tag))
                    elif len(xp)>1:
			    
                            tag = string.lower(xp[0])
                            if (tag[0] == "#"):
                                str = string.join(xp)[1:]
                                xml.write('     <comment>%s</comment>\n' %(escape(str)))
                            else:
                                # A quick and dirty hack to make multiple independent arches
                                # in a single .arc file each get their own arch tag.
                                #
                                # TODO: Make a couple .arc files that consist entirely
                                # of comments not receive their own arch tag.
                                #
                                # -- SilverNexus 2015-05-21
                                #
                                if need_new_arch == 1:
                                    xml.write('\n</arch>\n<arch>\n')
                                    need_new_arch = 0

                                str = string.join(xp[1:])
                                xml.write('     <%s>%s</%s>\n' %(tag,str,tag))
            xml.write('\n</arch>\n')
            arc.close()
    xml.write('\n</ARCHES>')
    xml.close()
    print "DONE"

if __name__ == '__main__':
    import sys
    if len(sys.argv) < 3:
        sys.stderr.write ('Converts arc files in a directory and all sub directories to an xml file\nUsage: arch2xml.py <directory> <XML-filename>\n')
        sys.exit()
    else:
        arch2xml(sys.argv[1],sys.argv[2])
