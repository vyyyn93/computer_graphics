#include <GL/glut.h>
#include <cmath>

// Variable globale pour stocker le temps écoulé au dernier rendu
int previousTime = 0;

void Render()
{
    glClearColor(1.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    // Dessiner le premier triangle (position ortho-centrée)
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); // Rouge
    glVertex2f(0.0f, 0.5f);

    glColor3f(0.0f, 0.0f, 1.0f); // Bleu
    glVertex2f(-0.5f, -0.5f);
    
    glColor3f(0.0f, 1.0f, 0.0f); // Vert
    glVertex2f(0.5f, -0.5f);
    glEnd();

    // Dessiner le deuxième triangle (rotation comme dans l'exercice 3.2)
    glTranslatef(0.5f, 0.f, 0.f); // Déplacer à droite
    glRotatef(-90.0f, 0.f, 0.f, 1.f); // Tourner vers la gauche (90 degrés autour de l'axe Z)

    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 1.0f); // Bleu
    glVertex2f(0.0f, 0.5f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();

    // Restaurer l'état précédent de la matrice de modèle-vue
    glPopMatrix();

    // Demander une nouvelle trame de rendu
    glutSwapBuffers();
}

// La fonction de rendu
void Display()
{
    Render();
}

// La fonction principale
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(960, 540);
    glutCreateWindow("Two Triangles");
    glutDisplayFunc(Display);
    glutMainLoop();
    return 0;
}
