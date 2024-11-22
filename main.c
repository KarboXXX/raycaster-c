#include <GL/freeglut_std.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.141592653589793238462

struct level {
  int x;
  int y;
  int size;
  int map[];
};
struct level map = {
  8,8,64,
  {
    1,1,1,0,0,0,0,0,
    1,0,1,0,0,1,1,0,
    1,0,1,0,0,1,0,0,
    1,0,1,0,0,0,0,0,
    1,0,1,0,0,0,0,0,
    1,0,1,0,0,0,1,0,
    1,0,0,0,0,0,1,0,
    1,1,1,1,1,1,1,0,
  }
};

void drawMap2D() {
  int x,y,xo,yo;
  for (int y=0; y<map.y;y++) {
    for (int x=0; x<map.x;x++) {
      if (map.map[y*map.x+x] == 1) { glColor3f(1,1,1); } else { glColor3f(0,0,0); }
      xo=x*map.size;
      yo=y*map.size;
      glBegin(GL_QUADS);
      glVertex2i(xo+1, yo+1);
      glVertex2i(xo+1, yo+map.size-1);
      glVertex2i(xo+map.size-1, yo+map.size-1);
      glVertex2i(xo+map.size-1, yo+1);
      glEnd();
    }
  }
}

struct player {
  float x;
  float y;
  
  float dX;
  float dY;
  float ang;
};
struct player p;
void draw_player() {
  glColor3f(1,1,0);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(p.x, p.y);
  glEnd();

  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(p.x, p.y);
  glVertex2i(p.x + p.dX * 5, p.y + p.dY * 5);
  glEnd();
}

void handle_keyboard(unsigned char key, int x, int y) {
  if (key == 'w') {
    /* p.y-=4; */
    p.x+=p.dX;
    p.y+=p.dY;
  }
  if (key == 's') {
    /* p.y+=4; */
    p.x-=p.dX;
    p.y-=p.dY;
  }
  if (key == 'a') {
    /* p.x-=4; */
    p.ang-=0.1;
    if (p.ang < 0) { p.ang += 2*PI; }
    p.dX = cos(p.ang) * 5;
    p.dY = sin(p.ang) * 5;
  }
  if (key == 'd') {
    /* p.x+=4; */
    p.ang+=0.1;
    if (p.ang > 2*PI) { p.ang -= 2*PI; }
    p.dX = cos(p.ang) * 5;
    p.dY = sin(p.ang) * 5;
  }

  glutPostRedisplay();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawMap2D();
  draw_player();
  
  glutSwapBuffers();
}

void init() {
  glClearColor(0.3, 0.3, 0.3, 0.3);
  gluOrtho2D(0, 1024, 512, 0);

  p.x=300; p.y=300;
  p.dX = cos(p.ang) * 5;
  p.dY = sin(p.ang) * 5;
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(1024, 512);
  glutCreateWindow("KarboXXX's Raycaster");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(handle_keyboard);
  glutMainLoop();
}
