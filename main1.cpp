#include <iostream>
#include <stdlib.h>
#include <math.h>
//#include<omp.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;




#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <OpenAL/alut.h>
#else
#include <GL/glut.h>
#include <AL/al.h>
#include <AL/alut.h>
#endif

#include "imageloader.h"

const double PI = atan(1)*4;

GLUquadricObj *quadratic;   //Necessary to draw cylinders with glu
GLuint _textureId;          //The OpenGL id of the texture
int ms = 20;               //Time between each screen refresh
float anglemax = 75;       //Maximum value for the angle
float start_angle=0 ;
//float increment_max = 6.5;  //Maximum value for angle increments
float angle = 0;  //The left sphere suspended begin with the most valid angle
bool clockwise = false;     //and will move in counter-clockwise direction
float increment=1.0;

//Spheres
int areas = 5;            //Total Number of areas
int motion = 1;           //Number of areas that are in motion
float d_sphere = 1.0;       //Diameter of each sphere
float cube = 0.125;   //Size of the embedded nature of each sphere

//Base                      //Size for wood-based madera
float base_X = 7.5;
float base_Y = 0.8;
float base_Z = 5.5;

float dist_sphere_base = 1.0;  //Distance between the spheres and the base when the angle is 0

//poles
float p_radius = 0.125;    //Radius of each cylinder
GLint slices = 32;          //Number of subdivisions around the z axis to draw each cylinder
GLint stacks = 32;          //Number of divisions along the z axis to draw each cylinder

float pX = 6.5;       // Size rectangular caramel
float pY = 5;      // I consists of pipes (including
float pZ = 4.5;       // the diameter of each tube)

//The length of the yarn is calculated based on the size of the tubes,
//the diameter of the spheres and the distance between the spheres and the base
float rope = pY-p_radius-dist_sphere_base-d_sphere-cube/2;

//Camara
float c_pos_x = 0.0;
float c_pos_y = 0.5;
float c_pos_z = 15.0;
float c_rot_x = 6.0;
float c_rot_y = 40.0;
float c_rot_z = 0.0;

//Axles
bool draw_axes = false;

bool mirror = false;

//Menus
static int main_menu, total_spheres, spheres_in_motion;

void draw_base();
void handleResize(int w, int h);






GLfloat UpwardsScrollVelocity = -10.0;
float view=20.0;

char quote[6][80];
int numberOfQuotes=0;

int s=1;







/*----------------------------------------------------------------------------------------------------------------------
					         LOADING TEXTURES
-----------------------------------------------------------------------------------------------------------------------*/					
GLuint loadTexture(Image* image) 
	{
    	 GLuint textureId;
    	 glGenTextures(1, &textureId); /*Returns one texture name to textureId*/
    	 
    	 glBindTexture(GL_TEXTURE_2D, textureId); /*glBindTexture lets you create or use a named texture. When a texture 
    	 					    is bound to a target, the previous binding for that target is 
    	 					    automatically broken. */
    	 
    	 
    	 /*specify a two-dimensional texture image*/
    	 glTexImage2D(GL_TEXTURE_2D,			/*Specifies the target texture*/
         	      0,				/*Specifies the level-of-detail number. Level 0 is the base image level.*/
         	      GL_RGB,				/*Specifies the number of color components in the texture*/
         	      image->width, image->height,	/*Specifies the height & width of the texture image */
        	      0,				/*border-this value must be 0. */
         	      GL_RGB,				/*Specifies the format of the pixel data*/
         	      GL_UNSIGNED_BYTE,			/*Specifies the data type of the pixel data*/
         	      image->pixels);			/*Specifies a pointer to the image data in memory.*/
	 return textureId;
	}
/*---------------------------------------------------------------------------------------------------------------------
				    	   INITIALIZING RENDERING PARAMETERS
---------------------------------------------------------------------------------------------------------------------*/
void initRendering() 
	{
	 glEnable(GL_DEPTH_TEST); /*If enabled,do depth comparisons and update the depth buffer*/
 	 
 	 glEnable(GL_LIGHTING); /*If enabled and no vertex shader is active,
                                  use the current lighting parameters to compute the vertex color or index.
                                  Otherwise, simply associate the current color or index with each
                                   vertex.*/

	 glEnable(GL_LIGHT0);   /*If enabled, include light i in the evaluation of the lighting equation.*/
	 
	 glEnable(GL_NORMALIZE); /*If enabled and no vertex shader is active,
                        	  normal vectors are normalized to unit length
	                          after transformation and before lighting. */
	 
	 glEnable(GL_COLOR_MATERIAL); /*If enabled,have one or more material parameters track the current color. */
	 
	 glShadeModel(GL_SMOOTH); /*Specifies a symbolic value representing a shading technique. Accepted values are GL_FLAT and GL_SMOOTH.
		                    The initial value is GL_SMOOTH.*/
    
    	 quadratic=gluNewQuadric(); /*To create a quadrics object*/
	 
	 gluQuadricNormals(quadratic, GLU_SMOOTH); /*For the quadrics object qobj, normals is one of GLU_NONE (the default), GLU_FLAT, or GLU_SMOOTH.*/
	 
	 gluQuadricTexture(quadratic, GL_TRUE);  /*For the quadrics object qobj, textureCoords is either GL_FALSE (the default) or GL_TRUE. If the value of 							  textureCoords is GL_TRUE, then texture coordinates are generated for the quadrics object.*/
    
    	 Image* image = loadBMP("madera.bmp");
    	 
    	 _textureId = loadTexture(image);
         delete image;
         
         
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         //glEnable(GL_LIGHTING);               
        // glEnable(GL_LIGHT0);
	}

