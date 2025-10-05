// Bibliotecas para OpenCV (visão computacional)
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
// Bibliotecas para SDL (áudio)
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
// Bibliotecas padrão C++
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
// Classes do projeto
#include "Bola.hpp"
#include "Jogador.hpp"

// Usar os namespaces para facilitar a escrita do código
using namespace cv;
using namespace std;

// --- Constantes do Jogo ---
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;

const int BALL_RADIUS = 10;



int main() {
    // --- Inicialização ---
    srand(time(0)); // Semente para números aleatórios

    while (true) {
        // Seleção do modo de jogo
        int gameMode;
        cout << "=== PONG COM DETECÇÃO FACIAL ===" << endl;
        cout << "1 - Singleplayer (vs IA)" << endl;
        cout << "2 - Multiplayer (2 jogadores)" << endl;
        cout << "0 - Sair" << endl;
        cout << "Escolha o modo: ";
        cin >> gameMode;
        
        if (gameMode == 0) break; // Sai do programa
        
        // Define se é modo singleplayer
        bool singlePlayer = (gameMode == 1);

    // Inicializa a webcam (dispositivo 0)
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Erro: Não foi possível abrir a webcam!" << endl;
        return -1;
    }

    // Carrega o classificador Haar Cascade para detecção de faces
    CascadeClassifier faceCascade;
    if (!faceCascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml")) {
        cout << "Erro: Não foi possível carregar o classificador de faces!" << endl;
        return -1;
    }

    // Inicializa SDL2 para sistema de áudio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        cout << "Erro ao inicializar SDL: " << SDL_GetError() << endl;
    }
    // Configura mixer de áudio (44.1kHz, formato padrão, estéreo, buffer 2048)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "Erro ao inicializar SDL_mixer: " << Mix_GetError() << endl;
    }
    
    // Carrega músicas do jogo
    Mix_Music* music = Mix_LoadMUS("assets/sound/soundtrack.mp3");
    Mix_Music* victoryMusic = Mix_LoadMUS("assets/sound/vitoria.mp3");
    if (music) {
        Mix_VolumeMusic(19); // 15% do volume (128 = 100%)
        Mix_PlayMusic(music, -1); // -1 = loop infinito
    }
    
    // Carrega efeitos sonoros
    Mix_Chunk* player1Sound = Mix_LoadWAV("assets/sound/player1.mp3");
    Mix_Chunk* player2Sound = Mix_LoadWAV("assets/sound/player2.mp3");
    Mix_Chunk* golSound = Mix_LoadWAV("assets/sound/gol.mp3");
    if (golSound) Mix_VolumeChunk(golSound, 32); // 25% do volume (128 = 100%)
    
    // Cria a janela do jogo com título baseado no modo
    string windowTitle = singlePlayer ? "Pong - vs IA" : "Pong - 2 Jogadores";
    namedWindow(windowTitle, WINDOW_AUTOSIZE);

    // Cria objetos do jogo
    // Jogador 1 (lado esquerdo)
    Jogador player1("Jogador 1", PADDLE_WIDTH, (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT, WINDOW_HEIGHT);
    // Jogador 2 (lado direito) - IA no singleplayer
    Jogador player2(singlePlayer ? "IA" : "Jogador 2", WINDOW_WIDTH - (PADDLE_WIDTH * 2), (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT, WINDOW_HEIGHT);
    // Bola do jogo
    Bola bola(WINDOW_WIDTH, WINDOW_HEIGHT, BALL_RADIUS);
    
    // Carrega recorde salvo do arquivo
    int record = 0;
    ifstream recordFile("assets/text/record.txt");
    if (recordFile.is_open()) {
        recordFile >> record;
        recordFile.close();
    }

    // Variáveis para detecção facial
    vector<Rect> faces; // Lista de faces detectadas
    int player1FaceY = WINDOW_HEIGHT / 2; // Posição Y da face do jogador 1
    int player2FaceY = WINDOW_HEIGHT / 2; // Posição Y da face do jogador 2
    Mat webcamFrame, grayFrame; // Frames da webcam (colorido e cinza)
    
    // Sistema de timer para início do jogo
    bool gameStarted = false; // Flag se o jogo começou
    int countdown = 3; // Contador regressivo
    auto countdownStart = chrono::steady_clock::now(); // Tempo de início

    // --- Game Loop Principal ---
    while (true) {
        // Captura frame da webcam
        cap >> webcamFrame;
        if (webcamFrame.empty()) break; // Sai se não conseguir capturar
        
        // Espelha horizontalmente para efeito natural
        flip(webcamFrame, webcamFrame, 1);

        // Converte para escala de cinza (necessário para detecção)
        cvtColor(webcamFrame, grayFrame, COLOR_BGR2GRAY);
        
        // Detecta faces usando Haar Cascade
        faceCascade.detectMultiScale(grayFrame, faces, 1.1, 3, 0, Size(30, 30));
        
        // Controle das raquetes baseado na detecção facial
        if (!singlePlayer && faces.size() >= 2) {
            // Multiplayer: organiza faces por posição (esquerda/direita)
            Rect leftFace = faces[0];
            Rect rightFace = faces[1];
            
            // Ordena faces por posição X (esquerda para direita)
            if (faces[0].x > faces[1].x) {
                leftFace = faces[1];
                rightFace = faces[0];
            }
            
            // Face esquerda controla jogador 1 (raquete esquerda)
            player1FaceY = leftFace.y + leftFace.height / 2;
            int mappedY1 = (player1FaceY * WINDOW_HEIGHT) / webcamFrame.rows;
            player1.updateRaquete(mappedY1);
            rectangle(webcamFrame, leftFace, Scalar(0, 255, 0), 2); // Retângulo verde
            putText(webcamFrame, "P1", Point(leftFace.x, leftFace.y - 10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
            
            // Face direita controla jogador 2 (raquete direita)
            player2FaceY = rightFace.y + rightFace.height / 2;
            int mappedY2 = (player2FaceY * WINDOW_HEIGHT) / webcamFrame.rows;
            player2.updateRaquete(mappedY2);
            rectangle(webcamFrame, rightFace, Scalar(0, 0, 255), 2); // Retângulo vermelho
            putText(webcamFrame, "P2", Point(rightFace.x, rightFace.y - 10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2);
        } else if (faces.size() >= 1) {
            // Singleplayer: primeira face controla jogador 1
            player1FaceY = faces[0].y + faces[0].height / 2;
            int mappedY1 = (player1FaceY * WINDOW_HEIGHT) / webcamFrame.rows;
            player1.updateRaquete(mappedY1);
            rectangle(webcamFrame, faces[0], Scalar(0, 255, 0), 2);
            putText(webcamFrame, "P1", Point(faces[0].x, faces[0].y - 10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        }
        
        if (singlePlayer) {
            // IA controla jogador 2 automaticamente
            player2.aiUpdate(bola.getPosition().y, PADDLE_HEIGHT);
        }

        // Redimensiona frame da webcam para o tamanho da janela do jogo
        Mat frame;
        resize(webcamFrame, frame, Size(WINDOW_WIDTH, WINDOW_HEIGHT));

        // --- 1. Processar Entradas (Teclado para sair) ---
        int key = waitKeyEx(1);
        if (key == 27 || key == 'q' || key == 'Q') { // Tecla ESC ou Q para sair
            // Para música do jogo e inicia música de vitória
            Mix_HaltMusic();
            if (victoryMusic) {
                Mix_PlayMusic(victoryMusic, 0);
            }
            
            // Exibe tela de vitória com vídeo de fundo
            VideoCapture victoryVideo("assets/video/vitoria.mp4");
            if (victoryVideo.isOpened()) {
                Mat victoryFrame;
                string congratsText;
                // Define mensagem baseada no resultado
                if (player1.getScore() == player2.getScore()) {
                    congratsText = "Deu Empate!!";
                } else if (singlePlayer) {
                    if (player1.getScore() > player2.getScore()) {
                        congratsText = "Parabens, voce ganhou da maquina!";
                    } else {
                        congratsText = "Voce perdeu, tem que treinar mais viu...";
                    }
                } else {
                    string winner = (player1.getScore() > player2.getScore()) ? player1.getName() : player2.getName();
                    congratsText = "Parabens " + winner + ", voce venceu!!";
                }
                
                // Calcula delay baseado no FPS do vídeo
                double fps = victoryVideo.get(CAP_PROP_FPS);
                int delay = (fps > 0) ? (1000 / fps) : 33;
                
                // Loop de reprodução do vídeo de vitória
                while (true) {
                    victoryVideo >> victoryFrame;
                    if (victoryFrame.empty()) break; // Fim do vídeo
                    
                    // Redimensiona vídeo e adiciona texto
                    resize(victoryFrame, victoryFrame, Size(WINDOW_WIDTH, WINDOW_HEIGHT));
                    int textWidth = getTextSize(congratsText, FONT_HERSHEY_SIMPLEX, 1.0, 2, 0).width;
                    putText(victoryFrame, congratsText, Point((WINDOW_WIDTH - textWidth)/2, WINDOW_HEIGHT/2), 
                           FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 0), 2);
                    
                    imshow(windowTitle, victoryFrame);
                    waitKey(delay);
                }
                victoryVideo.release();
            }
            break; // Sai do loop principal
        }

        // --- 2. Atualizar Estado do Jogo ---
        
        // Sistema de contagem regressiva antes do início
        if (!gameStarted) {
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(now - countdownStart).count();
            
            // Decrementa contador a cada segundo
            if (elapsed >= 1) {
                countdown--;
                countdownStart = now;
                if (countdown <= 0) {
                    gameStarted = true; // Inicia o jogo
                }
            }
        }

        // Atualiza posição da bola apenas se o jogo começou
        if (gameStarted) {
            bola.update();
        }

        // Verifica colisão com paredes superior e inferior
        if (bola.getPosition().y <= BALL_RADIUS || bola.getPosition().y >= WINDOW_HEIGHT - BALL_RADIUS) {
            bola.reverseY(); // Inverte direção vertical
        }

        // Sistema de pontuação
        if (gameStarted && bola.getPosition().x <= 0) {
            // Bola saiu pela esquerda - ponto para jogador 2
            player2.addPoint();
            if (golSound) Mix_PlayChannel(-1, golSound, 0); // Toca som de gol
            cout << "Ponto para " << player2.getName() << "! Placar: " << player1.getScore() << " x " << player2.getScore() << endl;
            
            // Verifica e atualiza recorde se necessário
            int maxScore = singlePlayer ? player1.getScore() : max(player1.getScore(), player2.getScore());
            if (maxScore > record) {
                record = maxScore;
                ofstream recordOut("assets/text/record.txt");
                recordOut << record;
                recordOut.close();
            }
            // Reinicia jogo com nova contagem regressiva
            bola.reset(WINDOW_WIDTH, WINDOW_HEIGHT);
            gameStarted = false;
            countdown = 3;
            countdownStart = chrono::steady_clock::now();
        } else if (gameStarted && bola.getPosition().x >= WINDOW_WIDTH) {
            // Bola saiu pela direita - ponto para jogador 1
            player1.addPoint();
            if (golSound) Mix_PlayChannel(-1, golSound, 0); // Toca som de gol
            cout << "Ponto para " << player1.getName() << "! Placar: " << player1.getScore() << " x " << player2.getScore() << endl;
            
            // Verifica e atualiza recorde se necessário
            int maxScore = singlePlayer ? player1.getScore() : max(player1.getScore(), player2.getScore());
            if (maxScore > record) {
                record = maxScore;
                ofstream recordOut("assets/text/record.txt");
                recordOut << record;
                recordOut.close();
            }
            // Reinicia jogo com nova contagem regressiva
            bola.reset(WINDOW_WIDTH, WINDOW_HEIGHT);
            gameStarted = false;
            countdown = 3;
            countdownStart = chrono::steady_clock::now();
        }

        // Sistema de colisão com raquetes
        if (gameStarted && bola.checkCollision(player1.getRaqueteBounds())) {
            // Colisão com raquete do jogador 1 (esquerda)
            bola.setVelocityX(abs(bola.getVelocityX()) * 1.02f); // Aumenta velocidade 2%
            bola.setPosition(player1.getRaqueteBounds().x + player1.getRaqueteBounds().width + BALL_RADIUS, bola.getPosition().y);
            if (player1Sound) Mix_PlayChannel(-1, player1Sound, 0); // Som do jogador 1
        } else if (gameStarted && bola.checkCollision(player2.getRaqueteBounds())) {
            // Colisão com raquete do jogador 2 (direita)
            bola.setVelocityX(-abs(bola.getVelocityX()) * 1.02f); // Inverte direção e aumenta velocidade
            bola.setPosition(player2.getRaqueteBounds().x - BALL_RADIUS, bola.getPosition().y);
            if (player2Sound) Mix_PlayChannel(-1, player2Sound, 0); // Som do jogador 2
        }

        // --- 3. Renderização (Desenho) do Jogo ---
        
        // Desenha elementos do jogo
        player1.drawRaquete(frame);
        player2.drawRaquete(frame);
        bola.draw(frame);

        // Desenha linha central pontilhada
        for (int i = 0; i < WINDOW_HEIGHT; i += 20) {
            line(frame, Point(WINDOW_WIDTH / 2, i), Point(WINDOW_WIDTH / 2, i + 10), Scalar(255, 255, 255), 2);
        }

        // Desenha placar com borda branca e texto preto
        string scoreText = to_string(player1.getScore()) + "   " + to_string(player2.getScore());
        putText(frame, scoreText, Point(WINDOW_WIDTH / 2 - 60, 50), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255, 255, 255), 4); // Borda branca
        putText(frame, scoreText, Point(WINDOW_WIDTH / 2 - 60, 50), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 0), 2); // Texto preto
        
        // Exibe recorde atual
        string recordText = "Record: " + to_string(record);
        putText(frame, recordText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 0), 2);
        
        // Exibe contagem regressiva quando jogo não iniciou
        if (!gameStarted && countdown > 0) {
            string countdownText = to_string(countdown);
            int textWidth = getTextSize(countdownText, FONT_HERSHEY_SIMPLEX, 3.0, 4, 0).width;
            // Texto grande com borda branca e preenchimento vermelho
            putText(frame, countdownText, Point((WINDOW_WIDTH - textWidth)/2, WINDOW_HEIGHT/2 + 50), 
                   FONT_HERSHEY_SIMPLEX, 3.0, Scalar(255, 255, 255), 6); // Borda branca
            putText(frame, countdownText, Point((WINDOW_WIDTH - textWidth)/2, WINDOW_HEIGHT/2 + 50), 
                   FONT_HERSHEY_SIMPLEX, 3.0, Scalar(255, 0, 0), 4); // Texto vermelho
        }

        // --- 4. Exibe o Frame Final ---
        imshow(windowTitle, frame);
    }

        // --- Limpeza de Recursos da Partida ---
        // Libera recursos de áudio
        if (music) {
            Mix_FreeMusic(music);
        }
        if (victoryMusic) {
            Mix_FreeMusic(victoryMusic);
        }
        if (player1Sound) {
            Mix_FreeChunk(player1Sound);
        }
        if (player2Sound) {
            Mix_FreeChunk(player2Sound);
        }
        if (golSound) {
            Mix_FreeChunk(golSound);
        }
        
        // Libera recursos do OpenCV
        cap.release(); // Libera webcam
        destroyAllWindows(); // Fecha janelas
        
        cout << "\n\nDeseja jogar novamente?\n" << endl;
    }
    
    // --- Limpeza Final ---
    Mix_CloseAudio(); // Fecha sistema de áudio
    SDL_Quit(); // Finaliza SDL
    cout << "Obrigado por jogar!" << endl;
    return 0; // Programa finalizado com sucesso
}