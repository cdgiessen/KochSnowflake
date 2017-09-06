#include <gl/freeglut.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

//freeglut functions
void init();
void display(void);
void centerOnScreen();

//calculating and displaying snowflake
void displayTriangle(float, float, float, float, float, float);
void displayLine(float, float, float, float);
void calculateSnowflake(float, float, float, float, float, float, int);
double calculateEdge(float, float, float, float, float, float, int);

//util functions for finding points
float getThirdWayPoint(float, float);
float getMidPoint(float, float);
float getDistance(float, float);
float getTriangle3rdVertex(float, float);
float getTriangleArea(float, float, float, float, float, float);

//inital veticies for the snowflake;
float V1x, V1y, V2x, V2y, V3x, V3y;

//inital number of recursions;
int recursions = 1;

//stores values for intermediate edge length and area
double* edgeLengthAtEachLevel;
float* areaAtEachLevel;
int* segmentsAtEachLevel;

//  define the window position on screen
int window_x;
int window_y;

//  variables representing the window size
int window_width = 700;
int window_height = 700;

//  variable representing the window title
char *window_title = "Koch Snowflake OpenGL FreeGlut App";

//if the mouse is inside the window or not
bool hasFocus;

//-------------------------------------------------------------------------
//  Set OpenGL program initial state.
//-------------------------------------------------------------------------
void init()
{
	//  Set the frame buffer clear color to black. 
	glClearColor(1.0, 1.0, 1.0, 1.0); //White is a nice background color. 
}

//-------------------------------------------------------------------------
//  This function is passed to glutDisplayFunc in order to display 
//	OpenGL contents on the window.
//-------------------------------------------------------------------------
void display(void)
{
	//  Clear the window or more specifically the frame buffer...
	//  This happens by replacing all the contents of the frame buffer by the clear color (white in our case)
	glClear(GL_COLOR_BUFFER_BIT);

	//  Show when are displaying the snowflake
	printf("Displaying Koch Snowflake\n\n");

	//compute and render the snowflake
	calculateSnowflake(V1x, V1y, V2x, V2y, V3x, V3y, recursions);

	//  Swap contents of backward and forward frame buffers
	glutSwapBuffers();
}

//-------------------------------------------------------------------------
//  This function takes in 7 arguments and produces a koch snowflake based on them.
//  The first 6 are 3 pairs of floats, one (x,y) pair for each edge of the triangle.
//  The last is the integer ammount of recursions the snowflake is supposed to undergo.
//-------------------------------------------------------------------------
void calculateSnowflake(float ax, float ay, float bx, float by, float  cx, float  cy, int recurse) {
	double edgeLength = 0.0f;
	float area = fabs((ax*(by - cy) + bx*(cy - ay) + cx*(ay - by)) / 2);
	float areaTotal = (float)(1.0/5.0)*(8-3*pow((4.0/9.0), recurse)) *area;

	for (int i = 0; i <= recurse; i++) {
		edgeLengthAtEachLevel[i] = 0.0f;
		areaAtEachLevel[i] = 0.0f;
		segmentsAtEachLevel[i] = 0;
	}

	edgeLength += calculateEdge(ax, ay, bx, by, cx, cy, recurse);
	edgeLength += calculateEdge(bx, by, cx, cy, ax, ay, recurse);
	edgeLength += calculateEdge(cx, cy, ax, ay, bx, by, recurse);

	areaAtEachLevel[recurse] = getTriangleArea(ax, ay, bx, by, cx, cy);
	
	float totalAreaSoFar = 0.0f;
	for (int i = recurse ; i >= 0; i--) {
		printf("Current recursion level is %d\n", recurse - i);
		printf("Number of segments:	%d\n", segmentsAtEachLevel[i]);
		printf("EdgeLength is:	%f\n", edgeLengthAtEachLevel[i]);
		totalAreaSoFar += areaAtEachLevel[i];
		printf("Area is: %f \n\n", totalAreaSoFar);
	}

	printf("\nTotal Number of Recursions %d\n", recurse);
	printf("Total Length = %f\n", edgeLength);
	printf("Total Area = %f\n\n", areaTotal);
}

