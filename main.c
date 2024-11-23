#include <GL/freeglut_std.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#define PI 3.141592653589793238462
#define PI_OVER_TWO PI / 2
#define PI3_OVER_TWO (3 * PI) / 2
#define ONE_DEGREE_IN_RADIANS 0.01745329252

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 530

struct level {
  int x;
  int y;
  int size;
  int map[];
};
struct level map = {
  8,8,64,
  {
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,1,0,0,1,0,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,
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

struct raycaster {
  float x;
  float y;
};
/* struct ray { */
/*   float x; */
/*   float y; */
/*   float ang; */
/* }; */

float dist(float ax, float ay, float bx, float by, float angle) // teorema de pitagoras
{
  return ( sqrt((bx-ax) * (bx-ax) + (by-ay) * (by-ay)) );
}

void raycast() {
  int r,mapx,mapy,mappos,dof;
  float rx,ry,ra,xoff,yoff;

  ra = p.ang - ONE_DEGREE_IN_RADIANS * 40;
  if (ra<0) { ra +=2*PI; }
  if (ra>2*PI) { ra-=2*PI; }

  for (r=0; r<80; r++) {
    // -------------------------- horizontal --------------------------
    float a_tan = (-1/tan(ra));
    float hx = p.x;
    float hy = p.y;
    float distH = 100000;
    dof = 0;

    // olhando pra baixo // looking down
    if (ra>PI) {
      ry = (((int)p.y>>6) <<6) - 0.0001;
      rx = (p.y - ry) * a_tan+p.x;
      yoff = -64;
      xoff = -yoff * a_tan;
    }

    //olhando pra cima // looking up
    if (ra<PI) {
      ry = (((int)p.y>>6) <<6) + 64;
      rx = (p.y - ry) * a_tan+p.x;
      yoff = 64;
      xoff = -yoff * a_tan;
    }

    //olhando diretamente pra esquerda ou direita // looking directly either right or left
    if (ra == 0 || ra == PI) {
      rx = p.x;
      ry = p.y;
      dof = 8;
    }

    while(dof<8) {
      mapx = (int)rx >> 6;
      mapy = (int)ry >> 6;
      mappos = mapy * map.x + mapx;
      if (mappos > 0 && mappos < map.x * map.y && map.map[mappos] == 1) { // ray hit a wall
        hx = rx;
        hy = ry;
        distH = dist(p.x, p.y, hx, hy, ra);
        dof = 8;
      } else {
        rx += xoff;
        ry += yoff;
        dof++;
      }
    }
    // -------------------------- vertical --------------------------
    float n_tan = -tan(ra);
    float vx = p.x;
    float vy = p.y;
    float distV = 100000;
    dof = 0;

    // olhando pra esquerda // looking left
    if (ra > PI_OVER_TWO && ra < PI3_OVER_TWO) {
      rx = (((int)p.x>>6) <<6) - 0.0001;
      ry = (p.x - rx) * n_tan+p.y;
      xoff = -64;
      yoff = -xoff * n_tan;
    }

    //olhando pra direita // looking right
    if (ra < PI_OVER_TWO || ra > PI3_OVER_TWO) {
      rx = (((int)p.x>>6) <<6) + 64;
      ry = (p.x - rx) * n_tan+p.y;
      xoff = 64;
      yoff = -xoff * n_tan;
    }

    //olhando diretamente pra cima ou pra baixo // looking directly up or down
    if (ra == 0 || ra == PI) {
      rx = p.x;
      ry = p.y;
      dof = 8;
    }

    while(dof<8) {
      mapx = (int)rx >> 6;
      mapy = (int)ry >> 6;
      mappos = mapy * map.x + mapx;
      if (mappos > 0 && mappos < map.x * map.y && map.map[mappos] == 1) { // ray hit a wall
        vx = rx;
        vy = ry;
        distV = dist(p.x, p.y, vx, vy, ra);
        dof = 8;
      } else {
        rx += xoff;
        ry += yoff;
        dof++;
      }
    }

    float distance;
    if (distV < distH) {
      distance = distV;
      rx = vx;
      ry = vy;
      
      glColor3f(0.9, 0, 0);
    } else if (distH < distV) {
      distance = distH;
      rx = hx;
      ry = hy;

      glColor3f(0.7, 0, 0);
    }

    ra+=ONE_DEGREE_IN_RADIANS;
    if (ra<0) { ra +=2*PI; }
    if (ra>2*PI) { ra-=2*PI; }
    
    // render raycasts in map
    /* glColor3f(1, 0, 0); */
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2i(p.x, p.y);
    glVertex2i(rx, ry);
    glEnd();

    //render 3D
    float ca=p.ang-ra;
    if (ca<0) { ca+=2*PI; }
    if (ca>2*PI) { ca-=2*PI; }
    distance = distance * cos(ca); // fix fisheye effect
    
    float line_height = (map.size * WINDOW_HEIGHT - 800)/distance;
    float line_offset = 200-line_height/2;
    glLineWidth(12);
    glBegin(GL_LINES);
    glVertex2i(r*6+530,line_offset);
    glVertex2i(r*6+530, line_height+line_offset);
    glEnd();
  }
}

int fix_ang(int a){
  if(a>359){
    a-=360;
  } if(a<0){
    a+=360;
  }
  return a;
}

unsigned char keymaps[256];
void handle_keyboard() {
  if (keymaps['w'] == 1) {
    /* p.y-=4; */
    p.x+=p.dX;
    p.y+=p.dY;
  }
  if (keymaps['s'] == 1) {
    /* p.y+=4; */
    p.x-=p.dX;
    p.y-=p.dY;
  }
  if (keymaps['a'] == 1) {
    /* p.x-=4; */
    p.ang-=0.1;
    fix_ang(p.ang);
    if (p.ang < 0) { p.ang += 2*PI; }
    p.dX = cos(p.ang) * 5;
    p.dY = sin(p.ang) * 5;
  }
  if (keymaps['d'] == 1) {
    /* p.x+=4; */
    p.ang+=0.1;
    fix_ang(p.ang);
    if (p.ang > 2*PI) { p.ang -= 2*PI; }
    p.dX = cos(p.ang) * 5;
    p.dY = sin(p.ang) * 5;
  }

  /* glutPostRedisplay(); */
}

void handle_keydown(unsigned char key, int x, int y) { keymaps[key] = 1; }
void handle_keyup(unsigned char key, int x, int y) { keymaps[key] = 0; }

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawMap2D();
  draw_player();
  raycast();
  handle_keyboard();
  
  glutSwapBuffers();
  glutPostRedisplay();
}

void init() {
  glClearColor(0.3, 0.3, 0.3, 0.3);
  gluOrtho2D(0, 1024, 512, 0);

  p.x=300; p.y=300;
  p.dX = cos(p.ang) * 3;
  p.dY = sin(p.ang) * 3;
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("KarboXXX's Raycaster");
  init();
  glutDisplayFunc(display);
  glutIgnoreKeyRepeat(GLUT_KEY_REPEAT_OFF);
  glutKeyboardFunc(handle_keydown);
  glutKeyboardUpFunc(handle_keyup);
  glutMainLoop();
}
