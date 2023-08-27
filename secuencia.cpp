#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <random>
#include <chrono>
#include <random>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float PARTICLE_RADIUS = 20.0f;

struct Particle {
    float velocityX;
    float velocityY;
    float posX;
    float posY;
    float colorR;
    float colorG;
    float colorB;
    float color_change;

    Particle(float vx, float vy, float x, float y, float r, float g, float b)
        : velocityX(vx), velocityY(vy), posX(x), posY(y), colorR(r), colorG(g), colorB(b), color_change(0.0f) {}
};

std::vector<Particle> particles;

std::chrono::high_resolution_clock::time_point previousFrameTime;
int frameCount = 0;
float fps = 0.0f;
float deltaTime = 0.0f;

void DrawParticles() {
    glClear(GL_COLOR_BUFFER_BIT);

    frameCount++;
    std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - previousFrameTime).count() / 1000.0f;

    if (deltaTime >= 1.0f) {
        fps = static_cast<float>(frameCount) / deltaTime;
        frameCount = 0;
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-WINDOW_WIDTH / 2 + 10, -WINDOW_HEIGHT / 2 + 10);
    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
    for (char c : fpsText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    for (const Particle& particle : particles) {
        glColor3f(particle.colorR, particle.colorG, particle.colorB);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(particle.posX, particle.posY);
        const int numSegments = 16;
        for (int i = 0; i <= numSegments; i++) {
            float angle = i * 2.0f * M_PI / numSegments;
            float dx = PARTICLE_RADIUS * std::cos(angle);
            float dy = PARTICLE_RADIUS * std::sin(angle);
            glVertex2f(particle.posX + dx, particle.posY + dy);
        }
        glEnd();
    }

    glutSwapBuffers();
}

int numParticlesToCreate = 0;
int particlesCreated = 0;
bool creationFinished = false;

void CreateParticle() {
    if (particlesCreated < numParticlesToCreate) {
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<float> randomFloatX(-WINDOW_WIDTH / 2 + PARTICLE_RADIUS, WINDOW_WIDTH / 2 - PARTICLE_RADIUS);
        std::uniform_real_distribution<float> randomFloatY(-WINDOW_HEIGHT / 2 + PARTICLE_RADIUS, WINDOW_HEIGHT / 2 - PARTICLE_RADIUS);
        std::uniform_real_distribution<float> randomVelocity(-10.0f, 10.0f);
        std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);
        float vx = randomVelocity(generator);
        float vy = randomVelocity(generator);
        float x = randomFloatX(generator);
        float y = randomFloatY(generator);
        float r = randomColor(generator);
        float g = randomColor(generator);
        float b = randomColor(generator);
        particles.emplace_back(vx, vy, x, y, r, g, b);

        particles.back().color_change = 0.0f;

        particlesCreated++;
    } else {
        if (!creationFinished) {
            creationFinished = true;
            std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
            float totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - previousFrameTime).count() / 1000.0f;
            std::cout << "Tiempo total: " << totalTime << " segundos" << std::endl;
        }
    }
}

void UpdateParticles(int value) {
    CreateParticle();

    for (size_t i = 0; i < particles.size(); i++) {
        particles[i].posX += particles[i].velocityX;
        particles[i].posY += particles[i].velocityY;

        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);
        std::uniform_int_distribution<int> randomInt(2, 6);

        particles[i].color_change += deltaTime;
        if (particles[i].color_change >= randomInt(generator)) { // Change color every random seconds
            particles[i].colorR = randomColor(generator);
            particles[i].colorG = randomColor(generator);
            particles[i].colorB = randomColor(generator);
            particles[i].color_change = 0.0f;
        }

        if (particles[i].posX < -WINDOW_WIDTH / 2 + PARTICLE_RADIUS || particles[i].posX > WINDOW_WIDTH / 2 - PARTICLE_RADIUS) {
            particles[i].velocityX = -particles[i].velocityX;
        }
        if (particles[i].posY < -WINDOW_HEIGHT / 2 + PARTICLE_RADIUS || particles[i].posY > WINDOW_HEIGHT / 2 - PARTICLE_RADIUS) {
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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);               // Establece el color de fondo
    glutDisplayFunc(DrawParticles);                     // Establece la función de dibujo
    glutTimerFunc(0, UpdateParticles, 0);               // Establece la función de actualización
    glutMainLoop();                                     // Inicia el ciclo de dibujo
    return 0;
}
