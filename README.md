# Controle para o jogo *Don't Starve*

<img alt="Capa do Jogo" src="https://imgur.com/a/wQCsYSz.jpeg">

[*Don't Starve*](https://store.steampowered.com/app/219740/Dont_Starve/?l=portuguese) é um jogo de sobrevivência e exploração hardcore em que você precisa sobreviver em um local desconhecido, coletando recursos, criando itens e se defendendo de criaturas.

### Sobre o Projeto

O projeto se trata de um prototipar um controle bluetooth mais simples que o normal e que, ainda sim, permita uma experiência completa do jogo.

<img alt="Imagem do Protótipo" src="">

O controle foi desenvolvido e prototipado para a APS 2 da disciplina *Computação Embarcada* Insper 2024.1. O enunciado da tarefa está disponível nesse [link](https://insper-embarcados.github.io/site/aps/aps-2-controle/).

### Funcionalidades do Controle
- `Joystick`: controla o mouse ou movimentação no jogo [ WASD ]
- `Botão Pequeno`: troca a função do *joystick* entre *mouse* e *WASD* 
- `Botão do Joystick`: Botão esquerdo do mouse
- `Botão Azul`: Coletar [ Space ]
- `Botão Azul`: Atacar [ F ]
- `Botão Cinza`: Cancelar [ Esc ]
- `Botão Branco`: Confirmar [ Enter ]
- `Botão Preto`: Botão direito do mouse

#### [Vídeo Demonstrativo](https://www.youtube.com/watch?v=-XfZkNZ6zs0)

### Desenvolvimento e Prototipação

O código do controle foi desenvolvido utilizando a *Linguagem C* fazendo uso de RTOS, tasks, queues e semaphores seguindo o diagrama abaixo.

<img alt="Estrutura do Código" src="https://imgur.com/a/PJG05R2.jpeg">

Após receber e filtrar os inputs dos botões e analógicos, o dados são trasmitidos para o computador por meio da conexão *Bluetooth*.

Para realizar a interpretação dos dados recebidos pelo controle, foi desenvolvido um [código em *python*](./python/main.py) que decodifica a informação e a utiliza para emular os botões do jogo usando a biblioteca *python-uinput*.

O protótipo físico foi construido utilizando os seguintes componentes e materiais:
- 1 Raspiberry Pi Pico;
- 1 Móludo Bluetooth HC-06
- 1 Led Branco;
- 1 led RGB;
- 6 botões *push up* coloridos simples;
- 1 joystick
- 1 teclado com 12 botões;
- 1 resistor de 330 $\Omega$;
- 1 resistor de 470 $\Omega$;
- 1 bateria Lipo de 530 mAh;
- 1 switch on/off;
- Jumpers e fios rígidos;
- MDF 3mm.


### Critérios Avaliativos
    C : Desenvolver um controle bluetooth para um jogo espefícico com protótipo mecÂnico costumizado, sequindo os critérios disponpiveis na rubrica;
    C+: Apresentar um protótipo físico completo e funcional do controle;
    B : Organizar o código em arquivos .c e .h separando o código em funções para maior organização;
    B+: Utiliza algum componente não visto em sala ( Keypad );
    A : Jogo escolhido é completamente jogável com o controle;  
    A+: 

### Desenvolvedores

- Carlos Eduardo Porciuncula Yamada
- Rafael Dourado Bastos de Oliveira