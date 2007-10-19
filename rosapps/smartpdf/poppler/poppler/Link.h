//========================================================================
//
// Link.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef LINK_H
#define LINK_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Object.h"

class GooString;
class UGooString;
class Array;
class Dict;
class Sound;

//------------------------------------------------------------------------
// LinkAction
//------------------------------------------------------------------------

enum LinkActionKind {
  actionGoTo,			// go to destination
  actionGoToR,			// go to destination in new file
  actionLaunch,			// launch app (or open document)
  actionURI,			// URI
  actionNamed,			// named action
  actionMovie,			// movie action
  actionSound,			// sound action
  actionUnknown			// anything else
};

class LinkAction {
public:

  // Destructor.
  virtual ~LinkAction() {}

  // Was the LinkAction created successfully?
  virtual GBool isOk() = 0;

  // Check link action type.
  virtual LinkActionKind getKind() = 0;

  // Parse a destination (old-style action) name, string, or array.
  static LinkAction *parseDest(Object *obj);

  // Parse an action dictionary.
  static LinkAction *parseAction(Object *obj, GooString *baseURI = NULL);

  // Extract a file name from a file specification (string or
  // dictionary).
  static GooString *getFileSpecName(Object *fileSpecObj);
};

//------------------------------------------------------------------------
// LinkDest
//------------------------------------------------------------------------

enum LinkDestKind {
  destXYZ,
  destFit,
  destFitH,
  destFitV,
  destFitR,
  destFitB,
  destFitBH,
  destFitBV
};

class LinkDest {
public:

  // Build a LinkDest from the array.
  LinkDest(Array *a);

  // Copy a LinkDest.
  LinkDest *copy() { return new LinkDest(this); }

  // Was the LinkDest created successfully?
  GBool isOk() { return ok; }

  // Accessors.
  LinkDestKind getKind() { return kind; }
  GBool isPageRef() { return pageIsRef; }
  int getPageNum() { return pageNum; }
  Ref getPageRef() { return pageRef; }
  double getLeft() { return left; }
  double getBottom() { return bottom; }
  double getRight() { return right; }
  double getTop() { return top; }
  double getZoom() { return zoom; }
  GBool getChangeLeft() { return changeLeft; }
  GBool getChangeTop() { return changeTop; }
  GBool getChangeZoom() { return changeZoom; }

private:

  LinkDestKind kind;		// destination type
  GBool pageIsRef;		// is the page a reference or number?
  union {
    Ref pageRef;		// reference to page
    int pageNum;		// one-relative page number
  };
  double left, bottom;		// position
  double right, top;
  double zoom;			// zoom factor
  GBool changeLeft, changeTop;	// for destXYZ links, which position
  GBool changeZoom;		//   components to change
  GBool ok;			// set if created successfully

  LinkDest(LinkDest *dest);
};

//------------------------------------------------------------------------
// LinkGoTo
//------------------------------------------------------------------------

class LinkGoTo: public LinkAction {
public:

  // Build a LinkGoTo from a destination (dictionary, name, or string).
  LinkGoTo(Object *destObj);

  // Destructor.
  virtual ~LinkGoTo();

  // Was the LinkGoTo created successfully?
  virtual GBool isOk() { return dest || namedDest; }

  // Accessors.
  virtual LinkActionKind getKind() { return actionGoTo; }
  LinkDest *getDest() { return dest; }
  UGooString *getNamedDest() { return namedDest; }

private:

  LinkDest *dest;		// regular destination (NULL for remote
				//   link with bad destination)
  UGooString *namedDest;	// named destination (only one of dest and
				//   and namedDest may be non-NULL)
};

//------------------------------------------------------------------------
// LinkGoToR
//------------------------------------------------------------------------

class LinkGoToR: public LinkAction {
public:

  // Build a LinkGoToR from a file spec (dictionary) and destination
  // (dictionary, name, or string).
  LinkGoToR(Object *fileSpecObj, Object *destObj);

  // Destructor.
  virtual ~LinkGoToR();

  // Was the LinkGoToR created successfully?
  virtual GBool isOk() { return fileName && (dest || namedDest); }

  // Accessors.
  virtual LinkActionKind getKind() { return actionGoToR; }
  GooString *getFileName() { return fileName; }
  LinkDest *getDest() { return dest; }
  UGooString *getNamedDest() { return namedDest; }

private:

  GooString *fileName;		// file name
  LinkDest *dest;		// regular destination (NULL for remote
				//   link with bad destination)
  UGooString *namedDest;	// named destination (only one of dest and
				//   and namedDest may be non-NULL)
};

//------------------------------------------------------------------------
// LinkLaunch
//------------------------------------------------------------------------

class LinkLaunch: public LinkAction {
public:

  // Build a LinkLaunch from an action dictionary.
  LinkLaunch(Object *actionObj);

  // Destructor.
  virtual ~LinkLaunch();

  // Was the LinkLaunch created successfully?
  virtual GBool isOk() { return fileName != NULL; }

  // Accessors.
  virtual LinkActionKind getKind() { return actionLaunch; }
  GooString *getFileName() { return fileName; }
  GooString *getParams() { return params; }

private:

  GooString *fileName;		// file name
  GooString *params;		// parameters
};

//------------------------------------------------------------------------
// LinkURI
//------------------------------------------------------------------------

