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
			
			while(!file.eof() && str.compare("<LevelSetEnd>") != 0)
			{
				str = GetNextLine(file);
				
				if (str.compare("<Name>") == 0)
					if (!file.eof())
					{
						str = GetNextLine(file);
						levelset->name = str;
					}
				else if (str.compare("<LevelBegin>") == 0)
				{
					leveltype leveltemp;
					
					while(!file.eof() && str.compare("<LevelEnd>") != 0)
					{
						str = GetNextLine(file);

						if (str.compare("<Name>") == 0)
							if (!file.eof())
							{
								str = GetNextLine(file);
								leveltemp.name = str;
							}
						else if (str.compare("<Number>"))
							if (!file.eof())
							{
								str = GetNextLine(file);
								leveltemp.number = atoi(str.c_str());
							}
						else if (str.compare("<PlayerBegin>") == 0)
						{
							playertype playertemp;

							playertemp.color.r = .5;
							playertemp.color.g = .5;
							playertemp.color.b = .5;

							while(!file.eof() && str.compare("<PlayerEnd>") != 0)
							{
								str = GetNextLine(file);
		
								if (str.compare("<GoalNumbers>"))
									while(!file.eof() && str.at(0) != '<')
									{
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");
										for (int i = 0; i < tokens.size(); i++)
											playertemp.goalnumbers.push_back(atoi(tokens.at(i).c_str()));
									}
								else if (str.compare("<PlayerColor>") == 0)
									while(!file.eof() && str.at(0) != '<')
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
								else if (str.compare("<PlayerVertexBegin>") == 0)
								{
									while(!file.eof() && str.compare("<PlayerVertexEnd>") != 0)
									{
										GLdouble vertextemp[6];
										
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");

										if (tokens.size() == 2)
										{
											vertextemp[0] = atof(tokens.at(0).c_str());
											vertextemp[1] = atof(tokens.at(1).c_str());
											vertextemp[2] = 0;
											playertemp.data.push_back(vertextemp);
										}
										else
											cout << "SquishMaze: Error parsing PlayerVertex, wrong number of components: " << tokens.size() << endl;
									}
								}
							}

							for (int i = 0; i < playertemp.data.size(); i++)
							{
								playertemp.data.at(i)[3] = playertemp.color.r;
								playertemp.data.at(i)[4] = playertemp.color.g;
								playertemp.data.at(i)[5] = playertemp.color.b;
							}
							
							leveltemp.player.push_back(playertemp);
						}
						else if (str.compare("<GoalBegin>") == 0)
						{
							goaltype goaltemp;

							goaltemp.color.r = .5;
							goaltemp.color.g = .5;
							goaltemp.color.b = .5;

							while(!file.eof() && str.compare("<GoalEnd>") != 0)
							{
								str = GetNextLine(file);
		
								if (str.compare("<GoalNumber>"))
									if (!file.eof())
									{
										str = GetNextLine(file);
										goaltemp.goalnumber = atoi(str.c_str());
									}
								else if (str.compare("<GoalColor>") == 0)
									while(!file.eof() && str.at(0) != '<')
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
								else if (str.compare("<GoalVertexBegin>") == 0)
								{
									while(!file.eof() && str.compare("<GoalVertexEnd>") != 0)
									{
										GLdouble vertextemp [6];
										
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");

										if (tokens.size() == 2)
										{
											vertextemp[0] = atof(tokens.at(0).c_str());
											vertextemp[0] = atof(tokens.at(1).c_str());
											vertextemp[0] = 0;
											goaltemp.data.push_back(vertextemp);
										}
										else
											cout << "SquishMaze: Error parsing GoalVertex, wrong number of components: " << tokens.size() << endl;
									}
								}
								else if (str.empty()) {}
								else
									cout << "SquishMaze: Error parsing Goal, unknown tag: " << str << endl;
							}

							for (int i = 0; i < goaltemp.data.size(); i++)
							{
								goaltemp.data.at(i)[3] = goaltemp.color.r;
								goaltemp.data.at(i)[4] = goaltemp.color.g;
								goaltemp.data.at(i)[5] = goaltemp.color.b;
							}
							
							leveltemp.goal.push_back(goaltemp);
						}
						else if (str.compare("<WallBegin>") == 0)
						{
							walltype walltemp;

							walltemp.color.r = .3;
							walltemp.color.g = .3;
							walltemp.color.b = .3;

							while(!file.eof() && str.compare("<WallEnd>") != 0)
							{
								str = GetNextLine(file);
		
								if (str.compare("<WallColor>") == 0)
									while(!file.eof() && str.at(0) != '<')
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
								else if (str.compare("<WallVertexBegin>") == 0)
								{
									while(!file.eof() && str.compare("<WallVertexEnd>") != 0)
									{
										GLdouble vertextemp[6];
										
										vector<string> tokens;
										str = GetNextLine(file);
										Tokenize(str, tokens, " ");

										if (tokens.size() == 2)
										{
											vertextemp[0] = atof(tokens.at(0).c_str());
											vertextemp[0] = atof(tokens.at(1).c_str());
											vertextemp[0] = 0;
											walltemp.data.push_back(vertextemp);
										}
										else
											cout << "SquishMaze: Error parsing WallVertex, wrong number of components: " << tokens.size() << endl;
									}
								}
								else if (str.empty()) {}
								else
									cout << "SquishMaze: Error parsing Wall, unknown tag: " << str << endl;
							}

							for (int i = 0; i < walltemp.data.size(); i++)
							{
								walltemp.data.at(i)[3] = walltemp.color.r;
								walltemp.data.at(i)[4] = walltemp.color.g;
								walltemp.data.at(i)[5] = walltemp.color.b;
							}
							
							leveltemp.wall.push_back(walltemp);
						}
						else if (str.empty()) {}
						else
							cout << "SquishMaze: Error parsing Level, unknown tag: " << str << endl;
					}
					levelset->level.push_back(leveltemp);
				}
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
	
	if (str.at(0) == '#')
		str.clear();
	
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
	
	tick = SDL_GetTicks();
	dt = (float)(tick-lastTick)/100;
	lastTick = tick;


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);

	// Draw the player polygons
// 	for (i = 0; i < player->size(); i++)
// 	{
// 		poly->BeginPolygon();
// 		poly->BeginContour();
// 		poly->RenderContour();
// 		poly->EndContour();
// 		poly->EndPolygon();
// 	}

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

bool SquishMaze::InPoly(vector<GLdouble [6]> poly, pointtype p)
{
	return false;
}


// This class is mostly from an article on flipcode.com
void combineCallback(GLdouble coords[3], GLdouble *vertex_data[4],
		GLfloat weight[4], GLdouble **dataOut)
{
	GLdouble *vertex;

	vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];

	for (int i = 3; i < 6; i++)
		vertex[i] = weight[0] * vertex_data[0][i] + vertex_data[1][i] + vertex_data[2][i] + vertex_data[3][i];

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

void TessPoly::RenderContour(vector<GLdouble [6]> poly)
{
	for (int i = 0; i < poly.size(); i++)
		gluTessVertex(tobj, poly.at(i), poly.at(i));
}
