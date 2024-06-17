#include <GL/glut.h>
#include <cmath>

// Variable globale pour stocker le temps écoulé au dernier rendu
int previousTime = 0;

void Render()
{
    // Effacer l'écran avec la couleur jaune
    glClearColor(1.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    // Configurer la matrice de transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 

    // Obtenir le temps écoulé depuis le début du programme en millisecondes
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    // Calculer le temps écoulé depuis le dernier rendu en secondes
    float elapsedTime = (currentTime - previousTime) / 1000.0f;

    // Utiliser le temps écoulé comme facteur d'angle de rotation
    float angle = elapsedTime * 100.0f; // Rotation de 45 degrés par seconde

    // Appliquer la rotation
    glRotatef(angle, 0.f, 0.f, 1.f);

    // Mettre à jour le temps précédent
    previousTime = currentTime;

    // Dessiner un triangle
    glBegin(GL_TRIANGLES);

    // Sommet rouge
    glColor3f(1.0f, 0.0f, 0.0f); // Rouge
    glVertex2f(0.0f, 0.5f);

    // Sommet bleu
    glColor3f(0.0f, 0.0f, 1.0f); // Bleu
    glVertex2f(-0.5f, -0.5f);

    // Sommet vert
    glColor3f(0.0f, 1.0f, 0.0f); // Vert
    glVertex2f(0.5f, -0.5f);

    glEnd();
}

// La fonction de rendu
void Display()
{
    Render();
    glutSwapBuffers();
    glutPostRedisplay(); // Demander une nouvelle trame de rendu
}

// La fonction principale
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(960, 540);
    glutCreateWindow("Triangle");
    glutDisplayFunc(Display);
    glutMainLoop();
    return 0;
}
