#include "Jogador.hpp"

// Construtor: cria um jogador com nome, pontuação zero e raquete
Jogador::Jogador(const string& playerName, int x, int y, int width, int height, int winHeight)
    : name(playerName), score(0), raquete(x, y, width, height, winHeight) {}

// Adiciona um ponto à pontuação do jogador
void Jogador::addPoint() {
    score++;
}

// Retorna a pontuação atual do jogador
int Jogador::getScore() const {
    return score;
}

// Atualiza a posição da raquete do jogador
void Jogador::updateRaquete(int y) {
    raquete.update(y);
}

// Desenha a raquete do jogador na tela
void Jogador::drawRaquete(Mat& frame) {
    raquete.draw(frame);
}

// Retorna os limites da raquete para detecção de colisão
Rect Jogador::getRaqueteBounds() const {
    return raquete.getBounds();
}

// Retorna o nome do jogador
string Jogador::getName() const {
    return name;
}

// Lógica da IA: move a raquete em direção à bola
void Jogador::aiUpdate(int ballY, int paddleHeight) {
    int paddleCenterY = raquete.getY() + paddleHeight / 2;
    
    if (ballY < paddleCenterY - 10) {
        raquete.setY(raquete.getY() - 6);
    } else if (ballY > paddleCenterY + 10) {
        raquete.setY(raquete.getY() + 6);
    }
}