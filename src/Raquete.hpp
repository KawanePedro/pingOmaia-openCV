#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

/**
 * Classe Raquete - Representa uma raquete do jogo Pong
 * Gerencia posição, movimento e renderização da raquete
 */
class Raquete {
private:
    Rect bounds;        // Retângulo que define posição e tamanho
    int windowHeight;   // Altura da janela (para limites)
    int paddleHeight;   // Altura da raquete

public:
    Raquete(int x, int y, int width, int height, int winHeight);  // Construtor
    void update(int newY);                                        // Atualiza posição
    void draw(Mat& frame);                                        // Desenha na tela
    void keepInBounds();                                          // Mantém nos limites
    Rect getBounds() const;                                       // Obtém retângulo
    int getY() const;                                             // Obtém posição Y
    void setY(int y);                                             // Define posição Y
};