/*------------------------------------------------------------------------------------------------------------------
				POSITIONS THE CAMERA
------------------------------------------------------------------------------------------------------------------*/				

void cam_pos()
	 {
    	  glTranslatef(-c_pos_x, -c_pos_y, -c_pos_z);		/*glTranslate produces a translation by (x, y, z ). The current matrix is multiplied by this 									 translation matrix, with the product replacing the current matrix*/ 
    	  glRotatef(c_rot_x, 1.0, 0.0, 0.0);
    	  glRotatef(c_rot_y, 0.0, 1.0, 0.0);
    	  glRotatef(c_rot_z, 0.0, 0.0, 1.0);
    	  
	 /* glRotate produces a rotation of angle degrees around the vector (x, y, z). The current matrix is multiplied by a rotation matrix with the product 			replacing the current matrix*/
	}

/*------------------------------------------------------------------------------------------------------------------
				FUNCTION TO CONVERT DEGREES TO RADIANS
-------------------------------------------------------------------------------------------------------------------*/
double toRad(double degree) 
	{
    	 return degree*PI/180;
	}


/*-------------------------------------------------------------------------------------------------------------------
		VELOCITY
-------------------------------------------------------------------------------------------------------------------*/
void timeTick(void)
{
    if (UpwardsScrollVelocity< -600)
        view-=0.5;
    if(view < 0) {view=20; UpwardsScrollVelocity = -10.0;}
    //  exit(0);
    UpwardsScrollVelocity -= 0.5;
  glutPostRedisplay();

}


/*-----------------------------------------------------------------------------------------------
		RENDER IN DISPLAY
--------------------------------------------------------------------------------------------------*/		

void RenderToDisplay()
{
    int l,lenghOfQuote, i;

    glTranslatef(0.0, -100, UpwardsScrollVelocity);
    glRotatef(-20, 1.0, 0.0, 0.0);
    glScalef(0.1, 0.1, 0.1);



    for(  l=0;l<numberOfQuotes;l++)
    {
        lenghOfQuote = (int)strlen(quote[l]);
        glPushMatrix();
        glTranslatef(-(lenghOfQuote*37), -(l*200), 0.0);
        for (i = 0; i < lenghOfQuote; i++)
        {
            glColor3f((UpwardsScrollVelocity/10)+300+(l*10),(UpwardsScrollVelocity/10)+300+(l*10),0.0);
            glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[l][i]);
        }
        glPopMatrix();
    }

}
/*----------------------------------------------------------------------------------------------------------------------
		START SCREEN
-----------------------------------------------------------------------------------------------------------------------*/		

