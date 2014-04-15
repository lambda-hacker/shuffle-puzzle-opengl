/*	
    Author / Programmer: Syed M. Madani
*/

#include <iostream>
#include <vector>
#include <conio.h>

#include "glut.h"

#include "Grid.hpp"
#include "Image.hpp"


using namespace std;
using namespace smadani;

// Veiwing Window is at the scale of X-Axis: 0 - 20.0 & Y-Axis: 0 - 20.0
GLdouble vw_xmin = 0.0, vw_xmax = 20.0, vw_ymin = 0.0, vw_ymax = 20.0;

GLint vp_x = 0, vp_y = 0, vp_width = 1000, vp_height = 600;

size_t s_rows = 4, s_cols = 4;

GLfloat x_offset = ((GLfloat)vp_height / (GLfloat)vp_width) * (vw_xmax - vw_xmin) / s_cols;
GLfloat y_offset = (vw_ymax - vw_ymin) / s_rows;

Grid game_grid(s_rows, s_cols);
GridController grid_controller(game_grid);
Shuffler grid_shuffler(game_grid);

Timer game_timer;
time_t time_elapsed;

Scorer game_step_counter;

Image texture;
GLuint texture_id;

/* GAME STATE */

void initialize();
void display_handler();
void reshape_handler(int w, int h);
void keyboard_handler(unsigned char key, int x, int y);
void special_keys_handler(int key, int x, int y);
void mouse_handler(int button, int state, int x, int y);
void update(int v);
bool is_game_solved();


int main(int argc, char** argv)
{

	if (load_TGA_file("fruits.tga", texture) == -1)
		cout << "Cannot Load" << endl;
	else cout << "Image Loaded" << endl;

	cout << x_offset << "  " <<  y_offset << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(1000, 600); //put image size or specific grid size 
	glutCreateWindow("Shuffle Puzzle");

	initialize();
	grid_shuffler.shuffle();
	
	//Registering CallBack Functions
	glutTimerFunc(500, update, 1);
	glutDisplayFunc(display_handler);
	glutReshapeFunc(reshape_handler);
	glutKeyboardFunc(keyboard_handler);
	glutSpecialFunc(special_keys_handler);
	glutMouseFunc(mouse_handler);
	glutMainLoop();
	
	return 0;
}

void initialize()
{
	glClearColor(0.1, 0.1, 0.2, 0.5);
	texture_id = load_texture(texture);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(vw_xmin, vw_xmax, vw_ymin, vw_ymax);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void draw_stroked_string(char* str)
{
	glPushMatrix();
	glScalef(.0085, .0085, .0085);
	char* c = str;
	size_t i = 0;
	while (c[i] != '\0')
	{
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c[i]);
		i++;
	}
	glPopMatrix();
}


