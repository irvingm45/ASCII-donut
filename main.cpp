#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <math.h>

using namespace std;
#define For(a, b, c) for(int a = b; a < c; a++)

//Creamos las constantes necesarias
const float theta_spacing = 0.07;
const float phi_spacing = 0.02;

const float r1 = 1.2;     // Radio del circulo
const float r2 = 2;     // Distancia del centro del circulo respecto al eje x
const float k2 = 40;     // Distancia de la dona de la pantalla
float k1 = 0;    // Distancia de la pantalla de la cámara PENDIENTE

void getTerminalSize(int& ancho, int& altura) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        ancho = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        altura = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        ancho = 80;   // Valor por defecto
        altura = 25;  // Valor por defecto
    }
}

void render_frame(float A, float B, int ancho, int altura){
    
    // Precomputamos el seno y coseno de los ejes sobre los que va a girar
    float cosenoA = cos(A), senoA = sin(A),
            cosenoB = cos(B), senoB = sin(B);

    char output[ancho][altura]; // Buffer que almacenara los caracteres
    For(i, 0, ancho) { For(j, 0, altura) { output[i][j] = ' '; }}
    
    float zbuffer[ancho][altura]; // Buffer de profundidad
    For(i, 0, ancho) { For(j, 0, altura) { zbuffer[i][j] = 0; }}

    // Empezamos a hacer ahora la figura
    // Primero hacemos que theta haga el torus con el círculo
    for(float theta = 0; theta < 2*M_PI; theta += theta_spacing) {
        //Precomputamos el seno y coseno the theta
        float cosenoTheta = cos(theta), senoTheta = sin(theta);

        // phi gira alrededor del centro de revolución de un torus
        for(float phi = 0; phi < 2*M_PI; phi += phi_spacing) {
            
            //Precomputamos el seno y coseno de phi
            float cosenoPhi = cos(phi), senoPhi = sin(phi);

            // Coordenadas del circulo antes de hacerlo torus
            float circulox = r2 + r1*cosenoTheta,
                    circuloY = r1*senoTheta;
    
            // Ahora si lo hacemos 3D
            float x = circulox*(cosenoB*cosenoPhi + senoA*senoB*senoPhi) - circuloY*cosenoA*senoB,
                y = circulox*(senoB*cosenoPhi - senoA*cosenoB*senoPhi) + circuloY*cosenoA*cosenoB,
                z = k2 + cosenoA*circulox*senoPhi + circuloY*senoA,
                ooz = 1/z;

            //Ahora hacemos la proyección en la pantalla
            int xp = (int) (ancho/2 + k1*ooz*x),
                yp = (int) (altura/2 - k1*ooz*y);
            
            // Calculamos la ilumincación
            float L = cosenoPhi*cosenoTheta*senoB - cosenoA*cosenoTheta*senoPhi - senoA*senoTheta + cosenoB*(cosenoA*senoTheta - cosenoTheta*senoA*senoPhi);

            if(L > 0) {
                if(ooz > zbuffer[xp][yp]) {
                    zbuffer[xp][yp] = ooz;
                    int indice_iluminacion = L*8;
                    output[xp][yp] = ".,-~:;=!*#$@"[indice_iluminacion];
                }
            }
        }    
    }

    //Ahora se vuelva el output a la pantalla
    printf("\x1b[H");
    for(int j = 0; j < altura; j++) {
        for(int i = 0; i < ancho; i++) {
            putchar(output[i][j]);
            // putchar('.');
        }
        putchar('\n');
    }
}

int main(){
    float A = 0, B = 0;
    // Obtenemos a k1
    int ancho, altura; getTerminalSize(ancho, altura);
    k1 = 150;

    while(true){
        A += 0.2; B += 0.3;
        render_frame(A, B, ancho, altura);
    }
}