void start()
	{
	 strcpy(quote[0],"NEWTON'S CRADLE");
         strcpy(quote[1],"+/- to set the start position ");
         strcpy(quote[2],"PG_DOWN  to start animation ");
         strcpy(quote[3],"PG_UP to reset ");
         strcpy(quote[4],"END to speed up animation");
         strcpy(quote[5],"HOME to slow down animation");
         strcpy(quote[7],"PRESS F1 TO START !!!!");
         numberOfQuotes=8;
         glClear(GL_COLOR_BUFFER_BIT);
         glLoadIdentity();
         gluLookAt(0.0, 30.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
          RenderToDisplay();
           glutSwapBuffers();
         glutIdleFunc(timeTick);
         return ; 
         }





/*----------------------------------------------------------------------------------------------------
				      FUNCTION TO DRAW THE AXES
-----------------------------------------------------------------------------------------------------*/				      
void DrawAxes(void) 
	{
    	//glDisable(GL_COLOR_MATERIAL);

glDisable(GL_LIGHTING);
    	 glLineWidth(5);    /*changes width of the line*/
    	 	//X Axis - Red
    	 glColor3f(1.0, 0.0, 0.0);
	 glBegin(GL_LINES);
           glVertex3f(-500.0,0.0,0.0);
           glVertex3f(500.0,0.0,0.0);
         glEnd();
    
    		//Y axis - Green
    	 glColor3f(0.0, 1.0, 0.0);
    	 glBegin(GL_LINES);
           glVertex3f(0.0,-500.0,0.0);
           glVertex3f(0.0,500.0,0.0);
    	 glEnd();
    
    		//Axis Z - Blue
    	 glColor3f(0.0, 0.0, 1.0);
    	 glBegin(GL_LINES);
           glVertex3f(0.0,0.0,-500.0);
           glVertex3f(0.0,0.0,500.0);
    	 glEnd();
    	 
    	 glLineWidth(1);
    	// glEnable(GL_COLOR_MATERIAL);
    	glEnable(GL_LIGHTING);

	}



/*float x = 0 has an implicit typecast from int to float.
float x = 0.0f does not have such a typecast.
float x = 0.0 has an implicit typecast from double to float. */



/*There is a stack of matrices for each of the matrix modes.
  In GL_MODELVIEW mode,the stack depth is at least 32.
  In the other modes,GL_COLOR, GL_PROJECTION, and GL_TEXTURE, the depth is at least 2.
  The current matrix in any mode is the matrix on the top of the stack for that mode.*/
 /*-----------------------------------------------------------------------------------------------------------
			FUNCTION TO DRAW POLES
------------------------------------------------------------------------------------------------------------------*/
void draw_poles()
	{
    	 





//glEnable(GL_COLOR_MATERIAL);



	 glPushMatrix();
    	 glTranslatef(0.0f, pY/2-d_sphere/2-dist_sphere_base, 0.0f); //Geometric center
    
    	 glColor3f(0.69f, 0.69f, 0.69f);
	

	//glColor3f(1, 1, 1);

	
    
    	 //lighting
    	GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	 GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	 GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	 GLfloat mat_shininess[] = { 300.0 };
	 
	 /*face
               Specifies which face or faces are being updated.Must be GL_FRONT_AND_BACK.
           pname
                Specifies the material parameter of the face or faces that is being updated. Must be one of
                    GL_AMBIENT,GL_DIFFUSE,GL_SPECULAR,GL_EMISSION,
                    GL_SHININESS, or GL_AMBIENT_AND_DIFFUSE.
           params
                Specifies a pointer to the value or values that pname will be set to.*/
	 
	  
	 glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient); /*contains four fixed-point or floating-point values that specify the ambient RGBA reflectance of the 							    material*/

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);/*contains four fixed-point or floating-point values that
					                    specify the diffuse RGBA reflectance of the material*/

	 glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);/*contains four fixed-point or floating-point values that
					                    specify the specular RGBA reflectance of the material*/

	 glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);/*is a single fixed-point or floating-point value that
					                    specifies the RGBA specular exponent of the material*/


	
	    
    	 /*quad
               Specifies the quadrics object (created with gluNewQuadric).
           base
               Specifies the radius of the cylinder at z = 0.
           top
               Specifies the radius of the cylinder at z = height.
           height
               Specifies the height of the cylinder.
           slices
               Specifies the number of subdivisions around the z axis.
           stacks
               Specifies the number of subdivisions along the z axis. */

    	
    
    		//front upper pole
    	 glPushMatrix();
    	 glTranslatef(-pX/2, pY/2-p_radius, pZ/2-p_radius);
    	 glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    	 gluCylinder(quadratic,p_radius,p_radius,pX,slices, stacks);
    	 glPopMatrix();
    
  		  //Back Upper pole
	 glPushMatrix();
	 glTranslatef(-pX/2, pY/2-p_radius, -(pZ/2-p_radius));
	 glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	 gluCylinder(quadratic,p_radius,p_radius,pX,slices, stacks);
	 glPopMatrix();
	    
		    //left Front
	 glPushMatrix();
    	 glTranslatef(-(pX/2-p_radius), pY/2-p_radius, pZ/2-p_radius);
    	 glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    	 gluCylinder(quadratic,p_radius,p_radius,pY-p_radius,slices, stacks);
    	 glPopMatrix();
    
    		//left Rear
    	 glPushMatrix();
    	 glTranslatef(-(pX/2-p_radius), pY/2-p_radius, -(pZ/2-p_radius));
    	 glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    	 gluCylinder(quadratic,p_radius,p_radius,pY-p_radius,slices, stacks);
    	 glPopMatrix();
    
  		  //right Forward
   	 glPushMatrix();
    	 glTranslatef((pX/2-p_radius), pY/2-p_radius, pZ/2-p_radius);
    	 glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    	 gluCylinder(quadratic,p_radius,p_radius,pY-p_radius,slices, stacks);
    	 glPopMatrix();
    
    		//right Back
    	 glPushMatrix();
    	 glTranslatef((pX/2-p_radius), pY/2-p_radius, -(pZ/2-p_radius));
    	 glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    	 gluCylinder(quadratic,p_radius,p_radius,pY-p_radius,slices, stacks);
    	 glPopMatrix();
    
	 glPopMatrix();

//glDisable(GL_COLOR_MATERIAL);
	
	}	