void display_handler()
{

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);					//Enabling Textures.
	glBindTexture(GL_TEXTURE_2D, texture_id);	//Binding the Texture
	
	//Setting Texture paramters for the Welcome Board
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	
	for (size_t i = 0; i < game_grid.num_rows(); i++)
	{
		for (size_t j = 0; j < game_grid.num_columns(); j++)
		{
			Piece const& p = game_grid.grid_piece(i, j);
			if (p.row() == s_rows - 1 && p.column() == s_cols - 1)
			{
				glColor3f(0.0f, 0.0f, 0.0f);
				glBegin(GL_QUADS);
				glVertex2f(j * x_offset, i * y_offset);
				glVertex2f((j+1) * x_offset - 0.1, i * y_offset);
				glVertex2f((j+1) * x_offset - 0.1, (i+1) * y_offset - 0.1);
				glVertex2f(j * x_offset, (i+1) * y_offset - 0.1);
				glEnd();
				glColor3f(1.0, 1.0, 1.0);
				continue;				
			}
			
			
			Vector2 t1 = p.tc1();
			Vector2 t2 = p.tc2();
			Vector2 t3 = p.tc3();
			Vector2 t4 = p.tc4();

			
			glBegin(GL_QUADS);
			glTexCoord2f(t1.x, t1.y);
			glVertex2f(j * x_offset, i * y_offset);

			glTexCoord2f(t2.x, t2.y);
			glVertex2f((j+1) * x_offset - 0.06, i * y_offset);

			glTexCoord2f(t3.x, t3.y);
			glVertex2f((j+1) * x_offset - 0.06, (i+1) * y_offset - 0.1);

			glTexCoord2f(t4.x, t4.y);
			glVertex2f(j * x_offset, (i+1) * y_offset - 0.1);
			glEnd();
		}
	}

	GLfloat p = s_cols * x_offset;
		
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(p + 1.0f, 2.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(p + 5.0f, 2.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(p + 5.0f, 8.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(p + 1.0f, 8.0f);
	glEnd();

	char buff[100];
	glPushMatrix();
		glTranslatef(12.0, 11.0, 0);
		draw_stroked_string("Time");
		glPushMatrix();
		glTranslatef(0.0f, -2.0f, 0.0f);
		sprintf(buff, "%d", time_elapsed / 1000);
		strcat(buff, " seconds");
		draw_stroked_string(buff);
		glPopMatrix();

		glTranslatef(0.0f, 5.0f, 0.0f);
		draw_stroked_string("Num Steps");
		glTranslatef(0.0f, -2.0f, 0.0f);
		draw_stroked_string(itoa(game_step_counter.get_num_steps(), buff, 10));

		if (is_game_solved())
		{
			game_timer.stop();
			glTranslatef(0.0f, 4.0f, 0.0f);
			draw_stroked_string("GAME OVER");
		}
	glPopMatrix();

	glutPostRedisplay();
	glFlush(); 
}


void reshape_handler(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(vw_xmin, vw_xmax, vw_ymin, vw_ymax);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport( vp_x, vp_y, vp_width, vp_height );
}


void keyboard_handler(unsigned char key, int x, int y)
{

	//Use GridController to move pieces, etc
	switch (key)
	{
	case 27: exit(0);
		break;

	case 13: if (is_game_solved()) {
          grid_shuffler.shuffle();
		  game_timer.reset();
		  game_step_counter.reset();
		  grid_controller.game_reset();
		  glutPostRedisplay();
	}

	
	case 't':
	case 'T': 
		if (is_game_solved())
			std::cout << "Yes" << std::endl;
		else std::cout << "NO" << std::endl;
		//glutPostRedisplay();
		break;

	case 's':
	case 'S':
		if (game_grid.is_active())
		{
			grid_shuffler.shuffle();
			game_timer.reset();
			game_step_counter.reset();
			grid_controller.game_reset();
			glutPostRedisplay();
		}
			
		break;

	case 'h':
	case 'H':
		std::cout << "Help mode" << std::endl;
		break;

	case 'g':
	case 'G':
		if (grid_controller.is_game_started())
			cout << "Yes" << endl;
		else cout << "NO" << endl;
		break;
	};
}


void special_keys_handler(int key, int x, int y)
{
    if (is_game_solved()) return;
	switch (key) {
	case GLUT_KEY_UP:
		if (game_grid.e_r() > 0){
			grid_controller.move(game_grid.e_r() - 1, game_grid.e_c());
			game_step_counter.increment_steps();
			glutPostRedisplay();
		}
		break;

	case GLUT_KEY_DOWN:
		if (game_grid.e_r() < game_grid.num_rows() - 1){
			grid_controller.move(game_grid.e_r() + 1, game_grid.e_c());
			game_step_counter.increment_steps();
			glutPostRedisplay();
		}
		break;

	case GLUT_KEY_LEFT:
		if (game_grid.e_c() < game_grid.num_columns() - 1){
			grid_controller.move(game_grid.e_r(), game_grid.e_c() + 1);
			game_step_counter.increment_steps();
			glutPostRedisplay();
		}
		break;

	case GLUT_KEY_RIGHT:
		if (game_grid.e_c() > 0){
			grid_controller.move(game_grid.e_r(), game_grid.e_c() - 1);
			game_step_counter.increment_steps();
			glutPostRedisplay();
		}
		break;
	};
}


void mouse_handler(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (is_game_solved()) return;
		if (state == GLUT_DOWN && (x >= vp_x && x <= vp_x + 600) && (y >= vp_y && y <= vp_y + vp_height))
		{
			size_t c = (size_t)((x - vp_x) * s_cols / 600);
			size_t r = (size_t)((y - vp_y) * s_rows / vp_height);
			r = s_rows - r - 1;

			cout << c << " " << r << endl;
			
			if (grid_controller.move(r, c))
			{
				game_step_counter.increment_steps();
			}
			glutPostRedisplay();
		}
		break;
	default:
		break;
	};
}



void update(int v)
{
	if (grid_controller.is_game_started() && !game_timer.is_started())
		game_timer.start();

    time_elapsed = game_timer.get_time();
    glutPostRedisplay();
	glutTimerFunc(500, update, 1);
}
bool is_game_solved()
{
	return game_grid.is_all_pieces_in_place();
	
}

