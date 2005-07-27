#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <stdlib.h>

#include "SquishMaze.h"

using namespace std;

SquishMaze::SquishMaze()
{
	srand(time(0));

	levelset = 0;
	currentlevel = 1;

	string levelsetfilename = "data/testlevelset.lvl";
	if (!LoadLevelSet(levelsetfilename))
	{
		cout << "SquishMaze: Failed to load the level: " << levelsetfilename << endl;
		running = false;
	}
	
	//create tess object; create callback functions
	poly = new TessPoly();
	poly->Init();
	poly->SetWindingRule(GLU_TESS_WINDING_POSITIVE);
}

SquishMaze::~SquishMaze(void)
{
}

bool SquishMaze::LoadLevelSet(string levelsetfilename)
{
	string str;
	
	fstream file (levelsetfilename.c_str(), ios::in);

	while(!file.eof())
	{
		str = GetNextLine(file);

		if (str.compare("<LevelSetBegin>") == 0)
		{
			delete levelset;
			levelset = new levelsettype;
			
			levelset->filename = levelsetfilename;
			
			while(!file.eof() && (str.empty() || str.compare("<LevelSetEnd>") != 0))
			{
				str = GetNextLine(file);

				if (str.compare("<Name>") == 0)
				{
					if (!file.eof())
					{
						str = GetNextLine(file);
						levelset->name = str;
					}
				}
				else if (str.compare("<LevelBegin>") == 0)
				{
					leveltype leveltemp;
					
					while(!file.eof() && (str.empty() || str.compare("<LevelEnd>") != 0))
					{
						str = GetNextLine(file);

						if (str.compare("<Name>") == 0)
						{
							if (!file.eof())
							{
								str = GetNextLine(file);
								leveltemp.name = str;
							}
						}
						else if (str.compare("<Number>") == 0)
						{
							if (!file.eof())
							{
								str = GetNextLine(file);
								leveltemp.number = atoi(str.c_str());
							}
						}
						else if (str.compare("<PlayerBegin>") == 0)
						{
							playertype playertemp;

							playertemp.color.r = .5;
							playertemp.color.g = .5;
							playertemp.color.b = .5;

							while(!file.eof() && (str.empty() || str.compare("<PlayerEnd>") != 0))
							{
								str = GetNextLine(file);
		
								if (str.compare("<GoalNumbers>") == 0)
								{
									if (!file.eof())
									{
										str = GetNextLine(file);
										vector<string> tokens;
										Tokenize(str, tokens, " ");
										for (int i = 0; i < tokens.size(); i++)
											playertemp.goalnumbers.push_back(atoi(tokens.at(i).c_str()));
									}
								}
								else if (str.compare("<PlayerColor>") == 0)
								{
									if (!file.eof())
									{
										vector<string> tokens;
										
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");
										
										if (tokens.size() == 3)
										{
											playertemp.color.r = atof(tokens.at(0).c_str());
											playertemp.color.g = atof(tokens.at(1).c_str());
											playertemp.color.b = atof(tokens.at(2).c_str());
										}
										else
											cout << "SquishMaze: Error parsing PlayerColor, wrong number of colors: " << tokens.size() << endl;
									}
								}
								else if (str.compare("<PlayerVertexBegin>") == 0)
								{
									while(!file.eof() && (str.empty() || str.compare("<PlayerVertexEnd>") != 0))
									{
										gldatatype vertextemp;
										
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");

										if (tokens.size() == 2)
										{
											vertextemp.d[0] = atof(tokens.at(0).c_str());
											vertextemp.d[1] = atof(tokens.at(1).c_str());
											vertextemp.d[2] = 0;
											playertemp.data.push_back(vertextemp);
										}
										else if (str.compare("<PlayerVertexEnd>") == 0) {}
										else
											cout << "SquishMaze: Error parsing PlayerVertex, wrong number of components: " << tokens.size() << endl;
									}
								}
								else if (str.compare("<PlayerEnd>") == 0) {}
								else if (str.empty()) {}
								else
									cout << "SquishMaze: Error parsing Player, unknown tag: " << str << endl;
							}

							for (int i = 0; i < playertemp.data.size(); i++)
							{
								playertemp.data.at(i).d[3] = playertemp.color.r;
								playertemp.data.at(i).d[4] = playertemp.color.g;
								playertemp.data.at(i).d[5] = playertemp.color.b;
							}

							leveltemp.player.push_back(playertemp);
						}
						else if (str.compare("<GoalBegin>") == 0)
						{
							goaltype goaltemp;

							goaltemp.color.r = .5;
							goaltemp.color.g = .5;
							goaltemp.color.b = .5;

							while(!file.eof() && (str.empty() || str.compare("<GoalEnd>") != 0))
							{
								str = GetNextLine(file);
		
								if (str.compare("<GoalNumber>") == 0)
								{
									if (!file.eof())
									{
										str = GetNextLine(file);
										goaltemp.goalnumber = atoi(str.c_str());
									}
								}
								else if (str.compare("<GoalColor>") == 0)
								{
									if (!file.eof())
									{
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");
										
										if (tokens.size() == 3)
										{
											goaltemp.color.r = atof(tokens.at(0).c_str());
											goaltemp.color.g = atof(tokens.at(1).c_str());
											goaltemp.color.b = atof(tokens.at(2).c_str());
										}
										else
											cout << "SquishMaze: Error parsing GoalColor, wrong number of colors: " << tokens.size() << endl;
									}
								}
								else if (str.compare("<GoalVertexBegin>") == 0)
								{
									while(!file.eof() && (str.empty() || str.compare("<GoalVertexEnd>") != 0))
									{
										gldatatype vertextemp;
										
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");

										if (tokens.size() == 2)
										{
											vertextemp.d[0] = atof(tokens.at(0).c_str());
											vertextemp.d[1] = atof(tokens.at(1).c_str());
											vertextemp.d[2] = 0;
											goaltemp.data.push_back(vertextemp);
										}
										else if (str.compare("<GoalVertexEnd>") == 0) {}
										else
											cout << "SquishMaze: Error parsing GoalVertex, wrong number of components: " << tokens.size() << endl;
									}
								}
								else if (str.compare("<GoalEnd>") == 0) {}
								else if (str.empty()) {}
								else
									cout << "SquishMaze: Error parsing Goal, unknown tag: " << str << endl;
							}

							for (int i = 0; i < goaltemp.data.size(); i++)
							{
								goaltemp.data.at(i).d[3] = goaltemp.color.r;
								goaltemp.data.at(i).d[4] = goaltemp.color.g;
								goaltemp.data.at(i).d[5] = goaltemp.color.b;
							}

							leveltemp.goal.push_back(goaltemp);
						}
						else if (str.compare("<WallBegin>") == 0)
						{
							walltype walltemp;

							walltemp.color.r = .8;
							walltemp.color.g = .8;
							walltemp.color.b = .8;

							while(!file.eof() && (str.empty() || str.compare("<WallEnd>") != 0))
							{
								str = GetNextLine(file);
		
								if (str.compare("<WallColor>") == 0)
								{
									if (!file.eof())
									{
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");
										
										if (tokens.size() == 3)
										{
											walltemp.color.r = atof(tokens.at(0).c_str());
											walltemp.color.g = atof(tokens.at(1).c_str());
											walltemp.color.b = atof(tokens.at(2).c_str());
										}
										else
											cout << "SquishMaze: Error parsing WallColor, wrong number of colors: " << tokens.size() << endl;
									}
								}
								else if (str.compare("<WallVertexBegin>") == 0)
								{
									while(!file.eof() && (str.empty() || str.compare("<WallVertexEnd>") != 0))
									{
										gldatatype vertextemp;
										
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");

										if (tokens.size() == 2)
										{
											vertextemp.d[0] = atof(tokens.at(0).c_str());
											vertextemp.d[1] = atof(tokens.at(1).c_str());
											vertextemp.d[2] = 0;
											walltemp.data.push_back(vertextemp);
										}
										else if (str.compare("<WallVertexEnd>") == 0) {}
										else
											cout << "SquishMaze: Error parsing WallVertex, wrong number of components: " << tokens.size() << endl;
									}
								}
								else if (str.compare("<WallEnd>") == 0) {}
								else if (str.empty()) {}
								else
									cout << "SquishMaze: Error parsing Wall, unknown tag: " << str << endl;
							}

							for (int i = 0; i < walltemp.data.size(); i++)
							{
								walltemp.data.at(i).d[3] = walltemp.color.r;
								walltemp.data.at(i).d[4] = walltemp.color.g;
								walltemp.data.at(i).d[5] = walltemp.color.b;
							}

							leveltemp.wall.push_back(walltemp);
						}
						else if (str.compare("<LevelEnd>") == 0) {}
						else if (str.empty()) {}
						else
							cout << "SquishMaze: Error parsing Level, unknown tag: " << str << endl;
					}
					
					levelset->level.push_back(leveltemp);
				}
				else if (str.compare("<LevelSetEnd>") == 0) {}
				else if (str.empty()) {}
				else
					cout << "SquishMaze: Error parsing LevelSet, unknown tag:\n\t" << str << endl;
			}
		}
		else if (str.empty()) {}
		else
		{
			cout << "SquishMaze: Error parsing file, <LevelSetBegin> needs to be the first tag\n";
			cout << "   " << str << endl;
		}
	}

	file.close();

	ShowLevelSet();

	return true;
}

