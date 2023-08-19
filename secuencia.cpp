#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <random>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float PARTICLE_RADIUS = 10.0f;
const float FRICTION = 0.9f;

struct Particle {
    float velocityX;
    float velocityY;
    float posX;
    float posY;

    Particle(float vx, float vy, float x, float y)
        : velocityX(vx), velocityY(vy), posX(x), posY(y) {}
};

std::vector<Particle> particles;

void DrawParticles() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 1.0f, 1.0f); // White color for all particles

    for (const Particle& particle : particles) {
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

void UpdateParticles(int value) {
    for (size_t i = 0; i < particles.size(); i++) {
        particles[i].posX += particles[i].velocityX;
        particles[i].posY += particles[i].velocityY;

        if (particles[i].posX < -WINDOW_WIDTH / 2 + PARTICLE_RADIUS || particles[i].posX > WINDOW_WIDTH / 2 - PARTICLE_RADIUS) {
            particles[i].velocityX = -particles[i].velocityX;
        }

        if (particles[i].posY < -WINDOW_HEIGHT / 2 + PARTICLE_RADIUS || particles[i].posY > WINDOW_HEIGHT / 2 - PARTICLE_RADIUS) {
            particles[i].velocityY = -particles[i].velocityY;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(0, UpdateParticles, 0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " numParticles\n";
        return 1;
    }

    int numParticles = std::atoi(argv[1]);

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> randomFloat(-1.0f, 1.0f);

    for (int i = 0; i < numParticles; i++) {
        float vx = randomFloat(generator) * 10.0f;
        float vy = randomFloat(generator) * 10.0f;
        float x = randomFloat(generator) * 700.0f;
        float y = randomFloat(generator) * 400.0f;
        particles.emplace_back(vx, vy, x, y);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Particle Animation");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glutDisplayFunc(DrawParticles);
    glutTimerFunc(0, UpdateParticles, 0);

    glutMainLoop();

    return 0;
}
