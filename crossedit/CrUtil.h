#ifndef _CrUtil_h
#define _CrUtil_h

extern void DrawFacePart(Widget w, GC gc, New_Face *face, int x, int y, int x_offset, int y_offset);
extern void DrawPartObject(Widget w, GC gc, object * op, int x, int y);
extern GC GCCreate (Widget w);

#endif /* _CrUtil_h */
