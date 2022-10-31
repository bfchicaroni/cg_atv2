
# Paredão

Nome: Beatriz Favini Chicaroni  
RA 11201721608

O projeto é um protótipo do jogo conhecido como Paredão do Telejogo da Philco.

O objetivo do jogo é impedir a bolinha de chegar na margem inferior.

Na versão web a barrinha não aparece, mas na versão desktop aparece e obedece aos comando das setas do teclado.
Eu não consegui implementar a colisão entre a barra e a bolinha.

Os atributos e métodos da bolinha estão no arquivo ball.cpp, os da barra estão em bar.cpp e o arquivo window.cpp chama os métodos desses elementos e controla o estado do jogo.

Nas linhas 35 a 42 de ball.cpp está definido o formato e tamanho da bolinha que, na verdade, é um polígono de 20 lados. Nas linhas 97 a 103 de ball.cpp está implementada a lógica da colisão da bolinha com as paredes. 

Nas linhas 20 a 32 de bar.cpp está definido o formato da barrinha, que é formada por dois triângulos que compartilham dois vértices entre si. Nas linhas 112 a 125 de bar.cpp está implementada a reação que a barrinha deve ter a cada seta do teclado e os limites da tela.
