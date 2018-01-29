//Animation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream> /* for cout */
#include <GL\glut.h> /* declare always last for exit(0) error */
#include <array>

using namespace std;

// Set constants
const float PI = 3.1415;
const float BACKGROUND_COLOR[4] = { 1.0, 1.0, 1.0, 1.0 };

// Angles are applied on z-axis only and are in order: [torso, neck, head, bru leg, brd leg, blu leg, bld leg, fru leg, frd leg, flu leg, fld leg]
// Initial body parts angles (from normal position) are zero
array<float, 11> curr_angles = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
array<float, 11> target_angles;
array<float, 11> prev_angles;
float animation_speed = 1.0;
int fps = 200;

// Set repeating animations
int curr_clip = 0;
array<array<float, 11>, 3> walk_clips = { {
	{1, 0, 0, 30, -10, -30, -10, -85, 85, 45, 0},
	{0, 4, 0, -5, -10, 5, -10, -45, 45, -85, 85},
	{-1, 0, 0, -30, -10, 30, -10, 45, 0, -45, 45}
} };

array<array<float, 11>, 2> run_clips = { {
	{ -5, 0, 0, 65, -5, 70, -5, -70, 40, -65, 40 },
	{ 5, 3, 0, -65, -5, -70, -5, 70, 0, 65, 0 }
} };


int mode = 0; // The currently selected mode
bool is_front = false;

// Declare functions
void updateMode(int new_mode);
void timer(int);
void updateAngles(void);

// Draws a solid cylinder with the given radius and height.
void draw_cylinder(GLfloat radius, GLfloat height)
{
	GLfloat x = 0.0;
	GLfloat y = 0.0;
	GLfloat angle = 0.0;
	GLfloat angle_stepsize = 0.1;

	/** Draw the tube */
	glBegin(GL_QUAD_STRIP);
	angle = 0.0;
	while (angle < 2 * PI) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y, height);
		glVertex3f(x, y, 0.0);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glVertex3f(radius, 0.0, 0.0);
	glEnd();

	/** Draw the circle on top of cylinder */
	glBegin(GL_POLYGON);
	angle = 0.0;
	while (angle < 2 * PI) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y, height);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glEnd();
}

void initEnvironment()
{
	// Attributes
	glEnable(GL_BLEND);
	glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], BACKGROUND_COLOR[2]); // Set background color
	glLoadIdentity();

	// Set camera view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100, 100, -100, 100, -100, 100);
	glMatrixMode(GL_MODELVIEW);

	updateMode(1);
	timer(0);
}

void displayAnimal(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the window
	glLoadIdentity();

	if (!is_front) {
		// Side view
		glRotatef(90, 0.0, 1.0, 0.0);
	}
	else
	{
		// Front view with slight rotation on y-axis
		glRotatef(20, 0.0, 1.0, 0.0);
	}
	// Draw torso
	glColor3ub(165, 3, 3);
	glRotatef(curr_angles[0], 1.0, 0.0, 0.0); // Apply rotation to body part and its children
	glPushMatrix();
	glTranslatef(0, 0, -25); // Set it to center
	draw_cylinder(10, 50);
	glPopMatrix();
	// Draw neck
	glColor3ub(135, 5, 5);
	glPushMatrix();
	glTranslatef(0, 2.5, 20); // Move it almost to the edge of the torso and a little upward
	glRotatef(curr_angles[1], 1.0, 0.0, 0.0); // Apply rotation to body part
	glRotatef(-30, 1.0, 0.0, 0.0); // Apply initial rotation
	draw_cylinder(3, 20);
	// Draw head
	glColor3ub(239, 31, 31);
	glTranslatef(0, 0, 20); // Move it to the edge of the neck
	glRotatef(curr_angles[2], 1.0, 0.0, 0.0); // Apply rotation to body part
	glutSolidSphere(8, 16, 16);
	glPopMatrix();
	// Draw back-right upper leg
	glPushMatrix();
	glColor3ub(135, 5, 5);
	glTranslatef(-7, -8, -17); // Move it down (-8) to the back (-17) and the right side (-7)
	glRotatef(curr_angles[3], 1.0, 0.0, 0.0); // Apply rotation to body part
	glRotatef(90, 1.0, 0.0, 0.0); // Rotate it to be vertical to the torso
	draw_cylinder(3, 10);
	// Draw back-right down leg
	glColor3ub(193, 7, 7);
	glTranslatef(0, 0, 8); // Move it down, almost to the edge of the upper leg
	glRotatef(curr_angles[4], 1.0, 0.0, 0.0); // Apply rotation to body part
	draw_cylinder(3, 15);
	glPopMatrix();
	// Draw back-left upper leg
	glPushMatrix();
	glColor3ub(135, 5, 5);
	glPushMatrix();
	glTranslatef(7, -8, -17); // Move it down (-8) to the back (-17) and the left side (7)
	glRotatef(curr_angles[5], 1.0, 0.0, 0.0); // Apply rotation to body part
	glRotatef(90, 1.0, 0.0, 0.0); // Rotate it to be vertical to the torso
	draw_cylinder(3, 10);
	// Draw back-left down leg
	glColor3ub(193, 7, 7);
	glTranslatef(0, 0, 8); // Move it down, almost to the edge of the upper leg
	glRotatef(curr_angles[6], 1.0, 0.0, 0.0); // Apply rotation to body part
	draw_cylinder(3, 15);
	glPopMatrix();
	// Draw front-right upper leg
	glPushMatrix();
	glColor3ub(135, 5, 5);
	glTranslatef(-7, -8, 17); // Move it down (-8) to the front (17) and the right side (-7)
	glRotatef(curr_angles[7], 1.0, 0.0, 0.0); // Apply rotation to body part
	glRotatef(90, 1.0, 0.0, 0.0); // Rotate it to be vertical to the torso
	draw_cylinder(3, 10);
	// Draw front-right down leg
	glColor3ub(193, 7, 7);
	glTranslatef(0, 0, 8); // Move it down, almost to the edge of the upper leg
	glRotatef(curr_angles[8], 1.0, 0.0, 0.0); // Apply rotation to body part
	draw_cylinder(3, 15);
	glPopMatrix();
	// Draw front-left upper leg
	glPushMatrix();
	glColor3ub(135, 5, 5);
	glPushMatrix();
	glTranslatef(7, -8, 17); // Move it down (-8) to the front (17) and the left side (7)
	glRotatef(curr_angles[9], 1.0, 0.0, 0.0); // Apply rotation to body part
	glRotatef(90, 1.0, 0.0, 0.0); // Rotate it to be vertical to the torso
	draw_cylinder(3, 10);
	// Draw front-left down leg
	glColor3ub(193, 7, 7);
	glTranslatef(0, 0, 8); // Move it down, almost to the edge of the upper leg
	glRotatef(curr_angles[10], 1.0, 0.0, 0.0); // Apply rotation to body part
	draw_cylinder(3, 15);
	glPopMatrix();

	glutSwapBuffers(); // Swap buffers
	return;
}

