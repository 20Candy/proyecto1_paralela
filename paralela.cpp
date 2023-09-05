#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <random>
#include <chrono>
#include <random>
#include <omp.h>
#include <string>
#include <cstdlib> // Librerías que importar

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080; //Tamaño de la pantalla.
const float PARTICLE_RADIUS = 60.0f; //Tamaño de las partículas.

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
}; //Estructura para simular una particula. 

std::vector<Particle> particles; //Vector para partículas.


std::chrono::high_resolution_clock::time_point previousFrameTime;
int frameCount = 0;
float fps = 0.0f; // Implementación de FPS.

int numParticlesToCreate = 0;
bool creationFinished = false; //Cantidad de partículas y creación respectiva


//CreateParticle
//0 Parametros
//Crear Partículas
//Retorno nulo
void CreateParticle() {
    // Tomar el tiempo de inicio
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    #pragma omp parallel
    {  
        //Creación paralela de las particulas
        std::random_device rd;
        std::mt19937 generator(rd());

       //Distribución aleatoria para crear las particulas basada en radio, color, posición y movimiento

        std::uniform_real_distribution<float> randomRadius(20.0f, PARTICLE_RADIUS);
        std::uniform_real_distribution<float> randomFloatX(-WINDOW_WIDTH / 2 + PARTICLE_RADIUS, WINDOW_WIDTH / 2 - PARTICLE_RADIUS);
        std::uniform_real_distribution<float> randomFloatY(-WINDOW_HEIGHT / 2 + PARTICLE_RADIUS, WINDOW_HEIGHT / 2 - PARTICLE_RADIUS);
        std::uniform_real_distribution<float> randomVelocity(-10.0f, 10.0f);
        std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);

        #pragma omp for
        for (int i = 0; i < numParticlesToCreate; i++) {
            float radius = randomRadius(generator);
            float vx = randomVelocity(generator);
            float vy = randomVelocity(generator);
            float x = randomFloatX(generator);
            float y = randomFloatY(generator);
            float r = randomColor(generator);
            float g = randomColor(generator);
            float b = randomColor(generator);

            particles[i] = Particle(vx, vy, x, y, r, g, b, 0.0f, radius);
        }//Creación respectivo en un Parallel For.

        #pragma omp master
        {   
            //Master que cuenta el tiempo de ejecución.
            std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> totalTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            std::cout << "Tiempo de creación de partículas: " << totalTime.count() << " segundos\n";
        }
    } 

}
//DrawParticulas
//0 Parametros
//Dibuja la particulas en la pantalla 
//Retorno nulo
void DrawParticles() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (size_t i = 0; i < numParticlesToCreate; i++) {
        // Dibujar el cuerpo (un círculo grande). Se empiezan a crear.
        glColor3f(particles[i].colorR, particles[i].colorG, particles[i].colorB);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(particles[i].posX, particles[i].posY);
        const int numSegments = 64;
        //Angulo de movimiento basado en su posición y movimiento        
        for (int j = 0; j <= numSegments; j++) {
            float angle = j * 2.0f * M_PI / numSegments;
            float dx = particles[i].radius * std::cos(angle);
            float dy = particles[i].radius * std::sin(angle);

            glVertex2f(particles[i].posX + dx, particles[i].posY + dy);
        }
        glEnd();
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-WINDOW_WIDTH / 2 + 10, -WINDOW_HEIGHT / 2 + 10);
    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
    for (char c : fpsText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
   //Se tiene el bitmap, y empieza a colorear en la pantalla.

    frameCount++;
    std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - previousFrameTime).count() / 1000.0f;
    if (deltaTime >= 1.0f) {
        fps = static_cast<float>(frameCount) / deltaTime;
        frameCount = 0;
        previousFrameTime = currentFrameTime;
    }
    //Se tiene el framerate, haciendo la cantidad de frames divido por el tiempo pasado. 

    glutSwapBuffers();
    //Cambio de Buffers
}
//UpdateParticle
//0 Parametros
//Actualiza la particulas a dibujar.
//Retorno nulo
void UpdateParticles(int value) {

    std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameTime - previousFrameTime).count() / 1000.0f;
    //Cambio para los tiempos y el framerate. 

    #pragma omp parallel
    {
        std::random_device rd;
        std::mt19937 generator(rd());

        std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);
        std::uniform_int_distribution<int> randomInt(5, 10);
        //Distribución aleatorio para los datos.

        #pragma omp for
        //Paralelización de los datos para los circulos. Hace el cambio de posición basado en su belocidad.
        for (int i = 0; i < numParticlesToCreate; i++) {
            particles[i].posX += particles[i].velocityX;
            particles[i].posY += particles[i].velocityY;
  
            particles[i].color_change += deltaTime;
            if (particles[i].color_change >= randomInt(generator)) { // Change color every random seconds
                particles[i].colorR = randomColor(generator);
                particles[i].colorG = randomColor(generator);
                particles[i].colorB = randomColor(generator);
                particles[i].color_change = 0.0f;
            }

            if (particles[i].posX < -WINDOW_WIDTH / 2 + particles[i].radius || particles[i].posX > WINDOW_WIDTH / 2 - particles[i].radius) {
                particles[i].velocityX = -particles[i].velocityX;
            }
            if (particles[i].posY < -WINDOW_HEIGHT / 2 + particles[i].radius || particles[i].posY > WINDOW_HEIGHT / 2 - particles[i].radius) {
                particles[i].velocityY = -particles[i].velocityY;
            } //La nueva posición en el caso que si se topa en la pantalla.

            
            // Cambiar el color de la partícula según el promedio de los colores de las partículas vecinas
            float avgColorR = 0.0f, avgColorG = 0.0f, avgColorB = 0.0f;
            int neighborCount = 0;

            #pragma omp parallel
            {
                float privateAvgColorR = 0.0f, privateAvgColorG = 0.0f, privateAvgColorB = 0.0f;
                int privateNeighborCount = 0;
                //Otra paralelización para cambiar los colores basado en posición y sus vecionas.
                
                #pragma omp for
                for (size_t j = 0; j < numParticlesToCreate; j++) {
                    if (i != j) {
                        float distance = std::sqrt((particles[i].posX - particles[j].posX) * (particles[i].posX - particles[j].posX) + (particles[i].posY - particles[j].posY) * (particles[i].posY - particles[j].posY));

                        if (distance < 30) {
                            privateAvgColorR += particles[j].colorR;
                            privateAvgColorG += particles[j].colorG;
                            privateAvgColorB += particles[j].colorB;
                            privateNeighborCount++;
                        }
                    }
                }

                #pragma omp critical
                {
                    avgColorR += privateAvgColorR;
                    avgColorG += privateAvgColorG;
                    avgColorB += privateAvgColorB;
                    neighborCount += privateNeighborCount;
                    //Sección crítica para que solo lo haga un thread, los colores vecinos.
                }
            }


            if (neighborCount > 0) {
                avgColorR /= neighborCount;
                avgColorG /= neighborCount;
                avgColorB /= neighborCount;

                particles[i].colorR = avgColorR;
                particles[i].colorG = avgColorG;
                particles[i].colorB = avgColorB;
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, UpdateParticles, 0);
    //Hace un nuevo display y cambia la función de tiempo
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " numParticles\n";  //Se asegura que el usuario ingrese un input.
        return 1;
    }

    bool isNumber = false;
    for (char c : input) {
        if (!std::isdigit(c)) {
            isNumber = true;
            break;
        }
    }

    if (isNumber) {
        // Seguridad. Asegura que sea un entero positivo, menor de 15000 para velocidad
        std::cout << "Debe de ingresar un número, no String.\n";
        return 1; //Programación Defensiva 
    } else {
         int number = std::stoi(input);
         if (numner > 15000){
             std::cout << "Debe de ingresar un número mayor que 15000.\n";
             return 1;
         }
        if (number < 0){
            std::cout << "Debe de ingresar un número positvo.\n";
            return 1;
        }
            
    }
    

    omp_set_num_threads(4);                             // Establece el número de hilos a utilizar

    previousFrameTime = std::chrono::high_resolution_clock::now();
    numParticlesToCreate = std::atoi(argv[1]);          // Obtiene el número de partículas a crear

    particles.reserve(numParticlesToCreate);            // Reserva el espacio para las partículas

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

    CreateParticle();                                   // Crea las partículas

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);               // Establece el color de fondo
    glutDisplayFunc(DrawParticles);                     // Establece la función de dibujo
    glutTimerFunc(0, UpdateParticles, 0);               // Establece la función de actualización
    glutMainLoop();                                     // Inicia el ciclo de dibujo
    return 0;
}
