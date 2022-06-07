#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double rad,fixed,pp;
double max_size = 60, current_size= 20;
double unit_t=5;         //Unit of Translation
double alpha = (5*pi)/180;  //Unit Angle of Rotation

struct point
{
	double x,y,z;
};

class Vector3D{
	public:
	double x,y,z;
	Vector3D(){
		x=0;
		y=0;
		z=0;
	}
	Vector3D(double x, double y, double z){
		this->x = x;
		this->y =y;
		this->z = z;
	}
	double magnitude(){
		return sqrt((this->x * this->x) + (this->y * this->y)+(this->z * this->z));
	}

	Vector3D operator+(const Vector3D a){
		return Vector3D(this->x+a.x, this->y+a.y ,this->z+a.z);
	}
	Vector3D operator-(const Vector3D a){
		return Vector3D(this->x-a.x, this->y-a.y ,this->z-a.z);
	}
	double operator*(const Vector3D a){
		return (this->x * a.x+ this->y * a.y + this->z*a.z);
	}
	Vector3D operator*(const double a){
		return Vector3D(this->x * a, this->y * a , this->z*a);
	}
	Vector3D operator^(const Vector3D a){
		return Vector3D((this->y*a.z - this->z*a.y), (this->z*a.x-this->x*a.z) , (this->x*a.y-this->y*a.x));
	}
	void print(){
		printf("x = %lf , y = %lf , z = %lf\n",x,y,z);
	}

};
//struct point pos;
Vector3D u, r, l, pos; 


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
	a = a/2;
    //glColor3f(0,0.0,1.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}

void drawRectangle(double length, double width)
{
	length = length/2;
	width = width/2;
	glColor3f(0,0.0,1.0);
	glBegin(GL_QUADS);
	{
		glVertex3f(length,width,0);
		glVertex3f(length,-width,0);
		glVertex3f(-length,-width,0);
		glVertex3f(-length,width,0);

	}
	glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(100,0,0);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawOneEighthOfSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*(2*pi)/4);
			points[i][j].y=r*sin(((double)j/(double)slices)*(2*pi)/4);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f(200,0,0);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
  
               
			}glEnd();
		}
	}
}
void drawOneFourthOfCylinder(double radius,int slices,int stacks,double height)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=height/2*sin(((double)i/(double)stacks)*(pi/2));
		r=radius;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*(2*pi)/4);
			points[i][j].y=r*sin(((double)j/(double)slices)*(2*pi)/4);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f(0,200,0);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawCylinder(double radius,int slices,int stacks,double height)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=height*sin(((double)i/(double)stacks)*(pi/2));
		r=radius;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*(2*pi));
			points[i][j].y=r*sin(((double)j/(double)slices)*(2*pi));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f(0,0.5,0.3);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}
void draw8Sphere(double r){
	//Upper Hemisphere 
	
	glPushMatrix();
	glColor3f(1,0,0);
	glTranslatef(current_size/2,current_size/2,current_size/2);
	drawOneEighthOfSphere(r,20,30);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,1,0);
	glRotatef(90,0,0,1);
	glTranslatef(current_size/2,current_size/2,current_size/2);
	drawOneEighthOfSphere(r,20,30);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,0,1);
	glRotatef(-90,0,0,1);
	glTranslatef(current_size/2,current_size/2,current_size/2);
	drawOneEighthOfSphere(r,20,30);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,0.5,0);
	glRotatef(180,0,0,1);
	glTranslatef(current_size/2,current_size/2,current_size/2);
	drawOneEighthOfSphere(r,20,30);
	glPopMatrix();

	//Lower Hemisphere
	glPushMatrix();
	glColor3f(1,0,0);
	glRotatef(90,0,1,0);
	glTranslatef(current_size/2,current_size/2,current_size/2);
	drawOneEighthOfSphere(r,20,30);
    glPopMatrix();

	glPushMatrix();
	glColor3f(1,1,1);
	glRotatef(90,0,1,0);
	glRotatef(90,1,0,0);
	glTranslatef(current_size/2,current_size/2,current_size/2);
	drawOneEighthOfSphere(r,20,30);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,0,1);
	glRotatef(90,0,1,0);
	glRotatef(180,1,0,0);
	glTranslatef(current_size/2,current_size/2,current_size/2);
	drawOneEighthOfSphere(r,20,30);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,1,1);
	glRotatef(90,0,1,0);
	glRotatef(-90,1,0,0);
	glTranslatef(current_size/2,current_size/2,current_size/2);
	drawOneEighthOfSphere(r,20,30);
	glPopMatrix();


	
}
void draw12PieceCylinder(double r){
	glPushMatrix();
	glColor3f(1,0,0);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,1,0);
	glRotatef(90,0,0,1);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,0,1);
	glRotatef(-90,0,0,1);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1,1,1);
	glRotatef(180,0,0,1);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	//Up 

	glPushMatrix();
	glColor3f(1,0,0);
	glRotatef(90,1,0,0);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,1,0);
	glRotatef(90,1,0,0);
	glRotatef(90,0,0,1);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,0,1);
	glRotatef(90,1,0,0);
	glRotatef(-90,0,1,0);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1,1,1);
	glRotatef(90,1,0,0);
	glRotatef(90,0,1,0);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	//Down 

	glPushMatrix();
	glColor3f(1,0,0);
	glRotatef(-90,1,0,0);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,1,0);
	glRotatef(-90,1,0,0);
	glRotatef(90,0,0,1);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,0,1);
	glRotatef(-90,1,0,0);
	glRotatef(-90,0,1,0);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1,1,1);
	glRotatef(-90,1,0,0);
	glRotatef(90,0,1,0);
	glTranslatef(current_size/2,current_size/2,0);
	drawOneFourthOfCylinder(r,20,30,current_size);
	glPopMatrix();
}