void timer(int)
{
	updateAngles();
	glutTimerFunc(1000.0 / fps, timer, 0);
}

// Interpolorates between the given values and returns the amount to add to the current value.
float interpolorateValues(float start, float target)
{
	return (target - start) / (fps * animation_speed);
}

// Updates the angle of every bodypart, creating an animation.
void updateAngles(void)
{
	bool animation_finished = true;
	for (int i = 0; i < 11; i++)
	{
		// If target has been reached
		if (((target_angles[i] - prev_angles[i] >= 0) && (curr_angles[i] >= target_angles[i])) ||
			((target_angles[i] - prev_angles[i] <= 0) && (curr_angles[i] <= target_angles[i])))
		{
			// Make them be equal
			curr_angles[i] = target_angles[i];
		}
		else
		{
			// Animation has not ended yet
			animation_finished = false;
			// Update the angle
			curr_angles[i] += interpolorateValues(prev_angles[i], target_angles[i]);
		}
	}

	if (animation_finished)
	{
		// If repeating animation, load next clip
		if (mode == 5) // Walk
		{
			curr_clip = (curr_clip + 1) % 3;
			prev_angles = curr_angles;
			target_angles = walk_clips[curr_clip];
		}
		else if (mode == 6) // Run
		{
			curr_clip = (curr_clip + 1) % 2;
			prev_angles = curr_angles;
			target_angles = run_clips[curr_clip];
		}
	}

	glutPostRedisplay();
	return;
}

// Control keyboard input
void KeyboardKeyPressed(unsigned char key, int x, int y)
{
	if (key == 32) // Spacebar
	{
		// Change view angle
		is_front = !is_front;
		glutPostRedisplay();
	}
}

// Sets the proper control points and updates the current mode.
void updateMode(int new_mode)
{
	// Update mode
	mode = new_mode;

	// Set new angle for every bodypart
	switch (mode)
	{
	case 1: // Initial Position
	{
		array<float, 11> new_target = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		target_angles = new_target;
		animation_speed = 1.5;
		break;
	}
	case 2: // Legs Up
	{
		array<float, 11> new_target = { 0, 0, 0, 0, 0, 0, 0, -40, 40, -40, 40 };
		target_angles = new_target;
		animation_speed = 1.5;
		break;
	}
	case 3: // Head Down
	{
		array<float, 11> new_target = { 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		target_angles = new_target;
		animation_speed = 1.5;
		break;
	}
	case 4: // Up in two legs
	{
		array<float, 11> new_target = { -45, 40, 0, 20, 20, 20, 20, -40, 40, -40, 40 };
		target_angles = new_target;
		animation_speed = 1.5;
		break;
	}
	case 5: // Walk
	{
		curr_clip = 0;
		target_angles = walk_clips[curr_clip];
		animation_speed = 0.5;
		break;
	}
	case 6: // Run
	{
		curr_clip = 0;
		target_angles = run_clips[curr_clip];
		animation_speed = 0.4;
		break;
	}
	}

	// Set as previous angles the angles before the mode changed
	prev_angles = curr_angles;

	glutPostRedisplay();
}

void menu(int id) {
	if (id != 7) {
		updateMode(id);
	}
	else { // Quit
		exit(0);
	}
}

void main(int argc, char** argv)
{
	// Standard GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);

	// Create Window
	glutInitWindowPosition(500, 200);
	glutCreateWindow("Animation");
	glEnable(GL_DEPTH_TEST);

	// Create menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Initial Position", 1);
	glutAddMenuEntry("Legs Up", 2);
	glutAddMenuEntry("Head Down", 3);
	glutAddMenuEntry("Up in two legs", 4);
	glutAddMenuEntry("Walk", 5);
	glutAddMenuEntry("Run!", 6);
	glutAddMenuEntry("Quit", 7);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutKeyboardFunc(KeyboardKeyPressed);
	glutDisplayFunc(displayAnimal);

	initEnvironment(); /* Initialize environment-view properties */
	glutMainLoop(); /* Enter event loop */
}