
#ifndef __CEXTRACT__

extern char *StrPathNormalize(char *this, char *src);
extern char *StrPathCd(char *cwd, const char *cd);
extern char *StrPathGenCd(char *cwd, const char *dstArg);
extern char *StrBasename(char *basename, const char *filename);

#endif /* __CEXTRACT__ */