/*---------------------------------------------------------------------------------------------------------------
					FUNCTION TO DRAW SPHERES AND WIRES
---------------------------------------------------------------------------------------------------------------*/					
void sphere_ropes(float angle) 
	{
   	// glEnable(GL_COLOR_MATERIAL);
//
	glPushMatrix();
    
    	 glRotatef(angle, 0.0f, 0.0f, 1.0f);
    	 glTranslatef(0.0f, -rope, 0.0f);
    
    	 glColor3f(0.8, 0.8, 0.8);
    	 //glColor3f(0.0, 0.0, 1.0);
    		//lighting
    	 GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
    	 GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    	 GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    	 GLfloat mat_shininess[] = { 300.0 };
    	 glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    	 glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    	 glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    	 glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    //Tying To Draw Cube
     glutSolidCube(cube);
     
     
     	/*radius
    		The radius of the sphere. 
	 slices	
	        The number of subdivisions around the Z axis (similar to lines of longitude). 
	 stacks
    		The number of subdivisions along the Z axis (similar to lines of latitude). */
    
    		//draw Sphere
    	 glPushMatrix();
    	 glTranslatef(0.0f, -(d_sphere/2), 0.0f);
    	 glutSolidSphere(d_sphere/2, 20, 20);
    	 glPopMatrix();
    
    	 glRotatef(-angle, 0.0f, 0.0f, 1.0f);
    
    		//draw wire
    	 float distX = sin(toRad(angle))*rope;
    	 float distY = cos(toRad(angle))*rope;
    	 float distZ = pZ/2-p_radius;
    
    glDisable(GL_LIGHTING);
    	 glColor3f(1.0f, 1.0f, 1.0f);
    	 glBegin(GL_LINES);
        	//Back wires
         glVertex3f(0.0f, 0.0f, 0.0f);
         glVertex3f(-distX, distY, -distZ);
        	//Front Wires
         glVertex3f(0.0f, 0.0f, 0.0f);
         glVertex3f(-distX, distY, distZ);
    	 glEnd();
    glEnable(GL_LIGHTING);
    	glPopMatrix();

//glDisable(GL_COLOR_MATERIAL);
	}
/*-----------------------------------------------------------------------------------------------------------------
		FUNCTION TO FIND THE POSITION OF THE SPHERES AND THE STRIINGS
-----------------------------------------------------------------------------------------------------------------*/					
void draw_spheres()
	 {
    	 glPushMatrix();
    	 glTranslatef(0.0f, pY-p_radius-dist_sphere_base-d_sphere/2, 0.0f); //Height of the tubes
    
    	 for(int i=1; i<=areas; i++) 
    	    {
             glPushMatrix();
             glTranslatef(-areas/2.0f-d_sphere/2.0f+i*d_sphere,0.0f,0.0f); //Center X
             if(i<=motion && angle<0)
                sphere_ropes(angle);
        else if(i>areas-motion && angle>0)
            sphere_ropes(angle);
        else
            sphere_ropes(0);
        glPopMatrix();
    }

    glPopMatrix();
}









/*---------------------------------------------------------------------------------------------------------
       FUNCTION FOR MIRROR FLOOR
-------------------------------------------------------------------------------------------------------*/       


void drawFloor()
{
 // glEnable(GL_COLOR_MATERIAL);
	glPushMatrix();
    glTranslatef(0.0f, -(d_sphere/2+dist_sphere_base+base_Y/2), 0.0f);
   
     GLfloat lightAmbient[] = {0.5f, 0.5f ,0.5f, 1.0f};
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
 
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
     
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Begin drawing the floor
    glBegin(GL_QUADS);
         
    glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
    glNormal3f(0.0f, 1.0f, 0.0f);   
    
     glVertex3f(-base_X/2, base_Y/2, base_Z/2); //Bottom Left of Texture & Plane
          glVertex3f(base_X/2, base_Y/2, base_Z/2); // Bottom Right of Texture & Plane
          glVertex3f(base_X/2, base_Y/2, -base_Z/2); // Top Right of Texture & Plane
           glVertex3f(-base_X/2, base_Y/2, -base_Z/2);
     
    glEnd();
    glPopMatrix();
    
    //glDisable(GL_COLOR_MATERIAL);

}





/*-----------------------------------------------------------------------------------------------------
	FUNCTION FOR MIRROR EFFECT
-----------------------------------------------------------------------------------------------------*/	