void SquishMaze::ShowLevelSet(void)
{
	cout << "LevelSet name: " << levelset->name << endl;
	cout << "LevelSet filename: " << levelset->filename << endl;
	
	for (int i = 0; i < levelset->level.size(); i++)
	{
		leveltype &l = levelset->level.at(i);
		
		cout << "  Level name: " << l.name << endl;
		cout << "  Level number: " << l.number << endl;

		for (int j = 0; j < l.player.size(); j++)
		{
			playertype &p = l.player.at(j);

			cout << "    Player goals: ";
			for (int k = 0; k < p.goalnumbers.size(); k++)
				cout << p.goalnumbers.at(k) << " ";
			cout << endl;
			cout << "    Player color: " << p.color.r << "," << p.color.g << "," << p.color.b << endl;
			cout << "    Player data:\n";
			for (int k = 0; k < p.data.size(); k++)
			{
				cout << "       ";
				for (int l = 0; l < 6; l++)
					cout << p.data.at(k).d[l] << " ";
				cout << endl;
			}
		}
		for (int j = 0; j < l.wall.size(); j++)
		{
			walltype &w = l.wall.at(j);

			cout << "    Wall color: " << w.color.r << "," << w.color.g << "," << w.color.b << endl;
			cout << "    Wall data:\n";
			for (int k = 0; k < w.data.size(); k++)
			{
				cout << "       ";
				for (int l = 0; l < 6; l++)
					cout << w.data.at(k).d[l] << " ";
				cout << endl;
			}
		}
		for (int j = 0; j < l.goal.size(); j++)
		{
			goaltype &g = l.goal.at(j);

			cout << "    Goal number: " << g.goalnumber << endl;
			cout << "    Goal color: " << g.color.r << "," << g.color.g << "," << g.color.b << endl;
			cout << "    Goal data:\n";
			for (int k = 0; k < g.data.size(); k++)
			{
				cout << "       ";
				for (int l = 0; l < 6; l++)
					cout << g.data.at(k).d[l] << " ";
				cout << endl;
			}
		}
	}
}

