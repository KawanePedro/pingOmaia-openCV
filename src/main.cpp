#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
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

    // Seleção do modo de jogo
    int gameMode;
    cout << "=== PONG COM DETECÇÃO FACIAL ===" << endl;
    cout << "1 - Singleplayer (vs IA)" << endl;
    cout << "2 - Multiplayer (2 jogadores)" << endl;
    cout << "Escolha o modo: ";
    cin >> gameMode;
    
    bool singlePlayer = (gameMode == 1);

    // Inicializa a webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Erro: Não foi possível abrir a webcam!" << endl;
        return -1;
    }

    // Carrega o classificador de faces
    CascadeClassifier faceCascade;
    if (!faceCascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml")) {
        cout << "Erro: Não foi possível carregar o classificador de faces!" << endl;
        return -1;
    }

    // Inicializa SDL2 para áudio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        cout << "Erro ao inicializar SDL: " << SDL_GetError() << endl;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "Erro ao inicializar SDL_mixer: " << Mix_GetError() << endl;
    }
    
    // Carrega e toca a música
    Mix_Music* music = Mix_LoadMUS("assets/sound/soundtrack.mp3");
    Mix_Music* victoryMusic = Mix_LoadMUS("assets/sound/vitoria.mp3");
    if (music) {
        Mix_VolumeMusic(38); // 30% do volume (128 = 100%)
        Mix_PlayMusic(music, -1); // -1 = loop infinito
    }
    
    // Carrega sons dos jogadores
    Mix_Chunk* player1Sound = Mix_LoadWAV("assets/sound/player1.mp3");
    Mix_Chunk* player2Sound = Mix_LoadWAV("assets/sound/player2.mp3");
    
    // Cria a janela do jogo
    string windowTitle = singlePlayer ? "Pong - vs IA" : "Pong - 2 Jogadores";
    namedWindow(windowTitle, WINDOW_AUTOSIZE);

    // Cria os jogadores e a bola
    Jogador player1("Jogador 1", PADDLE_WIDTH, (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT, WINDOW_HEIGHT);
    Jogador player2(singlePlayer ? "IA" : "Jogador 2", WINDOW_WIDTH - (PADDLE_WIDTH * 2), (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2), PADDLE_WIDTH, PADDLE_HEIGHT, WINDOW_HEIGHT);
    Bola bola(WINDOW_WIDTH, WINDOW_HEIGHT, BALL_RADIUS);
    
    // Carrega recorde
    int record = 0;
    ifstream recordFile("assets/text/record.txt");
    if (recordFile.is_open()) {
        recordFile >> record;
        recordFile.close();
    }

    // Variáveis para controle facial
    vector<Rect> faces;
    int player1FaceY = WINDOW_HEIGHT / 2;
    int player2FaceY = WINDOW_HEIGHT / 2;
    Mat webcamFrame, grayFrame;

    // --- Game Loop Principal ---
    while (true) {
        // Captura frame da webcam
        cap >> webcamFrame;
        if (webcamFrame.empty()) break;
        
        // Flip horizontal para efeito espelho
        flip(webcamFrame, webcamFrame, 1);

        // Converte para escala de cinza para detecção
        cvtColor(webcamFrame, grayFrame, COLOR_BGR2GRAY);
        
        // Detecta faces
        faceCascade.detectMultiScale(grayFrame, faces, 1.1, 3, 0, Size(30, 30));
        
        // Atualiza posições das raquetes baseado nas faces detectadas
        if (faces.size() >= 1) {
            // Primeira face controla jogador 1
            player1FaceY = faces[0].y + faces[0].height / 2;
            int mappedY1 = (player1FaceY * WINDOW_HEIGHT) / webcamFrame.rows;
            player1.updateRaquete(mappedY1);
            
            // Desenha retângulo na primeira face
            rectangle(webcamFrame, faces[0], Scalar(0, 255, 0), 2);
            putText(webcamFrame, "P1", Point(faces[0].x, faces[0].y - 10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        }
        
        if (!singlePlayer && faces.size() >= 2) {
            // Segunda face controla jogador 2 (apenas no multiplayer)
            player2FaceY = faces[1].y + faces[1].height / 2;
            int mappedY2 = (player2FaceY * WINDOW_HEIGHT) / webcamFrame.rows;
            player2.updateRaquete(mappedY2);
            
            // Desenha retângulo na segunda face
            rectangle(webcamFrame, faces[1], Scalar(0, 0, 255), 2);
            putText(webcamFrame, "P2", Point(faces[1].x, faces[1].y - 10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2);
        } else if (singlePlayer) {
            // IA controla jogador 2
            player2.aiUpdate(bola.getPosition().y, PADDLE_HEIGHT);
        }

        // Redimensiona a webcam para o tamanho da janela do jogo
        Mat frame;
        resize(webcamFrame, frame, Size(WINDOW_WIDTH, WINDOW_HEIGHT));

        // --- 1. Processar Entradas (Teclado para sair) ---
        int key = waitKeyEx(1);
        if (key == 27 || key == 'q' || key == 'Q') { // Tecla ESC ou Q para sair
            // Para a música do jogo e toca o áudio do vídeo
            Mix_HaltMusic();
            if (victoryMusic) {
                Mix_PlayMusic(victoryMusic, 0);
            }
            
            // Tela de vitória
            VideoCapture victoryVideo("assets/video/vitoria.mp4");
            if (victoryVideo.isOpened()) {
                Mat victoryFrame;
                string congratsText;
                if (singlePlayer) {
                    if (player1.getScore() > player2.getScore()) {
                        congratsText = "Parabens, voce ganhou da maquina!";
                    } else {
                        congratsText = "Voce perdeu, tem que treinar mais viu...";
                    }
                } else {
                    string winner = (player1.getScore() > player2.getScore()) ? player1.getName() : player2.getName();
                    congratsText = "Parabens " + winner + ", voce venceu!!";
                }
                
                double fps = victoryVideo.get(CAP_PROP_FPS);
                int delay = (fps > 0) ? (1000 / fps) : 33;
                
                while (true) {
                    victoryVideo >> victoryFrame;
                    if (victoryFrame.empty()) break;
                    
                    resize(victoryFrame, victoryFrame, Size(WINDOW_WIDTH, WINDOW_HEIGHT));
                    int textWidth = getTextSize(congratsText, FONT_HERSHEY_SIMPLEX, 1.0, 2, 0).width;
                    putText(victoryFrame, congratsText, Point((WINDOW_WIDTH - textWidth)/2, WINDOW_HEIGHT/2), 
                           FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 0), 2);
                    
                    imshow(windowTitle, victoryFrame);
                    waitKey(delay);
                }
                victoryVideo.release();
            }
            break;
        }

        // --- 2. Atualizar Estado do Jogo ---

        // Mover a bola
        bola.update();

        // Colisão com as paredes superior e inferior
        if (bola.getPosition().y <= BALL_RADIUS || bola.getPosition().y >= WINDOW_HEIGHT - BALL_RADIUS) {
            bola.reverseY();
        }

        // Verificação de pontuação
        if (bola.getPosition().x <= 0) {
            player2.addPoint();
            cout << "Ponto para " << player2.getName() << "! Placar: " << player1.getScore() << " x " << player2.getScore() << endl;
            
            // Atualiza recorde se necessário
            int maxScore = singlePlayer ? player1.getScore() : max(player1.getScore(), player2.getScore());
            if (maxScore > record) {
                record = maxScore;
                ofstream recordOut("assets/text/record.txt");
                recordOut << record;
                recordOut.close();
            }
            bola.reset(WINDOW_WIDTH, WINDOW_HEIGHT);
        } else if (bola.getPosition().x >= WINDOW_WIDTH) {
            player1.addPoint();
            cout << "Ponto para " << player1.getName() << "! Placar: " << player1.getScore() << " x " << player2.getScore() << endl;
            
            // Atualiza recorde se necessário
            int maxScore = singlePlayer ? player1.getScore() : max(player1.getScore(), player2.getScore());
            if (maxScore > record) {
                record = maxScore;
                ofstream recordOut("assets/text/record.txt");
                recordOut << record;
                recordOut.close();
            }
            bola.reset(WINDOW_WIDTH, WINDOW_HEIGHT);
        }

        // Colisão com as raquetes
        if (bola.checkCollision(player1.getRaqueteBounds())) {
            bola.setVelocityX(abs(bola.getVelocityX()) * 1.02f);
            bola.setPosition(player1.getRaqueteBounds().x + player1.getRaqueteBounds().width + BALL_RADIUS, bola.getPosition().y);
            if (player1Sound) Mix_PlayChannel(-1, player1Sound, 0);
        } else if (bola.checkCollision(player2.getRaqueteBounds())) {
            bola.setVelocityX(-abs(bola.getVelocityX()) * 1.02f);
            bola.setPosition(player2.getRaqueteBounds().x - BALL_RADIUS, bola.getPosition().y);
            if (player2Sound) Mix_PlayChannel(-1, player2Sound, 0);
        }

        // --- 3. Renderizar (Desenhar) o Jogo ---
        
        player1.drawRaquete(frame);
        player2.drawRaquete(frame);
        bola.draw(frame);

        for (int i = 0; i < WINDOW_HEIGHT; i += 20) {
            line(frame, Point(WINDOW_WIDTH / 2, i), Point(WINDOW_WIDTH / 2, i + 10), Scalar(255, 255, 255), 2);
        }

        string scoreText = to_string(player1.getScore()) + "   " + to_string(player2.getScore());
        putText(frame, scoreText, Point(WINDOW_WIDTH / 2 - 60, 50), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255, 255, 255), 2);
        
        // Exibe recorde
        string recordText = "Record: " + to_string(record);
        putText(frame, recordText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 0), 2);

        // --- 4. Exibir o Quadro ---
        imshow(windowTitle, frame);
    }

    // Limpa SDL
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
    Mix_CloseAudio();
    SDL_Quit();
    
    cap.release();
    destroyAllWindows();
    return 0;
}