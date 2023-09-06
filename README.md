# Proyecto 1 ScreenSaver - Computación Paralela - UVG
Implementación de un ScreenSaver con OpenMP. 

## Instalacion 🔧

1. Clona este repositorio:  ```git remote add origin https://github.com/20Candy/proyecto1_paralela ```
2. Accede al directorio del proyecto: ```cd nombre_del_directorio```


## Correr el programa 🚀

1. Programa Secuencial
```shell
  g++ -o sec secuencia.cpp -lGL -lGLU -lglut -lm -fopenmp
  ./ sec [numero_particulas]
```

2. Programa paralelo
```shell
  g++ -o par paralela.cpp -lGL -lGLU -lglut -lm -fopenmp
  ./ par [numero_particulas]
```

## Construido con 🛠️
- C++

## Características 📋

Este ScreenSaver posee las siguientes implementaciones:

- [X] Programa secuencial funcionando correctamente y desplegando el screensaver
    - [X] Programa compila sin errores. Se ejecuta sin fallar
    - [X] Programación Defensiva en cado de errores de ingreso de datos
    - [x] Incluir Readme.md
    - [X] Se evita el uso de hard-coded variables. 
- [X] Programa paralelo funcionando correctamente y desplegando el screensaver
    - [X] Programa compila sin errores. Se ejecuta sin fallar
    - [X] Programación Defensiva en cado de errores de ingreso de datos
    - [x] Incluir Readme.md
    - [X] Se evita el uso de hard-coded variables. 

## Documentación 📋

[Documento](https://docs.google.com/document/d/1U96Er7mJJI_DsJxIyI-juTQotDvnr1ACivtlx2O1mfk/edit?usp=sharing)

Incluye
- [X] Formato según guía de informes UVG
- [X] Anexo 1 con Diagrama de Flujo del Programa
- [X] Anaexo 2 con Catálogo de Funciones
- [X] Anexo 3 con Bitácora de Pruebas




## Autores ✒️

* **Stefano Aragoni** - *desarrollo* - [#20Candy](https://github.com/20Candy)
* **Roberto Vallecillos** - *desarrollo* - [#20Candy](https://github.com/Roberto-VC)
* **Carol Arevalo** - *desarrollo* - [#20Candy](https://github.com/stefanoaragoni)