string SquishMaze::GetNextLine(fstream &file)
{
	char buf [2000];
	string str;
		
	file.getline(buf, 2000);
	str = buf;
	// Remove cruft of either side of the stuff we really want
	str = str.erase(str.find_last_not_of(" ") + 1);
	str = str.erase(str.find_last_not_of("\t") + 1);
	str = str.erase(0, str.find_first_not_of(" "));
	str = str.erase(0, str.find_first_not_of("\t"));
	
	if (!str.empty() && str.at(0) == '#')
		str.clear();

	//cout << str << endl;
	return str;
}

void SquishMaze::Tokenize(const string& str, vector<string>& tokens, const string& del)
{
    string::size_type lastPos = str.find_first_not_of(del, 0);
    string::size_type pos = str.find_first_of(del, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(del, pos);
        pos = str.find_first_of(del, lastPos);
    }
}

inline bool SquishMaze::Draw(void)
{
	int i, j;

	leveltype &l = levelset->level.at(currentlevel-1);
	
	tick = SDL_GetTicks();
	dt = (float)(tick-lastTick)/100;
	lastTick = tick;

	

	//
	// Drawing stuff from here on
	//
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);

	// Draw the player polygons
	for (i = 0; i < l.player.size(); i++)
	{
		poly->BeginPolygon();
		poly->BeginContour();
		poly->RenderContour(l.player.at(i).data);
		poly->EndContour();
		poly->EndPolygon();
	}

	// Draw the wall polygons
	for (i = 0; i < l.wall.size(); i++)
	{
		poly->BeginPolygon();
		poly->BeginContour();
		poly->RenderContour(l.wall.at(i).data);
		poly->EndContour();
		poly->EndPolygon();
	}

	// Draw the goal polygons
	for (i = 0; i < l.goal.size(); i++)
	{
		poly->BeginPolygon();
		poly->BeginContour();
		poly->RenderContour(l.goal.at(i).data);
		poly->EndContour();
		poly->EndPolygon();
	}

	// Copy the image to a texture
	if (displaytype == 0)
		for (int j=0; j < videobuffer->h; ++j)
			memcpy(&teximage[j*tex_w], &videobuffer->buffer[j*videobuffer->w], videobuffer->w);
	else
		for (int j=0; j < shadowbuffer->h; ++j)
			memcpy(&teximage[j*tex_w], &shadowbuffer->buffer[j*shadowbuffer->w], shadowbuffer->w);

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_w, tex_h, 0,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, teximage);

	glLoadIdentity();

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(window.right, window.top, window.back);
		glTexCoord2f(1, 0);
		glVertex3f(window.left, window.top, window.back);
		glTexCoord2f(1, 1);
		glVertex3f(window.left, window.bottom, window.back);
		glTexCoord2f(0, 1);
		glVertex3f(window.right, window.bottom, window.back);
	glEnd();

	SDL_GL_SwapBuffers();

	return true;
}

