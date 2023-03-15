/****************************************************
 * 2016-2021 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include <ctime>
#include <vector>
#include "defs.h"
#include "utilities.h"
#include "framebuffer.h"
#include "colorandmaterials.h"
#include "rasterization.h"

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

const int SZ = 51;
const int SZ2 = SZ / 2;

void closedSquare(int x, int y, color C) {
}

void closedSquare(const ivec2 &centerPt, color C) {
}

void openSquare(const ivec2 &centerPt, color C) {
}

void render() {
	frameBuffer.clearColorAndDepthBuffers();

	drawLine(frameBuffer, 0, 0, 100, 100, red);
	drawLine(frameBuffer, 100, 100, 200, 100, green);

	closedSquare(100, 150, red);
	closedSquare(ivec2(200, 150), green);
	openSquare(ivec2(300, 150), blue);

	frameBuffer.showColorBuffer();
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}
int main(int argc, char *argv[]) {
   graphicsInit(argc, argv, __FILE__);
   /*     
	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboardUtility);
	glutMouseFunc(mouseUtility);

	frameBuffer.setClearColor(white);

	glutMainLoop();
	*/
	
	double x = 45;
	double y = 99;
	cout << x << ' ' << y << endl;
	swap(x, y);
	cout << x << ' ' << y << endl;
	
	cout << approximatelyEqual(0.0, 0.0000001) << endl;

	cout << approximatelyZero(1.0) << endl;
	cout << approximatelyZero(0.000000000001) << endl;

	cout << normalizeDegrees(360.5) << endl;

	cout << normalizeRadians(-31*PI) << endl;

	cout << rad2deg(1) << endl;

	cout << min(0, 0, 0) << endl;
	dvec3 v(-2, 2, 1);
	dvec3 v2(3, 0, 1);
	dvec3 v3(1, 0, 1);
	cout << isOrthogonal(v,v2) <<" ++ " << endl;
	cout << areaOfParallelogram(v, v2) << endl;
	cout << areaOfTriangle(v, v2, v3) << endl;
	dvec3 v4 = pointingVector(v, v2);
	cout << v4.x << ", "<< v4.y << ", " << v4.z << endl;
	dvec2 d(1, 0);
	dvec2 d2(0, 1);
	cout << cosBetween(d, d2) <<endl;

	cout << formAcuteAngle(v3, v2) << " here "<< endl;


	double root[2] = {0.0,0.0};
	cout << quadratic(-4.0, -2.0, -1.0, root[2]);
	return 0;
}