void draw_mirror()   	  
    	 { 
    	  

	  glEnable(GL_STENCIL_TEST);   // If enabled,do stencil testing and update the stencil buffer.
          glColorMask(0, 0, 0, 0);    //enable and disable writing of frame buffer color components
          glDisable(GL_DEPTH_TEST);   //disable depth test
          glStencilFunc(GL_ALWAYS, 1, 1);   // Make pixels in the stencil buffer be set to 1 when the stencil test passes
	  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);    // Set all of the pixels covered by the floor to be 1 in the stencil buffer
	  
	  
	  
	  
	  drawFloor();    // Enable drawing colors to the screen
	  
	  
	  
	  
	  
	  
	  
	  
	  
          glColorMask(1, 1, 1, 1);    // Enable depth testing
	  glEnable(GL_DEPTH_TEST);    // Make the stencil test pass only when the pixel is 1 in the stencil buffer
	  glStencilFunc(GL_EQUAL, 1, 1);    // Make the stencil buffer not change
	  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);         //Draw the cube, reflected vertically, at all pixels where the stencil
	 
	 
	 
	  glPushMatrix();   //buffer is 1
      	  glScalef(1, -1, 1);
      	  
      	  
    	  glTranslatef(0.0f, (d_sphere/2+dist_sphere_base+base_Y/2), 0.0f);
          draw_poles(); 
          draw_spheres();
          if(draw_axes)
            DrawAxes();
    	  glPopMatrix();
     
    // Disable using the stencil buffer
    glDisable(GL_STENCIL_TEST);
    
     
    // Blend the floor onto the screen
    glEnable(GL_BLEND);   
       drawFloor();
      
     
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	glDisable( GL_LIGHTING );
	
	//glColor4f( 1.0f, 1.0f, 1.0f, .2f );
        
      drawFloor();
    
         glEnable( GL_LIGHTING );
         glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
    glDisable(GL_BLEND);
    
    
      glPushMatrix();
    glTranslatef(0.0f, -(d_sphere/2+dist_sphere_base+base_Y/2), 0.0f);   
    
    glDisable(GL_LIGHTING);
    glColor3f( 0.0f, 0.0f, 0.0f);
   // glEnable(GL_LIGHTING);

 //   float mat_ambient[]={0.0,0.0,0.0,1.0};
  //  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glLineWidth(7);
	glBegin( GL_LINE_LOOP );
	    glNormal3f(0.0f, 1.0f, 0.0f);   
    
     glVertex3f(-base_X/2, base_Y/2, base_Z/2); //Bottom Left of Texture & Plane
     glVertex3f(base_X/2, base_Y/2, base_Z/2); // Bottom Right of Texture & Plane
     glVertex3f(base_X/2, base_Y/2, -base_Z/2); // Top Right of Texture & Plane
     glVertex3f(-base_X/2, base_Y/2, -base_Z/2);
	glEnd( );
	glLineWidth(1); 
glEnable(GL_LIGHTING);
	glPopMatrix();
    // draw_base();
     
      //glFlush( );
    	  
    	

	}       

