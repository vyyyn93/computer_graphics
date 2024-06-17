#include <GL/glut.h>

void Render()
{
    // Effacer l'écran avec la couleur jaune
    glClearColor(1.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Activer le mode projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0f, 1.0f, 0.1f, 1000.0f); // Angle de vue, ratio, near, far

    // Changer au mode modèle-vue
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Déplacer le triangle plus près de la caméra
    glTranslatef(0.0f, 0.0f, -5.0f);

    // Dessiner le triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); // Rouge
    glVertex3f(-1.0f, -1.0f, 0.0f); // Coin inférieur gauche
    glVertex3f(1.0f, -1.0f, 0.0f); // Coin inférieur droit
    glVertex3f(0.0f, 1.0f, 0.0f); // Sommet
    glEnd();

    glutSwapBuffers();
}

void Display()
{
    Render();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600); // Taille de la fenêtre
    glutCreateWindow("Triangle");
    glutDisplayFunc(Display);
    glutMainLoop();
    return 0;
}
