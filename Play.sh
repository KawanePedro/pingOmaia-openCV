#!/bin/bash

# Auto-permissão: torna este script executável
chmod +x "$0"

# Cria a pasta output se não existir
mkdir -p output

# Compila o código
echo "Iniciando o jogo..."
g++ -o output/pongCore src/main.cpp src/Bola.cpp src/Raquete.cpp src/Jogador.cpp `pkg-config --cflags --libs opencv4` -lSDL2 -lSDL2_mixer

# Verifica se a compilação foi bem-sucedida
if [ $? -eq 0 ]; then
    ./output/pongCore
else
    echo "Erro ao iniciar o jogo!"
    exit 1
fi