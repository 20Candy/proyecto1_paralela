#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <random>
#include <chrono>
#include <random>
#include <tbb/tbb.h>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float PARTICLE_RADIUS = 60.0f;

tbb::tbb_rng rng;           // Random number generator

struct Particle {
    float velocityX;
    float velocityY;
    float posX;
    float posY;
    float colorR;
    float colorG;
    float colorB;
    float color_change;
    float radius;

    Particle(float vx, float vy, float x, float y, float r, float g, float b, float ch, float rad)
        : velocityX(vx), velocityY(vy), posX(x), posY(y), colorR(r), colorG(g), colorB(b), color_change(ch), radius(rad) {}
};

std::vector<Particle> particles;

std::chrono::high_resolution_clock::time_point previousFrameTime;
int frameCount = 0;
float fps = 0.0f;

int numParticlesToCreate = 0;
bool creationFinished = false;

void CreateParticle() {
    // Obtenemos el tiempo de inicio
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numParticlesToCreate; i++) {
        float radius = tbb::parallel_deterministic_random<float>(rng, 20.0f, PARTICLE_RADIUS);
        float vx = tbb::parallel_deterministic_random<float>(rng, -10.0f, 10.0f);
        float vy = tbb::parallel_deterministic_random<float>(rng, -10.0f, 10.0f);
        float x = tbb::parallel_deterministic_random<float>(rng, -WINDOW_WIDTH / 2 + PARTICLE_RADIUS, WINDOW_WIDTH / 2 - PARTICLE_RADIUS);
        float y = tbb::parallel_deterministic_random<float>(rng, -WINDOW_HEIGHT / 2 + PARTICLE_RADIUS, WINDOW_HEIGHT / 2 - PARTICLE_RADIUS);
        float r = tbb::parallel_deterministic_random<float>(rng, 0.0f, 1.0f);
        float g = tbb::parallel_deterministic_random<float>(rng, 0.0f, 1.0f);
        float b = tbb::parallel_deterministic_random<float>(rng, 0.0f, 1.0f);

        particles[i] = Particle(vx, vy, x, y, r, g, b, 0.0f, radius);
    } 
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Tiempo de ejecución: " << duration.count() << " microsegundos." << std::endl;
}

void DrawParticles() {
    glClear(GL_COLOR_BUFFER_BIT);

    frameCount++;
    std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - previousFrameTime).count() / 1000.0f;
    if (deltaTime >= 1.0f) {
        fps = static_cast<float>(frameCount) / deltaTime;
        frameCount = 0;
        previousFrameTime = currentFrameTime;
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-WINDOW_WIDTH / 2 + 10, -WINDOW_HEIGHT / 2 + 10);
    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
    for (char c : fpsText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    for (size_t i = 0; i < numParticlesToCreate; i++) {
        // Dibujar el cuerpo (un círculo grande)
        glColor3f(particles[i].colorR, particles[i].colorG, particles[i].colorB);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(particles[i].posX, particles[i].posY);
        const int numSegments = 64;
        for (int j = 0; j <= numSegments; j++) {
            float angle = j * 2.0f * M_PI / numSegments;
            float dx = particles[i].radius * std::cos(angle);
            float dy = particles[i].radius * std::sin(angle);
            glVertex2f(particles[i].posX + dx, particles[i].posY + dy);
        }
        glEnd();

        // Dibujar un circulo negro de la mitad del radio
        glColor3f(0.0f,0.0f,0.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(particles[i].posX, particles[i].posY);
        const int numSegments2 = 32;
        for (int j = 0; j <= numSegments2; j++) {
            float angle = j * 2.0f * M_PI / numSegments2;
            float dx = particles[i].radius/2 * std::cos(angle);
            float dy = particles[i].radius/2 * std::sin(angle);
            glVertex2f(particles[i].posX + dx, particles[i].posY + dy);
        }
        glEnd();

        // Dibujar el un circulo de color adentro del negro
        glColor3f((particles[i].colorR)/2, (particles[i].colorG)/2, (particles[i].colorB)/2);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(particles[i].posX, particles[i].posY);
        const int numSegments3 = 64;
        for (int j = 0; j <= numSegments3; j++) {
            float angle = j * 2.0f * M_PI / numSegments3;
            float dx = particles[i].radius/4 * std::cos(angle);
            float dy = particles[i].radius/4 * std::sin(angle);
            glVertex2f(particles[i].posX + dx, particles[i].posY + dy);
        }
        glEnd();



    }

    glutSwapBuffers();
}

void UpdateParticles(int value) {

    std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - previousFrameTime).count() / 1000.0f;

    for (size_t i = 0; i < numParticlesToCreate; i++) {
        particles[i].posX += particles[i].velocityX;
        particles[i].posY += particles[i].velocityY;

        particles[i].color_change += deltaTime;
        if (particles[i].color_change >= tbb::parallel_deterministic_random<int>(rng, 4, 10);) { // Change color every random seconds
            particles[i].colorR = tbb::parallel_deterministic_random<float>(rng, 0.0f, 1.0f);
            particles[i].colorG = tbb::parallel_deterministic_random<float>(rng, 0.0f, 1.0f);
            particles[i].colorB = tbb::parallel_deterministic_random<float>(rng, 0.0f, 1.0f);
            particles[i].color_change = 0.0f;
        }

        if (particles[i].posX < -WINDOW_WIDTH / 2 + particles[i].radius || particles[i].posX > WINDOW_WIDTH / 2 - particles[i].radius) {
            particles[i].velocityX = -particles[i].velocityX;
        }
        if (particles[i].posY < -WINDOW_HEIGHT / 2 + particles[i].radius || particles[i].posY > WINDOW_HEIGHT / 2 - particles[i].radius) {
            particles[i].velocityY = -particles[i].velocityY;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, UpdateParticles, 0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " numParticles\n";
        return 1;
    }
    previousFrameTime = std::chrono::high_resolution_clock::now();
    numParticlesToCreate = std::atoi(argv[1]);          // Obtiene el número de partículas a crear

    particles.reserve(numParticlesToCreate);                                // Reserva el espacio para las partículas

    glutInit(&argc, argv);                              // Inicializa GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);        // Habilita el doble buffer y el modelo de color RGB
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);    // Establece el tamaño de la ventana
    glutCreateWindow("Proyecto Paralela");              // Crea la ventana con el título dado

    glMatrixMode(GL_PROJECTION);                        // Establece la matriz de proyección
    glLoadIdentity();                                   // Carga la matriz identidad

    // Establece la matriz de proyección ortogonal
    glOrtho(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);                         // Establece la matriz de vista del modelo
    glLoadIdentity();                                   // Carga la matriz identidad

    CreateParticle();                                   // Inicia la simulación de partículas

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);               // Establece el color de fondo
    glutDisplayFunc(DrawParticles);                     // Establece la función de dibujo
    glutTimerFunc(0, UpdateParticles, 0);               // Establece la función de actualización
    glutMainLoop();                                     // Inicia el ciclo de dibujo
    return 0;
}