/*-----------------------------------------------------------------------------------------------------------
				FUNCTION TO DRAW THE BASE PLATFORM
-----------------------------------------------------------------------------------------------------------*/				
void draw_base()
	{
 	 if(mirror)
 	  draw_mirror();
 	 
 	 glPushMatrix();  /*glPushMatrix pushes the current matrix stack down by one, duplicating the current matrix.
            		     That is,after a glPushMatrix call,the matrix on top of the stack is identical to the one below it.*/

    	 glTranslatef(0.0f, -(d_sphere/2+dist_sphere_base+base_Y/2), 0.0f); //Geometric Center Base
    
    	 glEnable(GL_TEXTURE_2D);	/*If enabled and no fragment shader is active, two-dimensional texturing is performed*/

    	 glBindTexture(GL_TEXTURE_2D, _textureId); /*glBindTexture lets you create or use a named texture. When a texture 
    	 					    is bound to a target, the previous binding for that target is 
    	 					    automatically broken. */
    
    
    	 /*Texture mapping is a technique that applies an image onto an object's surface
            as if the image were a decal or cellophane shrink-wrap*/
    
    	 
    	 /*	target
                    Specifies the target texture of the active texture unit,which must be either GL_TEXTURE_2D or GL_TEXTURE_CUBE_MAP.
                pname
                    Specifies the symbolic name of a texture parameter.pname can be one of the following:
                    GL_TEXTURE_MIN_FILTER,GL_TEXTURE_MAG_FILTER,
                    GL_TEXTURE_WRAP_S, or GL_TEXTURE_WRAP_T.
                params
                    Specifies a pointer to an array where the value of pname is stored.
                    params supplies a function for minifying/maxifying the texture*/
                
    	 /*The texture minifying function is used whenever the pixel being textured maps to an area greater than one texture element. */
    	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	 
    	 
    	 /*The texture magnification function is used when the pixel being textured maps to an area less than or equal to one texture element*/
    	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	 
    	 /*GL_LINEAR : Returns the weighted average of the four texture elements that are closest to the center of the pixel being textured.*/
                                    
    
    	 glColor3f(1.0f,1.0f,1.0f);
    
    	 
        
        if(!mirror)	//Front face
      {   glBegin(GL_QUADS);
           glNormal3f(0.0, 1.0f, 0.0f);	/*Specify the x, y, and z cordinates of the new current normal.The initial value of the 
         				  current normal is the unit vector, (0, 0, 1).*/

                /*glTexCoord specifies texture coordinates in one,two, three, or four dimensions*/

           glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_X/2, base_Y/2, base_Z/2); //Bottom Left of Texture & Plane
           glTexCoord2f(1.0f, 0.0f); glVertex3f(base_X/2, base_Y/2, base_Z/2); // Bottom Right of Texture & Plane
           glTexCoord2f(1.0f, 1.0f); glVertex3f(base_X/2, base_Y/2, -base_Z/2); // Top Right of Texture & Plane
           glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_X/2, base_Y/2, -base_Z/2); // Top Left of Texture & Plane
          glEnd(); 
       }
       
           
        	//Bottom face
           
          glBegin(GL_QUADS);
           glNormal3f(0.0, -1.0f, 0.0f);
           glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_X/2, -base_Y/2, base_Z/2);
           glTexCoord2f(1.0f, 0.0f); glVertex3f(base_X/2, -base_Y/2, base_Z/2);
           glTexCoord2f(1.0f, 1.0f); glVertex3f(base_X/2, -base_Y/2, -base_Z/2);
           glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_X/2, -base_Y/2, -base_Z/2);
        
        	//Left Side
           glNormal3f(-1.0, 0.0f, 0.0f);
           glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_X/2, -base_Y/2, -base_Z/2);
           glTexCoord2f(1.0f, 0.0f); glVertex3f(-base_X/2, -base_Y/2, base_Z/2);
           glTexCoord2f(1.0f, 1.0f); glVertex3f(-base_X/2, base_Y/2, base_Z/2);
           glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_X/2, base_Y/2, -base_Z/2);
        
        	// Right Side
           glNormal3f(-1.0, 0.0f, 0.0f);
           glTexCoord2f(0.0f, 0.0f); glVertex3f(base_X/2, -base_Y/2, -base_Z/2);
           glTexCoord2f(1.0f, 0.0f); glVertex3f(base_X/2, -base_Y/2, base_Z/2);
           glTexCoord2f(1.0f, 1.0f); glVertex3f(base_X/2, base_Y/2, base_Z/2);
           glTexCoord2f(0.0f, 1.0f); glVertex3f(base_X/2, base_Y/2, -base_Z/2);
        
        	//Front Side
           glNormal3f(0.0, 0.0f, 1.0f);
           glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_X/2, -base_Y/2, base_Z/2); 
           glTexCoord2f(1.0f, 0.0f); glVertex3f(base_X/2, -base_Y/2, base_Z/2);
           glTexCoord2f(1.0f, 1.0f); glVertex3f(base_X/2, base_Y/2, base_Z/2);
           glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_X/2, base_Y/2, base_Z/2);
        	
        	//Back Side
           glNormal3f(0.0, 0.0f, 1.0f);
           glTexCoord2f(0.0f, 0.0f); glVertex3f(-base_X/2, -base_Y/2, -base_Z/2); 
           glTexCoord2f(1.0f, 0.0f); glVertex3f(base_X/2, -base_Y/2, -base_Z/2);
           glTexCoord2f(1.0f, 1.0f); glVertex3f(base_X/2, base_Y/2, -base_Z/2);
           glTexCoord2f(0.0f, 1.0f); glVertex3f(-base_X/2, base_Y/2, -base_Z/2);
     	 glEnd();
    
    glDisable(GL_TEXTURE_2D);    
    glPopMatrix();
}















/*---------------------------------------------------------------------------------------------
				FUNCTION TO DRAW THE SCENE
---------------------------------------------------------------------------------------------*/
void drawScene()
	 {
    	   
    	  if(s==1){
    	     // glLineWidth(10);
    	    // glDisable(GL_LIGHTING);  
    	    // glutReshapeFunc(reshape); 	 	
    	     start();
    	    // glEnable(GL_LIGHTING);   
    	     // glLineWidth(1);	 
    	     }
    	  else   
    	   {
    	   initRendering();
    	  // glutReshapeFunc(handleResize);
    	  glClearStencil(0); 
    	  glClearDepth(1.0f);  
    	  glClearColor(0.0f,0.0f,0.0f,1.0f); 
    	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT); 
    	  
    	  glMatrixMode(GL_MODELVIEW); /*Specifies which matrix stack is the target for subsequent matrix operations.
  			                  Three values are accepted:GL_MODELVIEW,GL_PROJECTION, and GL_TEXTURE.The initial value is GL_MODELVIEW*/
    	  glLoadIdentity(); /*replaces the currnet matrix with identity matrix*/
    
    	  /** lighting **/
    	  GLfloat ambientLight[] = {0.4f, 0.4f, 0.4f, 1.0f};		/*configure light sources (rgba)*/
    	  GLfloat diffuseLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    	  GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    	  GLfloat lightPos[] = {-c_pos_x+pX, -c_pos_y+pY, -c_pos_z+pZ, 1.0f};	/*directional or positional (xyzw), w=0(dirctional), w=1(potitional*/    	  
    	  
    	//  GLfloat lightPos[] = {0, 10, 0, 0.0f};
    	  
    	   glLightfv(GL_LIGHT0, GL_POSITION, lightPos);		/*potion*/
    	  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);	/*light source*/
    	  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    	  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    	
    	  /** Viewing **/
    	  cam_pos();
    
   	  /** Modeling **/
    	  if(draw_axes)
            DrawAxes();
           draw_base();
          //draw_mirror();
    	  draw_poles();
    	  draw_spheres();
	}
    	  glutSwapBuffers();
    	  
    	 } 
    	 
    	  
    	  
    	  
    	  
    	  
 

