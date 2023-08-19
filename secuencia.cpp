//Proyecto 1 Paralela


#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <random>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float PARTICLE_RADIUS = 20.0f;
const float FRICTION = 0.9f;

struct Particle {
    float velocityX;
    float velocityY;
    float posX;
    float posY;
    float colorR;
    float colorG;
    float colorB;

    Particle(float vx, float vy, float x, float y, float r, float g, float b)
        : velocityX(vx), velocityY(vy), posX(x), posY(y), colorR(r), colorG(g), colorB(b) {}
};

std::vector<Particle> particles;

void DrawParticles() {
    glClear(GL_COLOR_BUFFER_BIT);

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


void UpdateParticles(int value) {
    for (size_t i = 0; i < particles.size(); i++) {
        particles[i].posX += particles[i].velocityX;
        particles[i].posY += particles[i].velocityY;

        // Bounce off the window edges
        if (particles[i].posX < -WINDOW_WIDTH / 2 + PARTICLE_RADIUS || particles[i].posX > WINDOW_WIDTH / 2 - PARTICLE_RADIUS) {
            particles[i].velocityX = -particles[i].velocityX;
        }

        if (particles[i].posY < -WINDOW_HEIGHT / 2 + PARTICLE_RADIUS || particles[i].posY > WINDOW_HEIGHT / 2 - PARTICLE_RADIUS) {
            particles[i].velocityY = -particles[i].velocityY;
        }

        // Check for collisions with other particles
        for (size_t j = i + 1; j < particles.size(); j++) {
            float dx = particles[j].posX - particles[i].posX;
            float dy = particles[j].posY - particles[i].posY;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < PARTICLE_RADIUS * 2) {
                // Collide with the other particle and update velocities
                particles[i].velocityX = -particles[i].velocityX;
                particles[i].velocityY = -particles[i].velocityY;
                particles[j].velocityX = -particles[j].velocityX;
                particles[j].velocityY = -particles[j].velocityY;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, UpdateParticles, 0); // 16 ms ~ 60 fps
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " numParticles\n";
        return 1;
    }

    int numParticles = std::atoi(argv[1]);

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> randomFloatX(-WINDOW_WIDTH / 2 + PARTICLE_RADIUS, WINDOW_WIDTH / 2 - PARTICLE_RADIUS);
    std::uniform_real_distribution<float> randomFloatY(-WINDOW_HEIGHT / 2 + PARTICLE_RADIUS, WINDOW_HEIGHT / 2 - PARTICLE_RADIUS);
    std::uniform_real_distribution<float> randomVelocity(-10.0f, 10.0f);

    for (int i = 0; i < numParticles; i++) {
        float vx = randomVelocity(generator);
        float vy = randomVelocity(generator);
        float x = randomFloatX(generator);
        float y = randomFloatY(generator);
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