//-------------------------------------------------------------------------
//  This function does the actual heavy lifting of the snowflake creation.  
//	It takes in 3 pairs of floats, a (x,y) pair each for the two sides of the line
//	and one pair for the back of the edge (not on the line but determines the height
//	of the snowflake. 
//	Again the last arg is the recursion number.
//	It returns the length of the edge, where if recurse <= 0 it returns the length from A to B
//	else it gives the sum of the lengths of the sub edges.
//-------------------------------------------------------------------------
double calculateEdge(float ax, float ay, float bx, float by, float cx, float cy, int recurse) {
	if (recurse <= 0) {
		displayLine(ax, ay, bx, by);
		double edgeLength = sqrt((ax - bx)*(ax - bx) + (ay - by)*(ay - by));
		edgeLengthAtEachLevel[0] += edgeLength;
		areaAtEachLevel[0] += getTriangleArea(ax, ay, bx, by, cx, cy)/4;
		segmentsAtEachLevel[0] += 1;
		return edgeLength;
	}
	else {
		float m1x, m1y, m2x, m2y, mx, my, c1x, c1y, b1x, b1y, b2x, b2y;

		m1x = getThirdWayPoint(ax, bx);
		m1y = getThirdWayPoint(ay, by);
		m2x = getThirdWayPoint(bx, ax);	
		m2y = getThirdWayPoint(by, ay);

		mx = getMidPoint(ax, bx);
		my = getMidPoint(ay, by);
		c1x = getTriangle3rdVertex(mx, cx);
		c1y = getTriangle3rdVertex(my, cy);

		b1x = getThirdWayPoint(ax, cx);
		b1y = getThirdWayPoint(ay, cy);
		b2x = getThirdWayPoint(bx, cx);
		b2y = getThirdWayPoint(by, cy);

		float sum = 0.0f;
		sum += calculateEdge(ax, ay, m1x, m1y, b1x, b1y, recurse - 1);
		sum += calculateEdge(m1x, m1y, c1x, c1y, m2x, m2y, recurse - 1);
		sum += calculateEdge(c1x, c1y, m2x, m2y, m1x, m1y, recurse - 1);
		sum += calculateEdge(m2x, m2y, bx, by, b2x, b2y, recurse - 1);

		edgeLengthAtEachLevel[recurse] += sqrt((ax - bx)*(ax - bx) + (ay - by)*(ay - by));
		areaAtEachLevel[recurse] += getTriangleArea(ax, ay, bx, by, cx, cy)/4;
		segmentsAtEachLevel[recurse] += 1;
		return sum;
	}
}

//	returns the point mid way between a and b
float getMidPoint(float a, float b) {
	return (a + b) / 2;
}

//	returns the point that is 1/3 away from a in the direction of b
float getThirdWayPoint(float a, float b) {
	return (2*a + b) / 3;
}

//	Not to be confused with the distance forula. This is simply the difference between 2 coordinates 
//	Also can be negative
float getDistance(float a, float b) {
	return b - a;
}

//	Gets the point on the edge off to one side that forms the top vertex of the middle two edges
float getTriangle3rdVertex(float mid, float opp) {
	return mid + getDistance(opp, mid) / 3;
}

//	returns the area in between each of the three points
float getTriangleArea(float ax, float ay, float bx, float by, float cx, float cy) {
	return fabs((ax*(by - cy) + bx*(cy - ay) + cx*(ay - by)) / 2);
}

//-------------------------------------------------------------------------
//	Takes in the three verticies of a triangle, and paints a line to each corner
//	with each corner having one of the three values of CYM color model.
//	Unused for project but looks pretty.
//-------------------------------------------------------------------------
void displayTriangle(float x1, float y1, float x2, float y2, float  x3, float  y3) {

	glBegin(GL_LINE_LOOP);              // Each set of 4 vertices form a quad
	glColor3f(0.0f, 1.0f, 1.0f); // Cyan
	glVertex2f(x1, y1);    // x, y

	glColor3f(1.0f, 0.0f, 1.0f); // Magenta
	glVertex2f(x2, y2);
	
	glColor3f(1.0f, 1.0f, 0.0f); // Yellow
	glVertex2f(x3, y3);

	glEnd();
}

//-------------------------------------------------------------------------
//	Takes in two coordinate pairs and produces a line in between them
//	Color is black. would of liked to make an option to enable colored snowflake later on
//-------------------------------------------------------------------------
void displayLine(float x1, float y1, float x2, float y2) {
	
	glBegin(GL_LINE_LOOP);

	glColor3f(0, 0, 0);
	glVertex2f(x1, y1);

	glColor3f(0, 0, 0);
	glVertex2f(x2, y2);

	glEnd();
	
}

//-------------------------------------------------------------------------
//  This function is passed to the glutReshapeFunc and is called
//  whenever the window is resized.
//-------------------------------------------------------------------------
void reshape(int w, int h)
{
	//  Stay updated with the window width and height
	window_width = w;
	window_height = h;

	//  Reset viewport
	glViewport(0, 0, window_width, window_height);

	//  Print current width and height on the screen
	printf("Window Width: %d, Window Height: %d.\n",
		window_width, window_height);
}

//-------------------------------------------------------------------------
//  Gets keyboard input, if use hits escape, the window closes;
//-------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
	
	//  Print what key the user is hitting
	//printf("User is hitting the '%c' key.\n", key);
	//printf("ASCII code is %d.\n", key);

	switch (key)
	{
	//  User hits ESC key
	case 27:
		exit(1);
		break;
	}

	glutPostRedisplay();
	
}