/*---------------------------------------------------------------------------------------------------------------
				PLAY
------------------------------------------------------------------------------------------------------------------*/					
int *play()
	{
	 ALuint helloBuffer, helloSource;
  //alutInit (&argc, argv);
  helloBuffer = alutCreateBufferFromFile ("1.wav");
  alGenSources (1, &helloSource);
  alSourcei (helloSource, AL_BUFFER, helloBuffer);
  alSourcePlay (helloSource);
  if(increment >=2.5 && increment <= 3.0)
    alutSleep (1); //you need to put here the length in seconds of the file
  else if(increment >=0.1 && increment <= 1.0)
    alutSleep(0.0000000000000000001);
  else
    alutSleep(0.01);  
  //alutExit ();
  return EXIT_SUCCESS;
  }

/*---------------------------------------------------------------------------------------------------------------
					FUNCTION TO SWING THE SPHERES
---------------------------------------------------------------------------------------------------------------*/					

void update(/*int value*/)
	 {
    	  #pragma omp parallel shared(angle,start_angle)
    	  { 
    	    #pragma omp sections 
    	     {
    	      #pragma omp section
    	       
    	  if(start_angle==0)
    	    {
    	     glutIdleFunc(NULL);
    	     return;
    	    } 
    	 
    	  if(clockwise and angle<=start_angle) 
    	    clockwise=false;
    	  else if(!clockwise and angle>=-start_angle) 
                 clockwise=true;
    
    
    	  if(clockwise)
            angle -= increment;
    	  else
            angle += increment;
            
          glutPostRedisplay();
        
            
         
         #pragma omp section
             if((int)angle==0 && areas!=1)
                 play();
          
          
       }  
        // glutTimerFunc(ms, update, 0);
	}
 }	
/*----------------------------------------------------------------------------------------------------------------
				FUNCTION TO HANDLE SPECIAL KEYBOARD INPUTS
----------------------------------------------------------------------------------------------------------------*/				
void handleSpecialKeys (int key, int x, int y) 
	{
    	 int inc = 2.0;
    	 switch(key)
    	      {
               case GLUT_KEY_RIGHT     : c_rot_y += inc;
				         glutPostRedisplay();
            			         break;
            
               case GLUT_KEY_LEFT      : c_rot_y -= inc;
            	              	         glutPostRedisplay();
			                 break;
        
	       case GLUT_KEY_UP        :if(c_rot_x>=-12 )
	       				  {
	       				   c_rot_x -= inc; 
			                   glutPostRedisplay();
			                 }
			                 break;
            
               case GLUT_KEY_DOWN      :if(c_rot_x<=188)
	       				  {
               			           c_rot_x += inc; 
           			           glutPostRedisplay();
           			          } 
           			         break;
           	
               case GLUT_KEY_PAGE_DOWN : glutIdleFunc(update);			    
   	             			 break;   
    	       
    	       case GLUT_KEY_PAGE_UP   : angle=0;
    	       				 start_angle=0;
    	       				 clockwise = false;
    	       				 increment=1;
    	       				 glutPostRedisplay();			    
   	             			 break;   
   	             			 
               case GLUT_KEY_END       : if(increment<=3)
               				 {	increment+=0.1; 	
               				 //glutPostRedisplay();              			 
    	     				// break;
    	     				 }
    	     				 break;
	
	       case GLUT_KEY_HOME      : if(increment>=0)
	       				      {
	       				       increment-=0.1;
	       				      // glutPostRedisplay();
	       				     //  break;
	       				      }
	       				 break;     
	       				 
	      case GLUT_KEY_F1		: s=0;
	      	 		          break;        				  			
	    }
     	}
/*-------------------------------------------------------------------------------------------------------------
				FUNCTION TO HANDLE GENERAL KEYBOARD CHARACTERS
-------------------------------------------------------------------------------------------------------------*/				
void keys(unsigned char key, int x, int y)
	{
	 switch(key)
	       {
	       	case '+' : while(!clockwise && angle>=-anglemax)
	       			{
	       		   	 angle=angle-0.7 ;
	       		   	 glutPostRedisplay();
	       		   	 start_angle=angle;
	       		   	 break; 
	       		   	} 
	       	           break;
	       	           
	       case '-' :  while(angle<0)
	       			{
	       		         angle = angle+0.7;
	       		         glutPostRedisplay();
	       		         start_angle=angle; 
	       	                 break;	           				
	      			}
		       	    break;
		}  
	}	     
