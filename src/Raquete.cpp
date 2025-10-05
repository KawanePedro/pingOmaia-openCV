#include "Raquete.hpp"

// Construtor: inicializa a raquete com posição e dimensões
Raquete::Raquete(int x, int y, int width, int height, int winHeight) 
    : bounds(x, y, width, height), windowHeight(winHeight), paddleHeight(height) {}

// Atualiza a posição Y da raquete centralizando na coordenada fornecida
void Raquete::update(int newY) {
    bounds.y = newY - paddleHeight / 2;
    keepInBounds();
}

// Desenha a raquete como um retângulo branco na tela
void Raquete::draw(Mat& frame) {
    rectangle(frame, bounds, Scalar(255, 255, 255), FILLED);
}

// Mantém a raquete dentro dos limites da tela
void Raquete::keepInBounds() {
    if (bounds.y < 0) bounds.y = 0;
    if (bounds.y > windowHeight - paddleHeight) bounds.y = windowHeight - paddleHeight;
}

// Retorna o retângulo que representa a raquete (para colisões)
Rect Raquete::getBounds() const {
    return bounds;
}

// Retorna a posição Y atual da raquete
int Raquete::getY() const {
    return bounds.y;
}

// Define nova posição Y da raquete
void Raquete::setY(int y) {
    bounds.y = y;
    keepInBounds();
}