void drawSixSquare(){
	glPushMatrix();
	glColor3f(1,0.5,0.5);
	glTranslatef(0,0,max_size/2);
	drawSquare(current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1,0.5,0.5);
	glTranslatef(0,0,-max_size/2);
	drawSquare(current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5,1,0.5);
	glRotatef(90,1,0,0);
	glTranslatef(0,0,-max_size/2);
	drawSquare(current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5,1,0.5);
	glRotatef(90,1,0,0);
	glTranslatef(0,0,max_size/2);
	drawSquare(current_size);
	glPopMatrix();


	glPushMatrix();
	glColor3f(1,0.5,0);
	glRotatef(90,0,1,0);
	glTranslatef(0,0,-max_size/2);
	drawSquare(current_size);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1,0.5,0);
	glRotatef(90,0,1,0);
	glTranslatef(0,0,max_size/2);
	drawSquare(current_size);
	glPopMatrix();

	



}

void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);  
}

void makeVertex(point p)
{
	glVertex3f(p.x, p.y, p.z);
}
void makePlane(point a, point b, point c, point d)
{
	makeVertex(a);
	makeVertex(b);
	makeVertex(c);
	makeVertex(d);
}
void drawCube(double val)
{
	point w ={0,0,0};
	point x ={val,0,0};
	point y ={val,val,0};
	point z ={0,val,0};

	point a = {0,0,val};
	point b = {val,0,val};
	point c = {val,val,val};
	point d = {0,val,val};

	glBegin(GL_QUADS);
	glColor3f(100,0,0);
	makePlane(w,x,y,z);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(100,0,0);
	makePlane(w,x,a,b);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(100,0,0);
	makePlane(w,a,d,z);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(100,0,0);
	makePlane(c,y,d,z);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(100,0,0);
	makePlane(b,x,y,c);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(100,0,0);
	makePlane(a,b,c,d);
	glEnd();
}


void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			//drawgrid=1-drawgrid;
			l = l*cos(alpha) + (u^l)*sin(alpha);
			r = r*cos(alpha) + (u^r)*sin(alpha);
			break;
		case '2':
			l = l*cos(-alpha) + (u^l)*sin(-alpha);
			r = r*cos(-alpha) + (u^r)*sin(-alpha);
			break;
		case '3':
			l = l*cos(alpha) + (r^l)*sin(alpha);
			u = u*cos(alpha) + (r^u)*sin(alpha);
			break;
		case '4':
			l = l*cos(-alpha) + (r^l)*sin(-alpha);
			u = u*cos(-alpha) + (r^u)*sin(-alpha);
			break;
		case '5':
			r = r*cos(alpha) + (l^r)*sin(alpha);
			u = u*cos(alpha) + (l^u)*sin(alpha);
			break;
		case '6':
			r = r*cos(-alpha) + (l^r)*sin(-alpha);
			u = u*cos(-alpha)+ (l^u)*sin(-alpha);
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			//cameraHeight -= 3.0;
			pos = pos - (l*unit_t);
			break;
		case GLUT_KEY_UP:		// up arrow key
			//cameraHeight += 3.0;
			pos = pos + (l*unit_t);
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			pos = pos - (r*unit_t);
			break;
		case GLUT_KEY_LEFT:
			//cameraAngle -= 0.03;
			pos = pos + (r*unit_t);
			break;

		case GLUT_KEY_PAGE_UP:
			pos = pos + (u*unit_t);
			break;
		case GLUT_KEY_PAGE_DOWN:
			pos = pos - (u*unit_t);
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			current_size += 1;
			if(current_size>max_size) current_size = max_size;
			break;
		case GLUT_KEY_END:
			current_size -= 1;
			if(current_size<0) current_size = 0;
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(0,0,0,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(pos.x,pos.y,pos.z,	pos.x+l.x,pos.y+l.y,pos.z+l.z,	u.x,u.y,u.z);
	//printf("Print: %lf" ,pos.x);
	//gluLookAt(100,100,0,	1,0,0,	0,0,1);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();
	//drawOneEighthOfSphere(50,30,40);
	double r = (max_size-current_size)/2;
	draw8Sphere(r);
	draw12PieceCylinder(r);
	
	//drawOneFourthOfCylinder(50,30,40,20);
	//drawCube(20);
	
	//drawRectangle(40,20);

    //glColor3f(1,0,0);
    //drawSquajre(10);
	drawSixSquare();

    //drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);
	//drawOneEight_Sphere(50,20,20);
	//drawOneForth_Cylinder(50,30,30,30);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	u = Vector3D(0,0,1);
	r = Vector3D(-1/sqrt(2),1/sqrt(2),0);
	l = Vector3D(-1/sqrt(2),-1/sqrt(2),0);
	pos = Vector3D(120,120,50);
    

	
	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
