#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

/**
 * Classe Bola - Representa a bola do jogo Pong
 * Gerencia posição, velocidade, movimento e colisões da bola
 */
class Bola {
private:
    Point2f position;   // Posição atual da bola (x, y)
    Point2f velocity;   // Velocidade da bola (dx, dy)
    int radius;         // Raio da bola

public:
    Bola(int windowWidth, int windowHeight, int ballRadius);  // Construtor
    void update();                                            // Atualiza posição
    void reset(int windowWidth, int windowHeight);           // Reinicia bola
    void draw(Mat& frame);                                    // Desenha na tela
    bool checkCollision(const Rect& paddle);                 // Verifica colisão
    void reverseY();                                          // Inverte velocidade Y
    void reverseX();                                          // Inverte velocidade X
    Point2f getPosition() const;                              // Obtém posição
    void setPosition(float x, float y);                       // Define posição
    void setVelocityX(float vx);                              // Define velocidade X
    float getVelocityX() const;                               // Obtém velocidade X
    int getRadius() const;                                    // Obtém raio
};