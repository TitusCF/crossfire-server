/*
 * static char *rcsid_xbmtobdf_c =
 *   "$Id$";
 */

/*
 * xbmtobdf - converts many xbm bitmaps into one bdf font file.
 * Kjetil T. Homme 1992
 */

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#if !defined(vax) && !defined(ibm032) && !defined(__MACH__) && !defined(MACH)
#include <sys/time.h>
#endif
#include <sys/types.h>
#if (!defined (__STRICT_ANSI__) || defined (__sun__) || defined(sgi) || defined(__osf__)) && !defined(vax)
#if !defined(MACH) &&  !defined(NeXT) && !defined(__FreeBSD__)
#include <malloc.h>
#endif
#include <stdlib.h>
#endif

#if defined (__sun__) && defined (StupidSunHeaders)
#include <sunos.h>
#endif

/*
 * We fetch our own strtol-function from crosslib.a, since it
 * differs on so many systems.
 */
extern int strtol_local(char *str, char **ptr, int base);

static void convert_bitmap(char *, int);

#define NO_MAPPING -4711
#define INVALID	     -42

#define hexdigit(x) ((x) > 9 ? (x) + 'A' - 10 : (x) + '0')
static char *prog_name, *font_name = "crossfire", *xbm_dir = ".";
static int  font_size = 24, char_defs;
static int  use_bitmaps = 0;
static char *map_file = NULL;

static unsigned char mirror[256] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0, 
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8, 
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4, 
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc, 
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2, 
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa, 
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6, 
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe, 
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1, 
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9, 
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5, 
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd, 
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3, 
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb, 
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7, 
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff, 
};

enum { m_file_char, m_file_list } map_style = m_file_char;

static void usage() {
    fprintf(stderr, "Usage: %s [options] [-] [bitmaps]\n", prog_name);
    fprintf(stderr, "Options are:\t-f <mapfile>    maps filename to font position\n");
    fprintf(stderr, "\t\t-b   make bitmap file instead of bdf\n");
    fprintf(stderr, "\t\t-name <fontname>\n\t\t-d <directory>\n\t\t-size <fontsize>\n");
    exit(-1);
}

static char *basename_local(char *n) {
    char *t;
    return (t = strrchr(n, '/')) ? ++t : n;
}

static int get_char(FILE *fd) {
    int c;
    if ((c = fgetc(fd)) == EOF) {
	fprintf(stderr, "Fatal: Unexpected eof.\n");
	exit(2);
    }
    return c;
}

static int at_end(char *s, char *d) {
    return ! (strcmp(s + strlen(s) - strlen(d), d));
}

static int get_byte(FILE *fd) {
    int c;

    while (1) {
	c = get_char(fd);
	if (c == '0') {
	    if (get_char(fd) != 'x' || fscanf(fd, "%x", &c) == 0) {
		fprintf(stderr, "Fatal: Malformed hex-value.\n");
		exit(4);
	    }
	    return c;
	}
    }
}

static int count_bmaps (char *map_file) {
    FILE *fd;
    char buff[256], path[256];
    int nrofbmaps = 0, ch;
    
    if ((fd = fopen(map_file, "r")) == 0) {
	perror(map_file);
	exit(5);
    }

    while (!feof(fd)) {
	fgets(buff, sizeof buff, fd);
	if (sscanf(buff, "\\%d %s", &ch, path) == 2)
	    nrofbmaps++;
    }
    fclose (fd);
    return nrofbmaps - 1;
}

static void read_mappings(char *map_file) {
    FILE *fd;
    int more = 1, line = 0, ch;
    char path[256];
    char buff[256];
    
    if ((fd = fopen(map_file, "r")) == 0) {
	perror(map_file);
	exit(5);
    }

    while (more) {
	line++;
	fgets(buff, sizeof buff, fd);
	if (feof(fd)) {
	    more = 0;
	} else {
	    if (*buff == '#')
		continue;

	    if (sscanf(buff, "\\%d %s", &ch, path) != 2) {
		(void) fprintf(stderr, "%s: %d: Syntax error.\n\t%s\n",
			       map_file, line, buff);
	    }
	    convert_bitmap(path, (int)ch);
	}
    }
    (void) fclose(fd);
}


