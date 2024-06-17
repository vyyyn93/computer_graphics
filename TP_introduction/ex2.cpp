#include <GL/glut.h>
void Render()
{
    // Dimensions de la fenêtre
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glViewport(0, height/2, width/2, height/2);
    glScissor(0, height/2, width/2, height/2);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(1.f, 1.f, 0.f, 1.f); // Jaune
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glEnd();

    glViewport(width/2, height/2, width/2, height/2);
    glScissor(width/2, height/2, width/2, height/2);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(1.f, 0.f, 0.f, 1.f); // Rouge
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0.5f, 0.5f);
    glEnd();
    
    glViewport(0, 0, width/2, height/2);
    glScissor(0, 0, width/2, height/2);
    glClearColor(0.f, 1.f, 1.f, 1.f);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(1.f, 0.f, 1.f, 1.f); // Magenta
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(-0.5f, 0.0f);
    glVertex2f(0.5f, 0.0f);
    glEnd();
    
    glViewport(width/2, 0, width/2, height/2);
    glScissor(width/2, 0, width/2, height/2);
    glClearColor(0.f, 1.f, 1.f, 1.f);
    glEnable(GL_SCISSOR_TEST);
    glClearColor(1.f, 1.f, 1.f, 1.f); // gris
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();
}
// la ‘callback’ executee par glutDisplayFunc()
void Display()
{
 Render();
 glutSwapBuffers();
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(960,540);
    glutCreateWindow("Triangle");
    glutDisplayFunc(Display);
    glutMainLoop();
    return 0;
}