bool SquishMaze::InPoly(vector<gldatatype> &poly, pointtype p)
{
	return false;
}


// This class is mostly from an article on flipcode.com
void combineCallback(GLdouble coords[3], GLdouble *vertex_data[4],
		GLfloat weight[4], GLdouble **dataOut)
{
	GLdouble *vertex;

	vertex = new GLdouble [6];
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];

	for (int i = 3; i < 6; i++)
		vertex[i] = (vertex_data[0][i] + vertex_data[1][i] + vertex_data[2][i] + vertex_data[3][i])/4;

	*dataOut = vertex;
}

void vertexCallback(GLvoid *vertex)
{
	GLdouble *ptr;

	ptr = (GLdouble *) vertex;
	glColor3dv((GLdouble *) ptr + 3);
	glVertex3dv((GLdouble *) ptr);
}

void TessPoly::Init(GLvoid)
{
	tobj = gluNewTess();

	// Set callback functions
	gluTessCallback(tobj, GLU_TESS_VERTEX, (GLvoid (*) ( )) &vertexCallback);
	gluTessCallback(tobj, GLU_TESS_BEGIN, (GLvoid (*) ( )) &glBegin);
	gluTessCallback(tobj, GLU_TESS_END, (GLvoid (*) ( )) &glEnd);
	gluTessCallback(tobj, GLU_TESS_COMBINE, (GLvoid (*) ( ))&combineCallback);
}

void TessPoly::RenderContour(vector<gldatatype> &poly)
{
	for (int i = 0; i < poly.size(); i++)
		gluTessVertex(tobj, poly.at(i).d, poly.at(i).d);
}
