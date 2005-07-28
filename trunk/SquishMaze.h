#ifndef SQUISHMAZE_H
#define SQUISHMAZE_H

#include "GameController.h"
#include "VideoInput.h"
#include "Shadow.h"
#include "KeyInput.h"
#include "GL/gl.h"
#include "GL/glu.h"

#include <string>
#include <vector>

//
// Data structures and types to hold the levelset
//

struct gldatatype
{
	GLdouble d [6];
};

struct pointtype
{
	float x, y;
};

struct colortype
{
	float r, g, b;
};

struct connectortype
{
	int a, b;
	float k;
	float c;
	float l;
};

struct playertype
{
	vector<gldatatype> data;
	vector<pointtype> vel;

	vector<connectortype> con;

	colortype color;
	vector<int> goalnumbers;
};

struct walltype
{
	vector<gldatatype> data;
	colortype color;
};

struct goaltype
{
	vector<gldatatype> data;

	colortype color;
	int goalnumber;
};

struct leveltype
{
	int number;
	string name;
	
	vector<playertype> player;
	vector<walltype> wall;
	vector<goaltype> goal;
};

struct levelsettype
{
	string name;
	string filename;

	vector<leveltype> level;
};


// Polygon drawing class
class TessPoly
{
  private:
	GLUtesselator *tobj;

  public:
	void Init(GLvoid);
	void SetWindingRule(GLenum windingrule) { gluTessProperty(tobj, GLU_TESS_WINDING_RULE, windingrule); };
	void RenderContour(vector<gldatatype> &poly);
	void BeginPolygon(GLvoid) { gluTessBeginPolygon(tobj, NULL); };
	void EndPolygon(GLvoid) { gluTessEndPolygon(tobj); };
	void BeginContour(GLvoid) { gluTessBeginContour(tobj); };
	void EndContour(GLvoid) { gluTessEndContour(tobj); };
	void End(GLvoid) { gluDeleteTess(tobj); };
};


// The main class
class SquishMaze : public GameController
{
  public:
	SquishMaze(void);
	~SquishMaze(void);

	bool Draw(void);
	bool LoadLevelSet(string levelsetfilename);

  private:
	string GetNextLine(fstream &file);
	void ShowLevelSet(void);
	void Tokenize(const string& str, vector<string>& tokens, const string& del = " ");
	bool InPoly(vector<gldatatype> &poly, float x, float y);
	void ResolveCollision(playertype &player, int n, vector<gldatatype> &poly);
	bool LinesIntersect(gldatatype &a1, gldatatype &a2, gldatatype &b1, gldatatype &b2);

	levelsettype *levelset;
	int currentlevel;
	
	int tick, lastTick;
	float dt;

	TessPoly *poly;
};

#endif