//-------------------------------------------------------------------------
//  This function sets the window x and y coordinates
//  such that the window becomes centered
//-------------------------------------------------------------------------
void centerOnScreen()
{
	window_x = (glutGet(GLUT_SCREEN_WIDTH) - window_width) / 2;
	window_y = (glutGet(GLUT_SCREEN_HEIGHT) - window_height) / 2;
}

//-------------------------------------------------------------------------
//  Program Main method.
//
//	Explicitly Takes in 7 arguements. Any more or less and they won't work.
//
//	Example flags are  -1 -0.5 0 1 1 -0.5 4
//  
//	1 - 6 are floating point numbers for the coordinates of the vertices
//	Its 1x 1y 2x 2y 3x 3y
//
//	7 is an int for the number of recursions
//-------------------------------------------------------------------------
void main(int argc, char **argv)
{
	//	Allocate memory for the various records keeping. 
	edgeLengthAtEachLevel = new double[100];
	areaAtEachLevel = new float[100];
	segmentsAtEachLevel = new int[100];

	// Set number of recursions
	recursions = 8;

	// Set the 3 initial vertices of the snowflake.
	V1x = -0.5;
	V1y = 0;
	V2x = 0.0f;
	V2y = 0.8660254037844f;
	V3x = 0.5f;
	V3y = 0;

	//	Gets command line input , sets default values if not correctly inputted
	if (argc == 8) {
		float VertexAx, VertexAy, VertexBx, VertexBy, VertexCx, VertexCy;
		int r;
		cout << "0 argument is " << argv[0] << "\n";

		if (!(istringstream(argv[1]) >> VertexAx)) {
			cout << "Invalid Arugment for Vertex 1 X, Setting default of " << V1x << "\n";
		} else { 
			V1x = VertexAx; 
			cout << "X component of First Vertex is " << argv[1] << "\n";
		}
		
		if (!(istringstream(argv[2]) >> VertexAy)) {
			cout << "Invalid Arugment for Vertex 1 Y, Setting default of " << V1y << "\n";
		} else { 
			V1y = VertexAy; 
			cout << "Y component of First Vertex is " << argv[2] << "\n";
		}
		
		if (!(istringstream(argv[3]) >> VertexBx)) {
			cout << "Invalid Arugment for Vertex 2 X, Setting default of " << V2x << "\n";
		}
		else { 
			V2x = VertexBx; 
			cout << "X component of Second Vertex is " << argv[3] << "\n";
		}
		
		if (!(istringstream(argv[4]) >> VertexBy)) {
			cout << "Invalid Arugment for Vertex 2 Y, Setting default of " << V2y << "\n";
		}
		else { 
			V2y = VertexBy; 
			cout << "Y component of Second Vertex is " << argv[4] << "\n";
		}
		
		if (!(istringstream(argv[5]) >> VertexCx)) {
			cout << "Invalid Arugment for Vertex 3 X, Setting default of " << V3x << "\n";
		}
		else { 
			V3x = VertexCx; 
			cout << "X component of Third Vertex is " << argv[5] << "\n";
		}
		
		if (!(istringstream(argv[6]) >> VertexCy)) {
			cout << "Invalid Arugment for Vertex 3 Y, Setting default of " << V3y << "\n";
		}
		else { 
			V3y = VertexCy; 
			cout << "Y component of Third Vertex is " << argv[6] << "\n";
		}
		
		if (!(istringstream(argv[7]) >> r)) {
			cout << "Invalid Arugment for recursions, Setting default of " << recursions << "\n";
		}
		else { 
			recursions = r; 
			cout << "Recursion level is " << argv[7] << "\n";
		}

		V1x = VertexAx;
		V1y = VertexAy;
		V2x = VertexBx;
		V2y = VertexBy;
		V3x = VertexCx;
		V3y = VertexCy;
		recursions = r;
	}

	//  Connect to the windowing system + create a window
	//  with the specified dimensions and position
	//  + set the display mode + specify the window title.

	glutInit(&argc, argv);
	centerOnScreen();
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(window_x, window_y);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow(window_title);

	//  Tells whether to display the window full screen or not
	//  Press Alt + Esc to exit a full screen.
	int full_screen = 0;

	//  View in full screen if the fullscreen flag is on
	if (full_screen)
		glutFullScreen();

	//  Set OpenGL program initial state.
	init();

	// Set the callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutMotionFunc(motion);

	// Set the callback functions
	glutDisplayFunc(display);

	printf("Area of inital triangle %f\n", getTriangleArea(V1x, V1y, V2x, V2y, V3x, V3y));
	
	//  Start GLUT event processing loop
	glutMainLoop();
	
}