/*-------------------------------------------------------------------------------------------------------------
				FUNCTION TO HANDLE CHANGES IN WINDOW SIZE
-------------------------------------------------------------------------------------------------------------*/				
void handleResize(int w, int h)
	 {
 		   /** Projection **/
    	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
	  /*fovy
                Specifies the field of view angle, in degrees, in the y direction.
            aspect
                Specifies the aspect ratio that determines the field of view in the x direction.
            zNear
                Specifies the distance from the viewer to the near clipping plane (always positive).
            zFar
                Specifies the distance from the viewer to the far clipping plane (always positive).*/
                
    	  gluPerspective(45.0, (float)w / (float)h, 1.0, 3200.0);
   	   	  
   	   	  
   	   	  
   	   	  
   	   	  /** ViewPort **/
   	   	  
   	  /*x, y
                Specify the lower left corner of the viewport rectangle,in pixels. The initial value is (0,0).
            width, height
                Specify the width and height of the viewport. When a GL context is first attached to a window,
                    width and height are set to the dimensions of that window.*/ 	  
   	   	  
    	  glViewport(0, 0, w, h);
    	  glMatrixMode(GL_MODELVIEW);
	 }
/*------------------------------------------------------------------------------------------------------
					MAIN MENU HANDLER
------------------------------------------------------------------------------------------------------*/					
void handle_main_menu(int m) 
	{
    	 switch(m) 
    	 	{
        	 case 0: exit(0);
        	 case 1: draw_axes = !draw_axes;
        	 	 glutPostRedisplay();
        	 	 break; 
        	 case 2: mirror= !mirror;
        	         glutPostRedisplay();
        	         break;
    		}
	}
/*--------------------------------------------------------------------------------------------------------
				   TOTAL SPHERES SUB MENU HANDLER
--------------------------------------------------------------------------------------------------------*/				   

void handle_total_spheres(int m) 
	{ 
    	 areas = m-10;
    	 glutPostRedisplay();
	}
/*--------------------------------------------------------------------------------------------------------
			 		MOTION SUB MENU HANDLER
---------------------------------------------------------------------------------------------------------*/			 		
void handle_spheres_in_motion(int m) 
	{ 
	 motion = m-20;
	 glutPostRedisplay();
	}
/*--------------------------------------------------------------------------------------------------------
					MENU CREATION
---------------------------------------------------------------------------------------------------------*/
void createMenu() 
	{
    	 total_spheres = glutCreateMenu(handle_total_spheres);
	 glutAddMenuEntry("1", 11);
    	 glutAddMenuEntry("2", 12);
    	 glutAddMenuEntry("3", 13);
    	 glutAddMenuEntry("4", 14);
    	 glutAddMenuEntry("5", 15);		
    	 glutAddMenuEntry("6", 16);
    	 glutAddMenuEntry("7", 17);
    
    	 spheres_in_motion = glutCreateMenu(handle_spheres_in_motion);
    	 glutAddMenuEntry("1", 21);
    	 glutAddMenuEntry("2", 22);
    	 glutAddMenuEntry("3", 23);
    	 glutAddMenuEntry("4", 24);
    	 glutAddMenuEntry("5", 25);
    	 glutAddMenuEntry("6", 26);
    	 glutAddMenuEntry("7", 27);
    	 
    	 /*glutAddMenuEntry adds a menu entry to the bottom of the current menu. The string name will be displayed for the 		
    	 newly added menu entry. If the menu entry is selected by the user, the menu's callback will be called passing
    	 value as the callback's parameter. */
    
    	 main_menu = glutCreateMenu(handle_main_menu);
    	 glutAddSubMenu("Total Spheres", total_spheres);
    	 glutAddSubMenu("Spheres In Motion", spheres_in_motion);
    	 glutAddMenuEntry("Show Axes", 1);
    	 glutAddMenuEntry("Mirror", 2);
    	 glutAddMenuEntry("Exit", 0);
    	 glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
/*------------------------------------------------------------------------------------------------------
					MAIN FUNCTION
-------------------------------------------------------------------------------------------------------*/
int main(int argc, char** argv) 
	{
	 glutInit(&argc, argv);
	 
	 alutInit (&argc, argv);
	 
	 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH|GLUT_STENCIL);
	 glutInitWindowSize(800, 600);
	 glClearColor(0.0, 0.0, 0.0, 1.0);
	 glutCreateWindow("NEWTON'S CRADLE");
	
	
        // initRendering();
         createMenu();
    
         glutDisplayFunc(drawScene);
        // glutReshapeFunc(reshape);
         glutReshapeFunc(handleResize);
         glutSpecialFunc(handleSpecialKeys);
         glutKeyboardFunc(keys);
         glutMainLoop();
         return 0;
        
        }
        
        
        
        


  
     
