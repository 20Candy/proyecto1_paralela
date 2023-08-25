#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>

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

    Particle(float vx, float vy, float x, float y, float r, float g, float b)
        : velocityX(vx), velocityY(vy), posX(x), posY(y), colorR(r), colorG(g), colorB(b) {}
};

std::vector<Particle> particles;

std::chrono::high_resolution_clock::time_point previousFrameTime;
int frameCount = 0;
float fps = 0.0f;

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
    #pragma omp parallel for
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
    glutTimerFunc(16, UpdateParticles, 0);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " numParticles\n";
        return 1;
    }
    previousFrameTime = std::chrono::high_resolution_clock::now();
    int numParticles = std::atoi(argv[1]);

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> randomFloatX(-WINDOW_WIDTH / 2 + PARTICLE_RADIUS, WINDOW_WIDTH / 2 - PARTICLE_RADIUS);
    std::uniform_real_distribution<float> randomFloatY(-WINDOW_HEIGHT / 2 + PARTICLE_RADIUS, WINDOW_HEIGHT / 2 - PARTICLE_RADIUS);
    std::uniform_real_distribution<float> randomVelocity(-10.0f, 10.0f);
    std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);

    #pragma omp parallel
    {
        std::vector<Particle> threadParticles;

        #pragma omp for
        for (int i = 0; i < numParticles; i++) {
            float vx = randomVelocity(generator);
            float vy = randomVelocity(generator);
            float x = randomFloatX(generator);
            float y = randomFloatY(generator);
            float r = randomColor(generator);
            float g = randomColor(generator);
            float b = randomColor(generator);
            threadParticles.emplace_back(vx, vy, x, y, r, g, b);
        }

        #pragma omp critical
        {
            particles.insert(particles.end(), threadParticles.begin(), threadParticles.end());
        }
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