static void convert_bitmap(char *file_name, int ch_enc) {
    FILE *fd;
    int w = -1, h = -1, bw, defs, value, x, y;
    char buff[256], func[32], out[24*24], *ch_name;
    char *nextout;

    sprintf (buff, "%s/%s", xbm_dir, file_name);
    fd = fopen(buff, "r");
    if (!fd) {
	perror(buff);
	exit(1);
    }
    defs = 1;
    while (defs) {
	(void) fgets(buff, sizeof buff, fd);
	if (feof(fd)) {
	    fprintf(stderr, "%s: Fatal: Unexpected end of file\n", file_name);
	    exit(2);
	} else if (!strncmp(buff, "static char ", 12) ||
		!strncmp(buff,"static unsigned char ", 21)) {
	    /* This is the beginning of the data. */
	    defs = 0;
	} else if (sscanf(buff, "#define %s %d", func, &value) == 2) {
	    if (at_end(func, "hot")) {
		/* Ignored at this point of development */
		continue;
	    } else if (at_end(func, "width")) {
		w = value;
	    } else if (at_end(func, "height")) {
		h = value;
	    }
	} else {
	    fprintf(stderr, "%s: Fatal: Unknown format\n", file_name);
	    fprintf(stderr,"Line = `%s`\n", buff);
	    exit(2);
	}
    }

    if (w%8) {
	fprintf(stderr, "%s: Fatal: Illegal size %dx%d.\nThe width must be a multiple of 8 in this version.\n", file_name, w, h);
	exit(-4);
    }

    /*fprintf(stderr, "%s: size %dx%d.\n", file_name, w, h);*/

    if (!use_bitmaps) {
	ch_name = basename_local(file_name);
	
	(void) printf("STARTCHAR %s\nENCODING %d\n",
		      ch_name, ch_enc);
	/* Not sure about the following. */
	(void) printf("SWIDTH 1 0\nDWIDTH %d 0\nBBX %d %d 0 0\nBITMAP\n",
		      w, w, h);
	bw = (w + 7) / 8;

	for (y = 0; y < h; y++) {
	    nextout = out;
	    for (x = 0; x < bw; x++) {
		char nybble;
		
		value = get_byte(fd);

		value = mirror[value];
		nybble = (value & 0xF0) >> 4;
		*nextout++ = hexdigit(nybble);
		nybble = (value & 0x0F);
		*nextout++ = hexdigit(nybble);
	    }
	    *nextout = '\0';
	    printf("%s\n", out);
	}
	printf("ENDCHAR\n");

     } else { 
	 bw = (w + 7) / 8;

	 for (y = 0; y < h; y++) {
	     for (x = 0; x < bw; x++) {
		 value = get_byte(fd);
		 putchar (value);
	     }
	 }
     }
    (void) fclose(fd);
}


static void epilogue() {
    if (!use_bitmaps)
	printf("ENDFONT\n");
}


static void prologue() {
    time_t tloc;

    if (!use_bitmaps) {
	tloc = time(NULL);
	printf("STARTFONT 2.1\n");
	printf("COMMENT This font was assembled using xbmtobdf on %sFONT %s\n",
	       ctime(&tloc), font_name);
	printf("SIZE %d 75 75\nFONTBOUNDINGBOX 0 0 0 0\n", font_size);
	printf("STARTPROPERTIES 2\nFONT_DESCENT 0\nFONT_ASCENT %d\n",
	       font_size);
	printf("ENDPROPERTIES\nCHARS %d\n", char_defs);
    }
}

static void parse_args(int argc, char **argv) {
    int n, args = 1;

    prog_name = argv[0];
    if (argc == 1)
	usage();

    for (n = 1; n < argc; n++) {
	if (strcmp(argv[n], "-f") == 0) {
	    /*
	     * Is the filename missing or is this option given earlier?
	     */
	    if (n == argc || map_style == m_file_list)
		usage();
	    map_style = m_file_list;
	    map_file = argv[++n];
	    args += 2;
	} else if (strcmp(argv[n], "-size") == 0) {
	    if (n == argc)
		usage();
	    font_size = (int) atol(argv[++n]);
	    args += 2;
	} else if (strcmp(argv[n], "-name") == 0) {
	    if (n == argc)
		usage();
	    font_name = argv[++n];
	    args += 2;
	} else if (strcmp(argv[n], "-d") == 0) {
	    if (n == argc)
		usage();
	    xbm_dir = argv[++n];
	    args += 2;
	} else if (strcmp(argv[n], "-b") == 0) {
	    use_bitmaps++;
	    args += 1;
	} else if (strcmp(argv[n], "-") == 0) {
	    ++args;
	    break; /* Don't parse the rest of the arguments */
	} else if ((strcmp(argv[n], "-?") == 0) || 
		   (strcmp(argv[n], "-h") == 0)) {
	    usage();
	}
    }
}


int main(int argc, char **argv) {

    parse_args(argc, argv);
    char_defs = count_bmaps (map_file);
    prologue();
    read_mappings(map_file);
    epilogue();

    return 0;
}
