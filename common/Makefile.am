noinst_LIBRARIES = libcross.a

libcross_a_SOURCES = \
	anim.c \
	arch.cpp\
	arch_types_valid.c \
	artifact.c\
	assets/AnimationWriter.cpp \
	assets/Animations.cpp \
	assets/ArchetypeLoader.cpp \
	assets/ArchetypeWriter.cpp \
	assets/Archetypes.cpp \
	assets/ArtifactWriter.cpp \
	assets/AssetCollector.cpp \
	assets/AssetsManager.cpp \
	assets/FaceLoader.cpp \
	assets/FaceWriter.cpp \
	assets/Faces.cpp \
	assets/FacesetLoader.cpp \
	assets/FacesetWriter.cpp \
	assets/Facesets.cpp \
	assets/FormulaeWriter.cpp \
	assets/MessageLoader.cpp \
	assets/MessageWriter.cpp \
	assets/Messages.cpp \
	assets/microtar.c \
	assets/PngLoader.cpp \
	assets/QuestLoader.cpp \
	assets/QuestWriter.cpp \
	assets/Quests.cpp \
	assets/TarLoader.cpp \
	assets/TreasureLoader.cpp \
	assets/TreasureWriter.cpp \
	assets/Treasures.cpp \
	assets/Utils.cpp \
	assets/assets.cpp\
	bufferreader.c \
	button.c\
	cf_random.c \
	dialog.c \
	events.cpp \
	exp.c\
	friend.c\
	holy.c\
	info.c\
	image.c \
	init.c\
	item.c\
	languages.c\
	links.c\
	living.c\
	loader.l\
	logger.c\
	los.c\
	map.c\
	ob_methods.c \
	ob_types.c \
	object.c\
	output_file.c \
	path.c\
	porting.c\
	player.c\
	re-cmp.c\
	readable.c\
	recipe.c\
	region.c\
	shstr.c\
	stringbuffer.c\
	time.c\
	treasure.c\
	utils.c

EXTRA_DIST = README is_valid_types_gen.pl
AM_CPPFLAGS = -I$(top_srcdir)/include -DDATADIR=\"$(pkgdatadir)\" -DCONFDIR=\"$(pkgconfdir)\" \
	      -DLIBDIR=\"$(pkglibdir)\" -DLOCALDIR=\"$(pkgstatedir)\"
AM_LFLAGS = -i -B

CLEANFILES = loader.c arch_types_valid.c

arch_types_valid.c: is_valid_types_gen.pl $(top_srcdir)/include/object.h
	$(PERL) $(top_srcdir)/common/is_valid_types_gen.pl $(top_srcdir)/include/object.h