class LinkURI: public LinkAction {
public:

  // Build a LinkURI given the URI (string) and base URI.
  LinkURI(Object *uriObj, GooString *baseURI);

  // Destructor.
  virtual ~LinkURI();

  // Was the LinkURI created successfully?
  virtual GBool isOk() { return uri != NULL; }

  // Accessors.
  virtual LinkActionKind getKind() { return actionURI; }
  GooString *getURI() { return uri; }

private:

  GooString *uri;			// the URI
};

//------------------------------------------------------------------------
// LinkNamed
//------------------------------------------------------------------------

class LinkNamed: public LinkAction {
public:

  // Build a LinkNamed given the action name.
  LinkNamed(Object *nameObj);

  virtual ~LinkNamed();

  virtual GBool isOk() { return name != NULL; }

  virtual LinkActionKind getKind() { return actionNamed; }
  GooString *getName() { return name; }

private:

  GooString *name;
};

//------------------------------------------------------------------------
// LinkMovie
//------------------------------------------------------------------------

class LinkMovie: public LinkAction {
public:

  LinkMovie(Object *annotObj, Object *titleObj);

  virtual ~LinkMovie();

  virtual GBool isOk() { return annotRef.num >= 0 || title != NULL; }

  virtual LinkActionKind getKind() { return actionMovie; }
  GBool hasAnnotRef() { return annotRef.num >= 0; }
  Ref *getAnnotRef() { return &annotRef; }
  GooString *getTitle() { return title; }

private:

  Ref annotRef;
  GooString *title;
};

//------------------------------------------------------------------------
// LinkSound
//------------------------------------------------------------------------

class LinkSound: public LinkAction {
public:

  LinkSound(Object *soundObj);

  virtual ~LinkSound();

  virtual GBool isOk() { return sound != NULL; }

  virtual LinkActionKind getKind() { return actionSound; }

  double getVolume() { return volume; }
  GBool getSynchronous() { return sync; }
  GBool getRepeat() { return repeat; }
  GBool getMix() { return mix; }
  Sound *getSound() { return sound; }

private:

  double volume;
  GBool sync;
  GBool repeat;
  GBool mix;
  Sound *sound;
};

//------------------------------------------------------------------------
// LinkUnknown
//------------------------------------------------------------------------

class LinkUnknown: public LinkAction {
public:

  // Build a LinkUnknown with the specified action type.
  LinkUnknown(char *actionA);

  // Destructor.
  virtual ~LinkUnknown();

  // Was the LinkUnknown create successfully?
  virtual GBool isOk() { return action != NULL; }

  // Accessors.
  virtual LinkActionKind getKind() { return actionUnknown; }
  GooString *getAction() { return action; }

private:

  GooString *action;		// action subtype
};

//------------------------------------------------------------------------
// LinkBorderStyle
//------------------------------------------------------------------------

enum LinkBorderType {
  linkBorderSolid,
  linkBorderDashed,
  linkBorderEmbossed,
  linkBorderEngraved,
  linkBorderUnderlined
};

class LinkBorderStyle {
public:

  LinkBorderStyle(LinkBorderType typeA, double widthA,
		  double *dashA, int dashLengthA,
		  double rA, double gA, double bA);
  ~LinkBorderStyle();

  LinkBorderType getType() { return type; }
  double getWidth() { return width; }
  void getDash(double **dashA, int *dashLengthA)
    { *dashA = dash; *dashLengthA = dashLength; }
  void getColor(double *rA, double *gA, double *bA)
    { *rA = r; *gA = g; *bA = b; }

private:

  LinkBorderType type;
  double width;
  double *dash;
  int dashLength;
  double r, g, b;
};

//------------------------------------------------------------------------
// Link
//------------------------------------------------------------------------

class Link {
public:

  // Construct a link, given its dictionary.
  Link(Dict *dict, GooString *baseURI);

  // Destructor.
  ~Link();

  // Was the link created successfully?
  GBool isOk() { return ok; }

  // Check if point is inside the link rectangle.
  GBool inRect(double x, double y)
    { return x1 <= x && x <= x2 && y1 <= y && y <= y2; }

  // Get action.
  LinkAction *getAction() { return action; }

  // Get the link rectangle.
  void getRect(double *xa1, double *ya1, double *xa2, double *ya2)
    { *xa1 = x1; *ya1 = y1; *xa2 = x2; *ya2 = y2; }

  // Get the border style info.
  LinkBorderStyle *getBorderStyle() { return borderStyle; }

private:

  double x1, y1;		// lower left corner
  double x2, y2;		// upper right corner
  LinkBorderStyle *borderStyle;	// border style
  LinkAction *action;		// action
  GBool ok;			// is link valid?
};

//------------------------------------------------------------------------
// Links
//------------------------------------------------------------------------

class Links {
public:

  // Extract links from array of annotations.
  Links(Object *annots, GooString *baseURI);

  // Destructor.
  ~Links();

  // Iterate through list of links.
  int getNumLinks() const { return numLinks; }
  Link *getLink(int i) const { return links[i]; }

  // If point <x>,<y> is in a link, return the associated action;
  // else return NULL.
  LinkAction *find(double x, double y) const;

  // Return true if <x>,<y> is in a link.
  GBool onLink(double x, double y) const;

private:

  Link **links;
  int numLinks;
};

#endif
