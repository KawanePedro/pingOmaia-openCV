#pragma once
#include "Raquete.hpp"
#include <string>

using namespace std;

/**
 * Classe Jogador - Representa um jogador do jogo Pong
 * Encapsula pontuação, nome e raquete do jogador
 */
class Jogador {
private:
    int score;          // Pontuação do jogador
    string name;        // Nome do jogador
    Raquete raquete;    // Raquete do jogador

public:
    Jogador(const string& playerName, int x, int y, int width, int height, int winHeight);  // Construtor
    void addPoint();                                    // Adiciona ponto
    int getScore() const;                               // Obtém pontuação
    void updateRaquete(int y);                          // Atualiza raquete
    void drawRaquete(Mat& frame);                       // Desenha raquete
    Rect getRaqueteBounds() const;                      // Obtém limites da raquete
    string getName() const;                             // Obtém nome
    void aiUpdate(int ballY, int paddleHeight);         // Lógica da IA
};