#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <random>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float PARTICLE_RADIUS = 10.0f;
const float FRICTION = 0.9f;
const float MAX_DISTANCE = 500.0f;

struct Particle {
    bool isAlpha;
    int colorID;
    float velocityX;
    float velocityY;
    float posX;
    float posY;

    Particle(bool alpha, int color, float vx, float vy, float x, float y)
        : isAlpha(alpha), colorID(color), velocityX(vx), velocityY(vy), posX(x), posY(y) {}
};

std::vector<Particle> particles;

void DrawParticles() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (const Particle& particle : particles) {
        if (particle.isAlpha)
            glColor3f(0.0f, 0.0f, 1.0f);
        else if (particle.colorID == 1)
            glColor3f(1.0f, 0.0f, 0.0f);
        else if (particle.colorID == 2)
            glColor3f(0.0f, 1.0f, 0.0f);
        else if (particle.colorID == 3)
            glColor3f(1.0f, 1.0f, 0.0f);
        else if (particle.colorID == 4)
            glColor3f(1.0f, 0.5f, 0.0f);

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
        if (!particles[i].isAlpha) {
            float minDistance = MAX_DISTANCE;
            int closestAlphaIndex = -1;

            for (size_t j = 0; j < particles.size(); j++) {
                if (particles[j].isAlpha && j != i) {
                    float distance = std::sqrt(
                        std::pow(particles[j].posX - particles[i].posX, 2) +
                        std::pow(particles[j].posY - particles[i].posY, 2)
                    );

                    if (distance < minDistance) {
                        minDistance = distance;
                        closestAlphaIndex = j;
                    }
                }
            }

            if (closestAlphaIndex != -1) {
                const float dx = particles[i].posX - particles[closestAlphaIndex].posX;
                const float dy = particles[i].posY - particles[closestAlphaIndex].posY;
                const float distance = std::max(minDistance, 0.5f);
                particles[i].velocityX -= (dx * 100) / distance;
                particles[i].velocityY -= (dy * 100) / distance;
                particles[i].velocityX *= FRICTION;
                particles[i].velocityY *= FRICTION;
            }
        }

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
        std::cout << "Usage: " << argv[0] << " numberOfParticles\n";
        return 1;
    }

    int numParticles = std::atoi(argv[1]);
    int numAlphaParticles = numParticles / 5;
    int numBetaParticles = numParticles - numAlphaParticles;

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> randomFloat(-1.0f, 1.0f);

    for (int i = 0; i < numAlphaParticles; i++) {
        float vx = randomFloat(generator) * 2.0f;  // Adjust the velocity range
        float vy = randomFloat(generator) * 2.0f;
        float x = randomFloat(generator) * 700.0f;
        float y = randomFloat(generator) * 400.0f;
        particles.emplace_back(true, 0, vx, vy, x, y);
    }

    for (int i = 0; i < numBetaParticles; i++) {
        float x = randomFloat(generator) * 700.0f;
        float y = randomFloat(generator) * 400.0f;
        particles.emplace_back(false, i % 4 + 1, 0.0f, 0.0f, x, y);
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
