#include "Bola.hpp"
#include <cstdlib>
#include <cmath>

// Construtor: inicializa a bola com raio e posição central
Bola::Bola(int windowWidth, int windowHeight, int ballRadius) : radius(ballRadius) {
    reset(windowWidth, windowHeight);
}

// Atualiza a posição da bola baseado na velocidade
void Bola::update() {
    position.x += velocity.x;
    position.y += velocity.y;
}

// Reinicia a bola no centro com velocidade aleatória
void Bola::reset(int windowWidth, int windowHeight) {
    position = Point2f(windowWidth / 2, windowHeight / 2);
    velocity.x = (rand() % 2 == 0) ? 12.0f : -12.0f;
    velocity.y = (rand() % 2 == 0) ? 12.0f : -12.0f;
}

// Desenha a bola como um círculo branco na tela
void Bola::draw(Mat& frame) {
    circle(frame, position, radius, Scalar(255, 255, 255), FILLED);
}

// Verifica se a bola colidiu com uma raquete
bool Bola::checkCollision(const Rect& paddle) {
    Rect ballRect(position.x - radius, position.y - radius, radius * 2, radius * 2);
    return (ballRect & paddle).area() > 0;
}

// Inverte a velocidade vertical (colisão com parede superior/inferior)
void Bola::reverseY() {
    velocity.y *= -1;
}

// Inverte a velocidade horizontal com aceleração (colisão com raquete)
void Bola::reverseX() {
    velocity.x *= -1.02f;
}

// Retorna a posição atual da bola
Point2f Bola::getPosition() const {
    return position;
}

// Define nova posição da bola
void Bola::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
}

// Define a velocidade horizontal da bola
void Bola::setVelocityX(float vx) {
    velocity.x = vx;
}

// Retorna a velocidade horizontal atual
float Bola::getVelocityX() const {
    return velocity.x;
}

// Retorna o raio da bola
int Bola::getRadius() const {
    return radius;
}