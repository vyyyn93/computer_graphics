#include <GL/glut.h>

void Render()
{
    // Effacer l'écran avec la couleur jaune
    glClearColor(1.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Activer le mode projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Définir le repère avec gluOrtho2D()
    gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT));

    // Changer au mode modèle-vue
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Dessiner le triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); // Rouge
    glVertex2f(200.0f, 200.0f); // Coin supérieur
    glVertex2f(400.0f, 200.0f); // Coin supérieur droit
    glVertex2f(300.0f, 400.0f); // Coin inférieur
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
