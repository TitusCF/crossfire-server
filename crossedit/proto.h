/* crossedit.c */
extern int main(int argc, char **argv, char **env);
/* Attr.c */
extern int GetType(object *tmp);
extern Attr AttrCreate(char *name, App app, object *ob, AttrDef *desc, unsigned long flags, Edit edit);
extern void AttrChange(Attr self, object *ob, unsigned long flags, Edit edit);
extern void AttrDestroy(Attr self);
extern void AttrApply(Attr self);
/* CrFace.c */
/* CrEdit.c */
extern void CrEditRefresh(Widget w, XRectangle rect);
extern void CrEditBorderOff(Widget w);
extern void CrEditSelect(Widget w, XRectangle rect);
/* CrUtil.c */
extern void FaceDraw(Widget w, GC gc, New_Face *face, int x, int y);
extern GC GCCreate(Widget w);
/* Edit.c */
extern object *MapGetObjectZ(mapstruct *emap, int x, int y, int z);
extern void EditResizeScroll(Edit self, int width, int height, int dx, int dy);
extern Edit EditCreate(App app, EditType type, String path);
extern void EditDestroy(Edit self);
extern void EditUpdate(Edit self);
extern EditReturn EditSave(Edit self);
extern EditReturn EditLoad(Edit self);
extern void EditPerformFill(Edit self, int x, int y);
extern void EditPerformFillBelow(Edit self, int x, int y);
extern void EditFillRectangle(Edit self, XRectangle rec);
extern void EditWipeRectangle(Edit self, XRectangle rec);
extern void EditShaveRectangle(Edit self, XRectangle rec);
extern void EditShaveRectangleBelow(Edit self, XRectangle rec);
extern void EditCopyRectangle(Edit self, Edit src, XRectangle rect, int sx, int sy);
extern void EditDeletePoint(Edit self, int x, int y);
extern void EditModified(Edit self);
extern void EditUnmodified(Edit self);
extern Boolean EditDelete(Edit self, int x, int y, int z);
extern Boolean EditObjectDelete(Edit self, int x, int y, int z);
extern object *EditCloneInsert(Edit self, object *obj, int x, int y, int z);
extern Boolean EditInsert(Edit self, int x, int y, int z);
extern Boolean EditObjectInsert(Edit self, object *obj, int x, int y, int z);
extern void EditSetPath(Edit self, String path);
/* App.c */
extern App AppCreate(XtAppContext appCon, String displayString, XtResource resources[], Cardinal resourcesNum, XrmOptionDescRec *options, Cardinal optionsNum, int *argc, char *argv[]);
extern void AppUpdate(App self);
extern void AppSelectSet(App self, Edit edit, XRectangle rect);
extern void AppSelectUnset(App self);
extern void AppItemSet(App self, Edit edit, object *obj, int wallSet);
extern void AppEditDeattach(App self, Edit edit);
/* Bitmaps.c */
extern void BitmapsCreate(Display *d);
/* Str.c */
extern char *StrPathNormalize(char *this, char *src);
extern char *StrPathCd(char *cwd, const char *cd);
extern char *StrPathGenCd(char *cwd, const char *dstArg);
extern char *StrBasename(char *basename, const char *filename);
/* xutil.c */
extern int init_pngx_loader(Display *display);
extern int png_to_xpixmap(Display *display, Drawable draw, char *data, int len, Pixmap *pix, Pixmap *mask, Colormap *cmap, unsigned long *width, unsigned long *height);
extern int ReadImages(Display *gdisp, Pixmap **pixmaps, Pixmap **masks, Colormap *cmap, enum DisplayMode type);
extern void free_pixmaps(Display *gdisp, Pixmap *pixmaps);
extern int allocate_colors(Display *disp, Window w, long screen_num, Colormap *colormap, XColor discolor[13]);
