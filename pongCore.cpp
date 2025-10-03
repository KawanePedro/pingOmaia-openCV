#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

// Usar os namespaces para facilitar a escrita do código
using namespace cv;
using namespace std;

// --- Constantes do Jogo ---
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;

const int BALL_RADIUS = 10;

// --- Estrutura para a Bola ---
struct Ball {
    Point2f center;
    Point2f velocity;
};

// --- Função para reiniciar a posição e velocidade da bola ---
void resetBall(Ball& ball) {
    ball.center = Point2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    // Inicia com uma velocidade aleatória para a esquerda ou direita
    ball.velocity.x = (rand() % 2 == 0) ? 5 : -5;
    ball.velocity.y = (rand() % 2 == 0) ? 5 : -5;
}

int main() {
    // --- Inicialização ---
    srand(time(0)); // Semente para números aleatórios

    // Cria a janela do jogo
    namedWindow("Pong - 2 Jogadores", WINDOW_AUTOSIZE);
    
    // Cria a imagem de fundo (nossa arena)
    Mat arena = Mat::zeros(WINDOW_HEIGHT, WINDOW_WIDTH, CV_8UC3);

    // Cria as raquetes (paddles) para os dois jogadores
    Rect player1Paddle(PADDLE_WIDTH, (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT);
    Rect player2Paddle(WINDOW_WIDTH - (PADDLE_WIDTH * 2), (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT);

    // Cria a bola
    Ball ball;
    resetBall(ball);

    // Variáveis de pontuação
    int player1Score = 0;
    int player2Score = 0;

    // --- Game Loop Principal ---
    while (true) {
        // Cria um clone da arena para desenhar nesta iteração
        Mat frame = arena.clone();

        // --- 1. Processar Entradas (Teclado) ---
        int key = waitKeyEx(16);

        if (key == 27) { // Tecla ESC para sair
            break;
        }

        // ---- Controles do Jogador 1 (W, S) ----
        if (key == 'w' || key == 'W') {
            player1Paddle.y -= 40; // VELOCIDADE ALTERADA de 25 para 40
        }
        if (key == 's' || key == 'S') {
            player1Paddle.y += 40; // VELOCIDADE ALTERADA de 25 para 40
        }

        // ---- Controles do Jogador 2 (Setas) ----
        if (key == 65362) { // CÓDIGO ALTERADO: Seta para Cima (Linux/macOS)
            player2Paddle.y -= 40; // VELOCIDADE ALTERADA de 25 para 40
        }
        if (key == 65364) { // CÓDIGO ALTERADO: Seta para Baixo (Linux/macOS)
            player2Paddle.y += 40; // VELOCIDADE ALTERADA de 25 para 40
        }

        // --- 2. Atualizar Estado do Jogo ---

        // Manter as raquetes dentro da tela
        if (player1Paddle.y < 0) player1Paddle.y = 0;
        if (player1Paddle.y > WINDOW_HEIGHT - PADDLE_HEIGHT) player1Paddle.y = WINDOW_HEIGHT - PADDLE_HEIGHT;
        
        if (player2Paddle.y < 0) player2Paddle.y = 0;
        if (player2Paddle.y > WINDOW_HEIGHT - PADDLE_HEIGHT) player2Paddle.y = WINDOW_HEIGHT - PADDLE_HEIGHT;

        // Mover a bola
        ball.center.x += ball.velocity.x;
        ball.center.y += ball.velocity.y;

        // Colisão com as paredes superior e inferior
        if (ball.center.y <= BALL_RADIUS || ball.center.y >= WINDOW_HEIGHT - BALL_RADIUS) {
            ball.velocity.y *= -1;
        }

        // Verificação de pontuação
        if (ball.center.x <= 0) {
            player2Score++;
            cout << "Ponto para o Jogador 2! Placar: " << player1Score << " x " << player2Score << endl;
            resetBall(ball);
        } else if (ball.center.x >= WINDOW_WIDTH) {
            player1Score++;
            cout << "Ponto para o Jogador 1! Placar: " << player1Score << " x " << player2Score << endl;
            resetBall(ball);
        }

        // Colisão com as raquetes
        Rect ballRect(ball.center.x - BALL_RADIUS, ball.center.y - BALL_RADIUS, BALL_RADIUS * 2, BALL_RADIUS * 2);
        if ((ballRect & player1Paddle).area() > 0 || (ballRect & player2Paddle).area() > 0) {
            ball.velocity.x *= -1.05;
        }

        // --- 3. Renderizar (Desenhar) o Jogo ---
        
        rectangle(frame, player1Paddle, Scalar(255, 255, 255), FILLED);
        rectangle(frame, player2Paddle, Scalar(255, 255, 255), FILLED);
        circle(frame, ball.center, BALL_RADIUS, Scalar(255, 255, 255), FILLED);

        for (int i = 0; i < WINDOW_HEIGHT; i += 20) {
            line(frame, Point(WINDOW_WIDTH / 2, i), Point(WINDOW_WIDTH / 2, i + 10), Scalar(255, 255, 255), 2);
        }

        string scoreText = to_string(player1Score) + "   " + to_string(player2Score);
        putText(frame, scoreText, Point(WINDOW_WIDTH / 2 - 60, 50), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255, 255, 255), 2);

        // --- 4. Exibir o Quadro ---
        imshow("Pong - 2 Jogadores", frame);
    }

    destroyAllWindows();
    return 0;
}