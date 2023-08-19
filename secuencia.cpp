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
    float velocityX;
    float velocityY;
    float posX;
    float posY;

    Particle(bool alpha, float vx, float vy, float x, float y)
        : isAlpha(alpha), velocityX(vx), velocityY(vy), posX(x), posY(y) {}
};

std::vector<Particle> particles;

void DrawParticles() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (const Particle& particle : particles) {
        if (particle.isAlpha)
            glColor3f(0.0f, 0.0f, 1.0f); // Blue for alpha particles
        else
            glColor3f(1.0f, 0.0f, 0.0f); // Red for beta particles

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
    for (Particle& particle : particles) {
        if (!particle.isAlpha) {
            float minDistance = MAX_DISTANCE;
            int closestAlphaIndex = -1;

            for (size_t j = 0; j < particles.size(); j++) {
                if (particles[j].isAlpha && j != i) {
                    float distance = std::sqrt(
                        std::pow(particles[j].posX - particle.posX, 2) +
                        std::pow(particles[j].posY - particle.posY, 2)
                    );

                    if (distance < minDistance) {
                        minDistance = distance;
                        closestAlphaIndex = j;
                    }
                }
            }

            if (closestAlphaIndex != -1) {
                const float dx = particle.posX - particles[closestAlphaIndex].posX;
                const float dy = particle.posY - particles[closestAlphaIndex].posY;
                const float distance = std::max(minDistance, 0.5f);
                particle.velocityX -= (dx * 100) / distance;
                particle.velocityY -= (dy * 100) / distance;
                particle.velocityX *= FRICTION;
                particle.velocityY *= FRICTION;
            }
        }

        particle.posX += particle.velocityX;
        particle.posY += particle.velocityY;

        if (particle.posX < -WINDOW_WIDTH / 2 + PARTICLE_RADIUS || particle.posX > WINDOW_WIDTH / 2 - PARTICLE_RADIUS) {
            particle.velocityX = -particle.velocityX;
        }

        if (particle.posY < -WINDOW_HEIGHT / 2 + PARTICLE_RADIUS || particle.posY > WINDOW_HEIGHT / 2 - PARTICLE_RADIUS) {
            particle.velocityY = -particle.velocityY;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(0, UpdateParticles, 0);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " numberOfAlphaParticles numberOfBetaParticles\n";
        return 1;
    }

    int numAlphaParticles = std::atoi(argv[1]);
    int numBetaParticles = std::atoi(argv[2]);

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> randomFloat(-1.0f, 1.0f);

    for (int i = 0; i < numAlphaParticles; i++) {
        float vx = randomFloat(generator) * 10.0f;
        float vy = randomFloat(generator) * 10.0f;
        float x = randomFloat(generator) * 700.0f;
        float y = randomFloat(generator) * 400.0f;
        particles.emplace_back(true, vx, vy, x, y);
    }

    for (int i = 0; i < numBetaParticles; i++) {
        float x = randomFloat(generator) * 700.0f;
        float y = randomFloat(generator) * 400.0f;
        particles.emplace_back(false, 0.0f, 0.0f, x, y);
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
