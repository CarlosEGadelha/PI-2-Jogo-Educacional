// Inclui o arquivo de cabeçalho da biblioteca Allegro 5
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_native_dialog.h>

//fps = frames per second = atualizacoes de tela por segundo
#define FPS 60.0
#define LARGURA_TELA 800
#define ALTURA_TELA 600

typedef struct { int x, y, lim_x_1, lim_y_1, lim_x_2, lim_y_2, direcao_x, direcao_y; } caixa;
int VidaPlayer = 3, vidaInimigo = 0, start = 0, start_dois = 0, start_tres, fase_atual = 0, qntPocao = 3, comeco = 0, inimigoAtual = 0;
//fase01 = 0,
int res_x_comp, res_y_comp;
caixa* ultimoColidido = NULL;
//criação das variaveis dos objetos
caixa player, inimigosFase1[2], inimigosFase2[2], inimigosFase3[2], saidaFase01, voltaFase01, saidaFase02, voltaFase02;

ALLEGRO_MONITOR_INFO info;

ALLEGRO_DISPLAY* janela = NULL;
ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
ALLEGRO_FONT* fonte = NULL;
ALLEGRO_FONT* atacar = NULL;
ALLEGRO_FONT* item = NULL;
ALLEGRO_FONT* fugir = NULL;
ALLEGRO_FONT* caverna = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_BITMAP* quadrado = NULL;
ALLEGRO_BITMAP* phantasoma = NULL;
ALLEGRO_BITMAP* inimigo_subtracao = NULL;
ALLEGRO_BITMAP* inimigo_subtracao_esquerda = NULL;
ALLEGRO_BITMAP* inimigo_subtracao_direita = NULL;
ALLEGRO_BITMAP* inimigo_subtracao_costas = NULL;
ALLEGRO_BITMAP* itemPocao = NULL;
ALLEGRO_BITMAP* direita = NULL;
ALLEGRO_BITMAP* esquerda = NULL;
ALLEGRO_BITMAP* cima = NULL;
ALLEGRO_BITMAP* baixo = NULL;
ALLEGRO_BITMAP* parado = NULL;
ALLEGRO_BITMAP* fundo = NULL;
//ALLEGRO_BITMAP* fundo_dois = NULL;
ALLEGRO_BITMAP* fundoBatalha = NULL;
ALLEGRO_AUDIO_STREAM* musica = NULL;
ALLEGRO_BITMAP* imagem = NULL;
ALLEGRO_BITMAP* botao_sair = NULL;
ALLEGRO_BITMAP* jogar = 0;
ALLEGRO_BITMAP* instrucoes = 0;
ALLEGRO_BITMAP* menuDesenvolvedores = 0;
ALLEGRO_BITMAP* voltarMenu = 0;
ALLEGRO_BITMAP* fonteTitulo = 0;
ALLEGRO_FONT* zero = NULL;
ALLEGRO_FONT* um = NULL;
ALLEGRO_FONT* dois = NULL;
ALLEGRO_FONT* tres = NULL;
ALLEGRO_FONT* quatro = NULL;
ALLEGRO_FONT* cinco = NULL;
ALLEGRO_FONT* seis = NULL;
ALLEGRO_FONT* sete = NULL;
ALLEGRO_FONT* oito = NULL;
ALLEGRO_FONT* nove = NULL;
ALLEGRO_FONT* fonteCalculo = NULL;
ALLEGRO_FONT* calculitoBatalha = NULL;
ALLEGRO_FONT* subtracaoBatalha = NULL;
ALLEGRO_FONT* phantasomaBatalha = NULL;
ALLEGRO_FONT* fonteVida = NULL;
ALLEGRO_TIMER* fundoItem = NULL;

ALLEGRO_BITMAP* ultimoFundo = NULL;
ALLEGRO_BITMAP* boss = NULL;

void error_msg(char* text) {
    al_show_native_message_box(janela, "ERRO",
        "Ocorreu o seguinte erro e o programa sera finalizado:",
        text, NULL, ALLEGRO_MESSAGEBOX_ERROR);
}

int inicializar(int fase) {
    if (!al_init()) {
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }

    //cria o timer com o intervalo de tempo que ele ira disparar
    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        error_msg("Falha ao criar temporizador");
        return 0;
    }

    //inicialização do add-on de audio
    if (!al_install_audio()) {
        error_msg("Falha ao inicializar o audio");
        return 0;
    }

    //addon que da suporte as extensoes de audio
    if (!al_init_acodec_addon()) {
        error_msg("Falha ao inicializar o codec de audio");
        return 0;
    }

    //cria o mixer (e torna ele o mixer padrao), e adciona 2 samples de audio nele
    if (!al_reserve_samples(2)) {
        error_msg("Falha ao reservar amostrar de audio");
        return 0;
    }

    //carrega o stream
    musica = al_load_audio_stream("victory.mp3", 4, 1024);
    if (!musica)
    {
        error_msg("Audio nao carregado");
        return 0;
    }

    //liga o stream no mixer
    al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());

    //define que o stream vai tocar no modo repeat
    al_set_audio_stream_playmode(musica, ALLEGRO_PLAYMODE_LOOP);

    // Inicialização do add-on para uso de fontes
    al_init_font_addon();

    // Inicialização do add-on para uso de fontes True Type
    if (!al_init_ttf_addon()) {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar add-on allegro_image");
        return 0;
    }

    //inicializa addon do teclado
    if (!al_install_keyboard()) {
        error_msg("Falha ao inicializar o teclado");
        return 0;
    }

    fonte = al_load_font("arial.ttf", 48, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return 0;
    }

    if (fase == 1) {
        //Inicia a imagem de fundo
        fundo = al_load_bitmap("cenario/mapa_base_montanha.bmp");
        if (!fundo) {
            error_msg("Falha ao carregar fundo");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            al_destroy_bitmap(direita);
            al_destroy_bitmap(esquerda);
            al_destroy_bitmap(cima);
            al_destroy_bitmap(baixo);
            return 0;
        }

        //carrega a folha de sprites na variavel inimigo para direita
        inimigo_subtracao_direita = al_load_bitmap("sprites/Subtracao_direita.bmp");
        if (!inimigo_subtracao_direita) {
            error_msg("Falha ao carregar sprites inimigo direita");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            return 0;
        }
        al_convert_mask_to_alpha(inimigo_subtracao_direita, al_map_rgb(255, 0, 255));

        //carrega a folha de sprites na variavel inimigo para cima
        inimigo_subtracao_costas = al_load_bitmap("sprites/Subtracao_andando_cima.bmp");
        if (!inimigo_subtracao_costas) {
            error_msg("Falha ao carregar sprites inimigo cima");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            return 0;
        }

        al_convert_mask_to_alpha(inimigo_subtracao_costas, al_map_rgb(255, 0, 255));

        //carrega a folha de sprites na variavel inimigo para esquerda
        inimigo_subtracao_esquerda = al_load_bitmap("sprites/Subtracao_esquerda.bmp");
        if (!inimigo_subtracao_esquerda) {
            error_msg("Falha ao carregar sprites inimigo esquerda");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            return 0;
        }

        al_convert_mask_to_alpha(inimigo_subtracao_esquerda, al_map_rgb(255, 0, 255));

        //carrega a folha de sprites na variavel
        inimigo_subtracao = al_load_bitmap("sprites/Subtracao_andando_baixo.bmp");
        if (!inimigo_subtracao) {
            error_msg("Falha ao carregar sprites inimigo baixo");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            return 0;
        }
        //usa a cor rosa como transparencia
        al_convert_mask_to_alpha(inimigo_subtracao, al_map_rgb(255, 0, 255));
    }
    else if (fase == 2) {
        //Inicia a imagem de fundo
        caverna = al_load_bitmap("cenario/mapa_dentro_montanha.bmp");
        if (!caverna) {
            error_msg("Falha ao carregar fundo");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            al_destroy_bitmap(direita);
            al_destroy_bitmap(esquerda);
            al_destroy_bitmap(cima);
            al_destroy_bitmap(baixo);
            return 0;
        }

        phantasoma = al_load_bitmap("sprites/phantasoma_andando.bmp");
        if (!phantasoma) {
            error_msg("Falha ao carregar sprites inimigo baixo");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            return 0;
        }
        //usa a cor rosa como transparencia
        al_convert_mask_to_alpha(phantasoma, al_map_rgb(255, 0, 255));
    }
    else if (fase == 3) {
        //Inicia a imagem de fundo
        ultimoFundo = al_load_bitmap("cenario/mapa_base_montanha.bmp");
        if (!ultimoFundo) {
            error_msg("Falha ao carregar fundo");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            al_destroy_bitmap(direita);
            al_destroy_bitmap(esquerda);
            al_destroy_bitmap(cima);
            al_destroy_bitmap(baixo);
            return 0;
        }

        boss = al_load_bitmap("sprites/Subtracao_andando_baixo.bmp");
        if (!boss) {
            error_msg("Falha ao carregar sprites inimigo baixo");
            al_destroy_timer(timer);
            al_destroy_display(janela);
            al_destroy_event_queue(fila_eventos);
            return 0;
        }
        //usa a cor rosa como transparencia
        al_convert_mask_to_alpha(boss, al_map_rgb(255, 0, 255));
    }

    //Inicia a imagem para a movimentação do personagem para a Direita
    direita = al_load_bitmap("sprites/movimento_direita.bmp");
    if (!direita) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(direita, al_map_rgb(255, 0, 255));

    //Inicia a imagem para a movimentação do personagem para a Esquerda
    esquerda = al_load_bitmap("sprites/movimento_esquerda.bmp");
    if (!esquerda) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(esquerda, al_map_rgb(255, 0, 255));

    //Inicia a imagem para a movimentação do personagem para a Cima
    cima = al_load_bitmap("sprites/movimento_cima.bmp");
    if (!cima) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(cima, al_map_rgb(255, 0, 255));

    //Inicia a imagem para a movimentação do personagem para a Baixo
    baixo = al_load_bitmap("sprites/movimento_baixo.bmp");
    if (!baixo) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(baixo, al_map_rgb(255, 0, 255));

    parado = al_load_bitmap("sprites/Calculito_parado.bmp");
    if (!parado) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(parado, al_map_rgb(255, 0, 255));

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_bitmap(quadrado);
        al_destroy_audio_stream(musica);

        return 0;
    }

    //registra duas fontes de eventos na fila. o da janela, e do teclado
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    al_set_target_bitmap(al_get_backbuffer(janela));
    fila_eventos, al_get_timer_event_source(timer);
    al_flip_display();
    al_start_timer(timer);

    return 1;
}

int colidiu(caixa box1, caixa box2) {
    if (box1.x + 80 > box2.x && box1.x < box2.x + 80 && box1.y + 170 > box2.y && box1.y < box2.y + 160) {
        return 1;
    }
    else {
        return 0;
    }
}

int deletaInimgo(caixa* inimigo) {
    inimigo->x = 2000;
    inimigo->y = 2000;
    jogo();

    return 0;
}

int* sorteioNumeros() {
    //sorteio dos numeros
    srand(time(0));

    if (fase_atual == 1) {
        //variaveis para sorteio
        static int vetor[2];

        for (int i = 0; i < 2; i++) {
            vetor[i] = rand() % 10;
        }
        int aux;

        if (vetor[0] < vetor[1]) {
            aux = vetor[0];
            vetor[0] = vetor[1];
            vetor[1] = aux;
        }
        return vetor;
    }
    else if (fase_atual == 2) {
        static int vetor2[3];

        vetor2[0] = rand() % 7;
        vetor2[1] = rand() % 7;

        if (vetor2[0] > vetor2[1]) {
            vetor2[2] = vetor2[0] / 2;
        }
        else {
            vetor2[2] = vetor2[1] / 2;
        }

        return vetor2;
    }
    else if (fase_atual == 3) {
        static int vetor3[2];
        int sair = 0;
        int resultado;

        do {
            for (int i = 0; i < 2; i++) {
                vetor3[i] = rand() % 100+1;
            }

            resultado = vetor3[0] / vetor3[1];

            if (vetor3[0] % vetor3[1] == 0) {
                if (resultado >= 0 && resultado <= 9) {
                    sair = 1;
                }
            }
            else {
                sair = 0;
            }

        } while (!sair);

        return vetor3;
    }
}

int resultadoCalculo2(int num1, int num2, int num3) {
    int resultado = num1 + num2;
    resultado = resultado - num3;
    /*printf("\nFuncao resultadoCalculo2 %d", resultado);*/
    return resultado;
}

int resultadoCalculo(int num1, int num2) {
    if (fase_atual == 1) {
        int resultado = num1 - num2;
        printf("\n%d", resultado);
        return resultado;
    }
    else if (fase_atual == 3) {
        int resultado = num1 / num2;
        if (num1 % num2 == 0) {
            if (resultado >= 0 && resultado <= 9) {
                return resultado;
            }
        }
        else {
            sorteioNumeros();
        }
    }
    
}

void acertou() {
    return vidaInimigo -= 1;
}

int errou() {
    //char tcaixa[50] = "OLA";
    //char titulo[100] = "ERROU...";
    //char texto[200] = "BOA SORTE NA PROXIMA!!";
    ////mostra a caixa de texto
    //int r = al_show_native_message_box(NULL, tcaixa, titulo, texto, NULL, ALLEGRO_MESSAGEBOX_QUESTION);

    return VidaPlayer -= 1;

}

int destroyCalculadora() {
    al_destroy_font(fonteCalculo);
    al_destroy_bitmap(zero);
    al_destroy_bitmap(um);
    al_destroy_bitmap(dois);
    al_destroy_bitmap(tres);
    al_destroy_bitmap(quatro);
    al_destroy_bitmap(cinco);
    al_destroy_bitmap(seis);
    al_destroy_bitmap(sete);
    al_destroy_bitmap(oito);
    al_destroy_bitmap(nove);
    al_destroy_event_queue(fila_eventos);
}

int calculadora() {
    if (!al_init()) {
        error_msg("Falha ao inicializar a Allegro");
        return -1;
    }
    al_init_font_addon();

    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar add-on allegro_image");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    fonteCalculo = al_load_font("arial.ttf", 58, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return 0;
    }

    zero = al_load_bitmap("sprites/botao_calculadora_0.bmp");
    if (!zero) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    um = al_load_bitmap("sprites/botao_calculadora_1.bmp");
    if (!um) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    dois = al_load_bitmap("sprites/botao_calculadora_2.bmp");
    if (!dois) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    tres = al_load_bitmap("sprites/botao_calculadora_3.bmp");
    if (!tres) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    quatro = al_load_bitmap("sprites/botao_calculadora_4.bmp");
    if (!quatro) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    cinco = al_load_bitmap("sprites/botao_calculadora_5.bmp");
    if (!cinco) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    seis = al_load_bitmap("sprites/botao_calculadora_6.bmp");
    if (!seis) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    sete = al_load_bitmap("sprites/botao_calculadora_7.bmp");
    if (!sete) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    oito = al_load_bitmap("sprites/botao_calculadora_8.bmp");
    if (!oito) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    nove = al_load_bitmap("sprites/botao_calculadora_9.bmp");
    if (!nove) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    if (!al_install_mouse()) {
        error_msg("Falha ao inicializar o mouse");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
        error_msg("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(janela);
        return -1;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    int menuZero = 0;
    int menuUm = 0;
    int menuDois = 0;
    int menuTres = 0;
    int menuQuatro = 0;
    int menuCinco = 0;
    int menuSeis = 0;
    int menuSete = 0;
    int menuOito = 0;
    int menuNove = 0;
    int menuConfirmar = 0;
    int menuBackspace = 0;
    int* numeros;
    int num1, num2, num3;
    numeros = sorteioNumeros();
    num1 = numeros[0];
    num2 = numeros[1];
    num3 = numeros[2];
    int resultado = 0;
    if (fase_atual == 1) {
        resultado = resultadoCalculo(num1, num2);
    }
    else if (fase_atual == 2) {
        resultado = resultadoCalculo2(num1, num2, num3);
    }
    else if (fase_atual == 3) {
        resultado = resultadoCalculo(num1, num2);
    }
    
    while (!menuConfirmar) {
        al_clear_to_color(al_map_rgb(45, 45, 45));
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            //Verifica se o mouse esta em algmenuUm dos botões
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 350 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 450 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 500 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 550 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 0) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);

                        return 0;

                    }

                }

                if (evento.mouse.x >= 100 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 200 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 400 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 450 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 1) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }

                if (evento.mouse.x >= 350 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 450 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 400 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 450 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 2) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }

                if (evento.mouse.x >= 600 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 700 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 400 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 450 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 3) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }

                if (evento.mouse.x >= 100 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 200 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 300 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 350 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 4) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }

                if (evento.mouse.x >= 350 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 450 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 300 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 350 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 5) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }

                if (evento.mouse.x >= 600 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 700 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 300 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 350 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 6) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }

                if (evento.mouse.x >= 100 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 200 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 200 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 250 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 7) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }

                if (evento.mouse.x >= 350 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 450 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 200 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 250 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 8) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }

                if (evento.mouse.x >= 600 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 700 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 200 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 250 * (res_y_comp / (float)ALTURA_TELA)) {
                    if (resultado == 9) {
                        acertou();
                    }
                    else {
                        errou();
                    }

                    destroyCalculadora();

                    if (VidaPlayer > 0 && vidaInimigo > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else if (VidaPlayer == 0) {
                        gameOver();
                    }
                    else {
                        deletaInimgo(ultimoColidido);
                        return 0;
                    }
                }
            }
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        if (fase_atual == 1) {
            al_draw_textf(fonteCalculo, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 80, ALLEGRO_ALIGN_CENTRE, "%d - %d", num1, num2);
        }
        else if (fase_atual == 2) {
            al_draw_textf(fonteCalculo, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 80, ALLEGRO_ALIGN_CENTRE, "%d + %d - %d", num1, num2, num3);
        }
        else if (fase_atual == 3) {
            al_draw_textf(fonteCalculo, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 80, ALLEGRO_ALIGN_CENTRE, "%d / %d", num1, num2);
        }
        
        al_draw_bitmap(zero, 350, 500, 0);
        al_draw_bitmap(um, 100, 400, 0);
        al_draw_bitmap(dois, 350, 400, 0);
        al_draw_bitmap(tres, 600, 400, 0);
        al_draw_bitmap(quatro, 100, 300, 0);
        al_draw_bitmap(cinco, 350, 300, 0);
        al_draw_bitmap(seis, 600, 300, 0);
        al_draw_bitmap(sete, 100, 200, 0);
        al_draw_bitmap(oito, 350, 200, 0);
        al_draw_bitmap(nove, 600, 200, 0);

        al_flip_display();
    }

    destroyCalculadora();

    return 0;
}

int destroyGameOver() {
    al_destroy_font(fonte);
    al_destroy_bitmap(voltarMenu);
    al_destroy_event_queue(fila_eventos);
}

int gameOver(void) {
    if (!al_init()) {
        error_msg("Falha ao inicializar a Allegro");
        return -1;
    }
    al_init_font_addon();

    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar add-on allegro_image");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    fonte = al_load_font("arial.ttf", 64, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }

    if (!al_install_mouse()) {
        error_msg("Falha ao inicializar o mouse");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
        error_msg("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(janela);
        return -1;
    }

    voltarMenu = al_load_bitmap("sprites/botao_quit.bmp");
    if (!voltarMenu) {
        error_msg("Falha ao criar botão de saída");
        al_destroy_display(janela);
        return 0;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    int sair = 0;

    while (!sair) {
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 10 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 210 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 10 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 60 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyGameOver();
                    VidaPlayer = 3;
                    start = 0;
                    menu();
                }
            }
        }

        // Preenchemos a tela com a cor ciza
        al_clear_to_color(al_map_rgb(45, 45, 45));

        // Texto tutorial
        al_draw_textf(fonte, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, 250, ALLEGRO_ALIGN_CENTRE, "GAME OVER");

        al_set_target_bitmap(voltarMenu);

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_bitmap(voltarMenu, 10, 10, 0);

        al_flip_display();
    }

    destroyGameOver();

    return 0;
}

int destroyItens() {
    al_destroy_font(fonte);
    al_destroy_bitmap(fundoItem);
    al_destroy_bitmap(itemPocao);
    al_destroy_bitmap(voltarMenu);
    al_destroy_event_queue(fila_eventos);
}

int itens() {
    if (!al_init()) {
        error_msg("Falha ao inicializar a Allegro");
        return -1;
    }
    al_init_font_addon();

    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar add-on allegro_image");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    fonte = al_load_font("arial.ttf", 48, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return 0;
    }

    voltarMenu = al_load_bitmap("sprites/botao_quit.bmp");
    if (!voltarMenu) {
        error_msg("Falha ao criar botão de saída");
        al_destroy_display(janela);
        return 0;
    }

    fundoItem = al_load_bitmap("cenario/fundo_iventario.bmp");
    if (!fundoItem) {
        error_msg("Falha ao carregar fundo");
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }

    itemPocao = al_load_bitmap("sprites/pocao_cura.bmp");
    if (!itemPocao) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }
    al_convert_mask_to_alpha(itemPocao, al_map_rgb(255, 0, 255));

    if (!al_install_mouse()) {
        error_msg("Falha ao inicializar o mouse");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
        error_msg("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(janela);
        return -1;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    int sair = 0;
    int menuAtacar = 0;

    while (!sair) {
        //al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap_region(fundoItem, 0, 0, LARGURA_TELA, ALTURA_TELA, 0, 0, 0);
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 320 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 480 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 200 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 360 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyItens();
                    VidaPlayer = 3;
                    qntPocao--;
                    menuBatalha(fase_atual);
                }

                if (evento.mouse.x >= 10 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 210 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 10 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 60 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyItens();
                    menuBatalha(fase_atual);
                }
            }
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 15, ALLEGRO_ALIGN_CENTRE, "Inventario");
        al_draw_bitmap(voltarMenu, 10, 10, 0);
        if (qntPocao > 0) {
            al_draw_textf(fonte, al_map_rgb(255, 255, 255), 310, 180, ALLEGRO_ALIGN_CENTRE, "%d", qntPocao);
            al_draw_bitmap(itemPocao, 320, 200, 0);
        }
        else {
            al_draw_textf(fonte, al_map_rgb(255, 255, 255), 2000, 2000, ALLEGRO_ALIGN_CENTRE, "%d", qntPocao);
            al_draw_bitmap(itemPocao, 2000, 2000, 0);
        }

        al_flip_display();
    }

    destroyItens();

    return 0;
}

int destroyMenuBatalha() {
    al_destroy_font(fonte);
    al_destroy_font(fonteVida);
    al_destroy_bitmap(fundoBatalha);
    al_destroy_bitmap(calculitoBatalha);
    al_destroy_bitmap(subtracaoBatalha);
    al_destroy_bitmap(atacar);
    al_destroy_bitmap(item);
    al_destroy_bitmap(fugir);
    al_destroy_event_queue(fila_eventos);
}

int menuBatalha(void) {
    if (!al_init()) {
        error_msg("Falha ao inicializar a Allegro");
        return -1;
    }
    al_init_font_addon();

    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar add-on allegro_image");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    fonte = al_load_font("arial.ttf", 48, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return 0;
    }

    fonteVida = al_load_font("arial.ttf", 24, 0);
    if (!fonteVida) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return 0;
    }

    fundoBatalha = al_load_bitmap("Cenario/fundo_combate.bmp");
    if (!fundoBatalha) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    calculitoBatalha = al_load_bitmap("sprites/Calculito_costas.bmp");
    if (!calculitoBatalha) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }
    al_convert_mask_to_alpha(calculitoBatalha, al_map_rgb(255, 0, 255));

    subtracaoBatalha = al_load_bitmap("sprites/Subtracao_frente_final.bmp");
    if (!subtracaoBatalha) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }
    al_convert_mask_to_alpha(subtracaoBatalha, al_map_rgb(255, 0, 255));

    atacar = al_load_bitmap("sprites/botao_ataque.bmp");
    if (!atacar) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    item = al_load_bitmap("sprites/botao_item.bmp");
    if (!item) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    fugir = al_load_bitmap("sprites/botao_run.bmp");
    if (!fugir) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    if (!al_install_mouse()) {
        error_msg("Falha ao inicializar o mouse");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
        error_msg("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(janela);
        return -1;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    int sair = 0;
    int menuAtacar = 0;
    int menuItem = 0;
    int menuFugir = 0;

    while (!sair) {
        al_draw_bitmap(fundoBatalha, 0, 0, 0);
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            //Verifica se o mouse esta em algum dos botões
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 50 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 250 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 500 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 565 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyMenuBatalha();
                    calculadora();
                }

                if (evento.mouse.x >= 300 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 500 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 500 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 565 * (res_y_comp / (float)ALTURA_TELA)) {
                    menuItem = 1;
                    destroyMenuBatalha();
                    itens();
                }

                if (evento.mouse.x >= 550 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 750 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 500 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 565 * (res_y_comp / (float)ALTURA_TELA)) {
                    player.x = 300; player.y = 300;
                    destroyMenuBatalha();
                    //if (fase_atual == 1) {
                    //    //fase_atual = 1;
                    //    jogo();
                    //}
                    //else if (fase_atual == 2) {
                    //    //fase_atual = 2;
                    //    jogo();
                    //} 
                    jogo();
                }
            }
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_textf(fonteVida, al_map_rgb(255, 255, 255), 120, 30, ALLEGRO_ALIGN_CENTRE, "Player: %d", VidaPlayer);
        if (fase_atual == 1) {
            al_draw_textf(fonteVida, al_map_rgb(255, 255, 255), 700, 30, ALLEGRO_ALIGN_RIGHT, "SubtraCao: %d", vidaInimigo);
        }else if (fase_atual == 2) {
            al_draw_textf(fonteVida, al_map_rgb(255, 255, 255), 700, 30, ALLEGRO_ALIGN_RIGHT, "Phatasoma: %d", vidaInimigo);
        }
        else if (fase_atual == 3) {
            al_draw_textf(fonteVida, al_map_rgb(255, 255, 255), 700, 30, ALLEGRO_ALIGN_RIGHT, "Final Boss: %d", vidaInimigo);
        }
        
        al_draw_bitmap(calculitoBatalha, 100, 290, 0);
        if (inimigoAtual == 1) {
            al_draw_bitmap(subtracaoBatalha, 460, 80, 0);
        }/*else if(inimigoAtual == 2) {
            al_draw_bitmap(NULL, 460, 80, 0);
        }
        else if (inimigoAtual == 3) {
            al_draw_bitmap(boss, 460, 80, 0);
        }*/

        al_draw_bitmap(atacar, 50, 500, 0);
        al_draw_bitmap(item, 300, 500, 0);
        al_draw_bitmap(fugir, 550, 500, 0);

        al_flip_display();
    }

    al_destroy_bitmap(atacar);
    al_destroy_bitmap(item);
    al_destroy_bitmap(fugir);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}

int destroyJogo(int fase) {

    if (fase == 1) {
        al_destroy_bitmap(inimigo_subtracao_costas);
        al_destroy_bitmap(inimigo_subtracao_direita);
        al_destroy_bitmap(inimigo_subtracao_esquerda);
        al_destroy_bitmap(inimigo_subtracao);
        al_destroy_bitmap(fundo);
    }
    else if (fase == 2) {
        al_destroy_bitmap(phantasoma);
        al_destroy_bitmap(caverna);
    }
    else if (fase == 3) {
        al_destroy_bitmap(boss);
        al_destroy_bitmap(ultimoFundo);
    }

    al_destroy_font(fonte);
    al_destroy_bitmap(direita);
    al_destroy_bitmap(esquerda);
    al_destroy_bitmap(cima);
    al_destroy_bitmap(baixo);
    al_destroy_bitmap(parado);
    al_destroy_timer(timer);
    al_destroy_audio_stream(musica);
    al_destroy_event_queue(fila_eventos);
}

int jogo() {
    int i;
    int tecla = 0;
    //define quando a tela sera atualizada
    int desenha = 1;
    //quando o loop principal deve encerrar
    int sair = 0;
    //largura e altura de cada sprite dentro da folha
    int altura_sprite = 170, largura_sprite = 160;
    int altura_spriteI = 170, largura_spriteI = 160;
    //quantos sprites tem em cada linha da folha, e a atualmente mostrada
    int colunas_folha = 3, coluna_atual = 0;
    int colunas_folhaI = 3, coluna_atualI = 0;
    //quantos sprites tem em cada coluna da folha, e a atualmente mostrada
    int linha_atual = 0, linhas_folha = 2;
    int linha_atualI = 0, linhas_folhaI = 2;
    //quantos frames devem se passar para atualizar para o proximo sprite
    int frames_sprite = 6, cont_frames = 0;
    int frames_spriteI = 6, cont_framesI = 0;
    //posicao X Y da janela em que sera mostrado o sprite
    int regiao_x_folha = 0, regiao_y_folha = 0;
    int regiao_x_folhaI = 0, regiao_y_folhaI = 0;

    if (fase_atual == 1) {
        vidaInimigo = 3;
        if (start == 0) {
            //inicialização do player
            player.x = 300; player.y = 400; player.direcao_x = 3; player.direcao_y = 3;

            //inicialização do inimigo 1
            inimigosFase1[0].x = 600; inimigosFase1[0].y = 200;
            inimigosFase1[0].lim_y_1 = 200; inimigosFase1[0].lim_y_2 = 400;
            inimigosFase1[0].direcao_x = 0; inimigosFase1[0].direcao_y = 3;

            //inicializacao do inimigo 2
            inimigosFase1[1].x = 200; inimigosFase1[1].y = 100;
            inimigosFase1[1].lim_x_1 = 200; inimigosFase1[1].lim_x_2 = 400;
            inimigosFase1[1].direcao_x = 3; inimigosFase1[1].direcao_y = 0;

            //inicialização da saida
            saidaFase01.x = 400; saidaFase01.y = -100;

            start = 1;
        }
    }
    else if(fase_atual == 2){
        vidaInimigo = 5;

        if (start_dois == 0) {
            //inicialização do player
            player.x = 300; player.y = 400; player.direcao_x = 3; player.direcao_y = 3;

            //inicialização do inimigo 1
            inimigosFase2[0].x = 600; inimigosFase2[0].y = 200;
            inimigosFase2[0].lim_y_1 = 200; inimigosFase2[0].lim_y_2 = 400;
            inimigosFase2[0].direcao_x = 0; inimigosFase2[0].direcao_y = 3;

            //inicializacao do inimigo 2
            inimigosFase2[1].x = 200; inimigosFase2[1].y = 100;
            inimigosFase2[1].lim_x_1 = 200; inimigosFase2[1].lim_x_2 = 400;
            inimigosFase2[1].direcao_x = 3; inimigosFase2[1].direcao_y = 0;

            saidaFase02.x = 400; saidaFase02.y = -100;
            //inicialização da saida
            voltaFase01.x = 300; voltaFase01.y = 590;

            start_dois = 1;
        }
    }
    else if (fase_atual == 3) {
        vidaInimigo = 3;

        if (start_tres == 0) {
            //inicialização do player
            player.x = 300; player.y = 400; player.direcao_x = 3; player.direcao_y = 3;

            //inicialização do inimigo 1
            inimigosFase3[0].x = 600; inimigosFase3[0].y = 200;
            inimigosFase3[0].lim_y_1 = 200; inimigosFase3[0].lim_y_2 = 400;
            inimigosFase3[0].direcao_x = 0; inimigosFase3[0].direcao_y = 3;

            //inicializacao do inimigo 2
            inimigosFase3[1].x = 200; inimigosFase3[1].y = 100;
            inimigosFase3[1].lim_x_1 = 200; inimigosFase3[1].lim_x_2 = 400;
            inimigosFase3[1].direcao_x = 3; inimigosFase3[1].direcao_y = 0;

            saidaFase02.x = 400; saidaFase02.y = -100;
            //inicialização da saida
            voltaFase02.x = 300; voltaFase02.y = 590;

            start_tres = 1;
        }
    }
    
    if (!inicializar(fase_atual)) {
        return -1;
    }

    while (!sair) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);
        
        if (fase_atual == 1) {
            //fila de inimigos
            for (i = 0; i < (int)(sizeof(inimigosFase1) / sizeof(inimigosFase1[0])); i++) {
                //quando colidi com o inimigo
                if (colidiu(player, inimigosFase1[i])) {
                    ultimoColidido = &inimigosFase1[i];
                    inimigoAtual = &inimigosFase1[i];
                    if (inimigoAtual == &inimigosFase1[i]) {
                        inimigoAtual = 1;
                    }
                    destroyJogo(fase_atual);
                    menuBatalha();
                    return 0;
                }
                //quando não colidi o inimigo
                else {
                    //printf("Nao colidiu \n");
                }
            }

            //if para quando o player matar os dois inimigos e passar pela passagem trocar de fase
            if (colidiu(player, saidaFase01) && inimigosFase1[0].x == 2000 && inimigosFase1[1].x == 2000) {
                destroyJogo(fase_atual);
                player.x = 300; player.y = 400;
                fase_atual = 2;
                jogo();
            }


            //movimentacao dos inimigos
            if (evento.type == ALLEGRO_EVENT_TIMER) {
                for (i = 0; i < (int)(sizeof(inimigosFase1) / sizeof(inimigosFase1[0])); i++) {

                    inimigosFase1[i].x += inimigosFase1[i].direcao_x;
                    inimigosFase1[i].y += inimigosFase1[i].direcao_y;

                    //se passou das bordas, inverte a direcao
                    if (inimigosFase1[i].x <= inimigosFase1[i].lim_x_1 ||
                        inimigosFase1[i].x >= inimigosFase1[i].lim_x_2) {
                        inimigosFase1[i].direcao_x *= -1;
                    }

                    if (inimigosFase1[i].y <= inimigosFase1[i].lim_y_1 ||
                        inimigosFase1[i].y >= inimigosFase1[i].lim_y_2) {
                        inimigosFase1[i].direcao_y *= -1;
                    }

                }
                desenha = 1;
            }
        }
        else if (fase_atual == 2) {
            //fila de inimigos
            for (i = 0; i < (int)(sizeof(inimigosFase2) / sizeof(inimigosFase2[0])); i++) {
                //quando colidi com o inimigo
                if (colidiu(player, inimigosFase2[i])) {
                    ultimoColidido = &inimigosFase2[i];
                    inimigoAtual = 2;
                    destroyJogo(fase_atual);
                    menuBatalha();
                    return 0;
                }
                //quando não colidi o inimigo
                else {
                    //printf("Nao colidiu \n");
                }
            }

            //if para quando o player matar os dois inimigos e passar pela passagem trocar de fase
            if (colidiu(player, voltaFase01)) {
                destroyJogo(fase_atual);
                player.x = 350; player.y = 100;
                fase_atual = 1;
                jogo();
                //printf("Passou");
                return 0;
            }
            //if para quando o player matar os dois inimigos e passar pela passagem trocar de fase
            if (colidiu(player, saidaFase02) && inimigosFase1[0].x == 2000 && inimigosFase1[1].x == 2000) {
                destroyJogo(fase_atual);
                player.x = 300; player.y = 400;
                fase_atual = 3;
                jogo();
            }



            //movimentacao dos inimigos
            if (evento.type == ALLEGRO_EVENT_TIMER) {
                for (i = 0; i < (int)(sizeof(inimigosFase2) / sizeof(inimigosFase2[0])); i++) {

                    inimigosFase2[i].x += inimigosFase2[i].direcao_x;
                    inimigosFase2[i].y += inimigosFase2[i].direcao_y;

                    //se passou das bordas, inverte a direcao
                    if (inimigosFase2[i].x <= inimigosFase2[i].lim_x_1 ||
                        inimigosFase2[i].x >= inimigosFase2[i].lim_x_2) {
                        inimigosFase2[i].direcao_x *= -1;
                    }

                    if (inimigosFase2[i].y <= inimigosFase2[i].lim_y_1 ||
                        inimigosFase2[i].y >= inimigosFase2[i].lim_y_2) {
                        inimigosFase2[i].direcao_y *= -1;
                    }

                }
                desenha = 1;
            }
        }
        else if (fase_atual == 3) {
            for (i = 0; i < (int)(sizeof(inimigosFase3) / sizeof(inimigosFase3[0])); i++) {
                //quando colidi com o inimigo
                if (colidiu(player, inimigosFase3[i])) {
                    ultimoColidido = &inimigosFase3[i];
                    inimigoAtual = &inimigosFase3[i];
                    if (inimigoAtual == &inimigosFase3[i]) {
                        inimigoAtual = 3;
                    }
                    destroyJogo(fase_atual);
                    menuBatalha();
                    return 0;
                }
                //quando não colidi o inimigo
                else {
                    //printf("Nao colidiu \n");
                }
            }

            //if para quando o player matar os dois inimigos e passar pela passagem trocar de fase
            if (colidiu(player, voltaFase02)) {
                destroyJogo(fase_atual);
                player.x = 350; player.y = 100;
                fase_atual = 2;
                jogo();
                //printf("Passou");
                return 0;
            }

            //movimentacao dos inimigos
            if (evento.type == ALLEGRO_EVENT_TIMER) {
                for (i = 0; i < (int)(sizeof(inimigosFase3) / sizeof(inimigosFase3[0])); i++) {

                    inimigosFase3[i].x += inimigosFase3[i].direcao_x;
                    inimigosFase3[i].y += inimigosFase3[i].direcao_y;

                    //se passou das bordas, inverte a direcao
                    if (inimigosFase3[i].x <= inimigosFase3[i].lim_x_1 ||
                        inimigosFase3[i].x >= inimigosFase3[i].lim_x_2) {
                        inimigosFase3[i].direcao_x *= -1;
                    }

                    if (inimigosFase3[i].y <= inimigosFase3[i].lim_y_1 ||
                        inimigosFase3[i].y >= inimigosFase3[i].lim_y_2) {
                        inimigosFase3[i].direcao_y *= -1;
                    }

                }
                desenha = 1;
            }
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            //a cada disparo do timer, incrementa cont_frames
            cont_frames++;

            //se alcancou a quantidade de frames que precisa passar para mudar para o proximo sprite
            if (cont_frames >= frames_sprite) {
                //reseta cont_frames
                cont_frames = 0;
                //incrementa a coluna atual, para mostrar o proximo sprite
                coluna_atual++;
                //se coluna atual passou da ultima coluna
                if (coluna_atual >= colunas_folha) {
                    //volta pra coluna inicial
                    coluna_atual = 0;
                    //incrementa a linha, se passar da ultima, volta pra primeira
                    linha_atual = (linha_atual + 1) % linhas_folha;
                    //calcula a posicao Y da folha que sera mostrada
                    regiao_y_folha = linha_atual * altura_sprite;
                }
                //calcula a regiao X da folha que sera mostrada
                regiao_x_folha = coluna_atual * largura_sprite;
            }
        }

        //Loop de movimentacao do inimigo
        if (evento.type == ALLEGRO_EVENT_TIMER) {
            //a cada disparo do timer, incrementa cont_frames
            cont_framesI++;
            //se alcancou a quantidade de frames que precisa passar para mudar para o proximo sprite
            if (cont_framesI >= frames_spriteI) {
                //reseta cont_frames
                cont_framesI = 0;
                //incrementa a coluna atual, para mostrar o proximo sprite
                coluna_atualI++;
                //se coluna atual passou da ultima coluna
                if (coluna_atualI >= colunas_folhaI) {
                    //volta pra coluna inicial
                    coluna_atualI = 0;
                    //incrementa a linha, se passar da ultima, volta pra primeira
                    linha_atualI = (linha_atualI + 1) % linhas_folhaI;
                    //calcula a posicao Y da folha que sera mostrada
                    regiao_y_folhaI = linha_atualI * altura_spriteI;
                }
                //calcula a regiao X da folha que sera mostrada
                regiao_x_folhaI = coluna_atualI * largura_spriteI;
            }
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (evento.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                tecla = 1;
                break;
            case ALLEGRO_KEY_DOWN:
                tecla = 2;
                break;
            case ALLEGRO_KEY_LEFT:
                tecla = 3;
                break;
            case ALLEGRO_KEY_RIGHT:
                tecla = 4;
                break;
                /*case ALLEGRO_KEY_D:
                    tecla = 5;
                    break;*/
            }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            tecla = 5;
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = 1;
        }

        // Estrutura que realiza a movimentação do Personagem
        if (tecla) {
            al_clear_to_color(al_map_rgb(255, 255, 255));
            switch (tecla) {
            case 1:
                //colisão de cima
                if (player.y >= 0) {
                    player.y -= player.direcao_y;
                    break;
                }
                break;
            case 2:
                //colisão de baixo
                if (player.y <= ALTURA_TELA - 180) {
                    player.y += player.direcao_y;
                    break;
                }
                break;
            case 3:
                //colisão da esquerda
                if (player.x >= 70) {
                    player.x -= player.direcao_x;
                    break;
                }
                break;
            case 4:
                //colisão da direita
                if (player.x <= LARGURA_TELA - 130) {
                    player.x += player.direcao_x;
                    break;
                }
                break;
            }
            desenha = 1;
        }

        //Desenha a nova posição do Personagem na tela
        if (desenha && al_is_event_queue_empty(fila_eventos)) {

            if (fase_atual == 1) {
                al_draw_bitmap_region(fundo, 0, 0, LARGURA_TELA, ALTURA_TELA, 0, 0, 0);
            }
            else if (fase_atual == 2) {
                al_draw_bitmap_region(caverna, 0, 0, LARGURA_TELA, ALTURA_TELA, 0, 0, 0);
            }
            else if (fase_atual == 3) {
                al_draw_bitmap_region(ultimoFundo, 0, 0, LARGURA_TELA, ALTURA_TELA, 0, 0, 0);
            }

            if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 4) {
                al_draw_bitmap_region(direita, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
            }
            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 3) {
                al_draw_bitmap_region(esquerda, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
            }
            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 2) {
                al_draw_bitmap_region(baixo, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
            }
            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 1) {
                al_draw_bitmap_region(cima, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
            }
            else {
                al_draw_bitmap_region(parado, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
            }

            //desenha os inimigos
            if (fase_atual == 1) {
                for (i = 0; i < (int)(sizeof(inimigosFase1) / sizeof(inimigosFase1[0])); i++) {

                    if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase1[i].direcao_x > 0) {
                        al_draw_bitmap_region(inimigo_subtracao_direita, regiao_x_folha,
                            regiao_y_folha, largura_sprite, altura_sprite, inimigosFase1[i].x, inimigosFase1[i].y, 0);
                    }
                    else if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase1[i].direcao_x < 0) {
                        al_draw_bitmap_region(inimigo_subtracao_esquerda, regiao_x_folha,
                            regiao_y_folha, largura_sprite, altura_sprite, inimigosFase1[i].x, inimigosFase1[i].y, 0);
                    }
                    else if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase1[i].direcao_y > 0) {
                        al_draw_bitmap_region(inimigo_subtracao, regiao_x_folha,
                            regiao_y_folha, largura_sprite, altura_sprite, inimigosFase1[i].x, inimigosFase1[i].y, 0);
                    }
                    else if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase1[i].direcao_y < 0) {
                        al_draw_bitmap_region(inimigo_subtracao_costas, regiao_x_folha,
                            regiao_y_folha, largura_sprite, altura_sprite, inimigosFase1[i].x, inimigosFase1[i].y, 0);
                    }

                }
                al_flip_display();
                desenha = 0;
            }
            else if (fase_atual == 2) {
                for (i = 0; i < (int)(sizeof(inimigosFase2) / sizeof(inimigosFase2[0])); i++) {

                    if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase2[i].direcao_x > 0 || inimigosFase2[i].direcao_x < 0) {
                        al_draw_bitmap_region(phantasoma, regiao_x_folha,
                            regiao_y_folha, largura_sprite, altura_sprite, inimigosFase2[i].x, inimigosFase2[i].y, 0);
                    }
                    else if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase2[i].direcao_y > 0 || inimigosFase2[i].direcao_y < 0) {
                        al_draw_bitmap_region(phantasoma, regiao_x_folha,
                            regiao_y_folha, largura_sprite, altura_sprite, inimigosFase2[i].x, inimigosFase2[i].y, 0);
                    }
                }
                al_flip_display();
                desenha = 0;
            }
            else if (fase_atual == 3) {
                for (i = 0; i < (int)(sizeof(inimigosFase3) / sizeof(inimigosFase3[0])); i++) {

                    if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase3[i].direcao_x > 0 || inimigosFase3[i].direcao_x < 0) {
                        al_draw_bitmap_region(boss, regiao_x_folha,
                            regiao_y_folha, largura_sprite, altura_sprite, inimigosFase3[i].x, inimigosFase3[i].y, 0);
                    }
                    else if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase3[i].direcao_y > 0 || inimigosFase3[i].direcao_y < 0) {
                        al_draw_bitmap_region(boss, regiao_x_folha,
                            regiao_y_folha, largura_sprite, altura_sprite, inimigosFase3[i].x, inimigosFase3[i].y, 0);
                    }
                }
                al_flip_display();
                desenha = 0;
            }
        }
    }

    destroyJogo(fase_atual);
}

int destroyTutoDes() {
    al_destroy_font(fonte);
    al_destroy_font(fonteTitulo);
    al_destroy_bitmap(voltarMenu);
    al_destroy_event_queue(fila_eventos);
}

int tutorial() {
    if (!al_init()) {
        error_msg("Falha ao inicializar a Allegro");
        return -1;
    }
    al_init_font_addon();

    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar add-on allegro_image");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    fonte = al_load_font("arial.ttf", 32, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }

    fonteTitulo = al_load_font("arial.ttf", 42, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }

    if (!al_install_mouse()) {
        error_msg("Falha ao inicializar o mouse");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
        error_msg("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(janela);
        return -1;
    }

    voltarMenu = al_load_bitmap("sprites/botao_quit.bmp");
    if (!voltarMenu) {
        error_msg("Falha ao criar botão de saída");
        al_destroy_display(janela);
        return 0;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    /*al_register_event_source(fila_eventos, al_get_display_event_source(janela));*/

    int sair = 0;

    while (!sair) {
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 10 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 210 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 10 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 60 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyTutoDes();
                    menu();
                }
            }
        }

        // Preenchemos a tela com a cor ciza
        al_clear_to_color(al_map_rgb(80, 80, 80));

        // Texto tutorial
        al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 100, ALLEGRO_ALIGN_CENTRE, "TUTORIAL");
        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 200, ALLEGRO_ALIGN_CENTRE, "MOVIMENTACAO: W.A.S.D");
        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 250,
            ALLEGRO_ALIGN_CENTRE, "INTERACAO NA TELA DE COMBATE: A (Direita)");
        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 300, ALLEGRO_ALIGN_CENTRE, "B(Esquerda) - ENTER(Selecionar)");

        al_set_target_bitmap(voltarMenu);

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_bitmap(voltarMenu, 10, 10, 0);

        al_flip_display();
    }

    return 0;
}

int desenvolvedores() {
    if (!al_init()) {
        error_msg("Falha ao inicializar a Allegro");
        return -1;
    }

    al_init_font_addon();

    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar add-on allegro_image");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    fonte = al_load_font("arial.ttf", 32, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }

    fonteTitulo = al_load_font("arial.ttf", 42, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }

    if (!al_install_mouse()) {
        error_msg("Falha ao inicializar o mouse");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
        error_msg("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(janela);
        return -1;
    }

    voltarMenu = al_load_bitmap("sprites/botao_quit.bmp");
    if (!voltarMenu) {
        error_msg("Falha ao criar botão de saída");
        al_destroy_display(janela);
        return 0;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_mouse_event_source());

    int sair = 0;

    while (!sair) {
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 10 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 210 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 10 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 60 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyTutoDes();
                    menu();
                }
            }
        }

        // Preenchemos a tela com a cor ciza
        al_clear_to_color(al_map_rgb(80, 80, 80));

        // Texto tutorial
        al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 100, ALLEGRO_ALIGN_CENTRE, "DESENVOLVEDORES");
        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 200, ALLEGRO_ALIGN_CENTRE, "Andre Tomio Nakayama");
        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 250, ALLEGRO_ALIGN_CENTRE, "Carlos Eduardo Gadelha Silva");
        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 300, ALLEGRO_ALIGN_CENTRE, "Rafael Ribas Albuquerque");

        al_set_target_bitmap(voltarMenu);

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_bitmap(voltarMenu, 10, 10, 0);

        al_flip_display();
    }

    return 0;
}

int destroyMenu() {
    al_destroy_bitmap(botao_sair);
    al_destroy_bitmap(jogar);
    al_destroy_bitmap(instrucoes);
    al_destroy_bitmap(menuDesenvolvedores);
    al_destroy_bitmap(imagem);
    al_destroy_event_queue(fila_eventos);
}

int menu() {
    if (!al_init()) {
        error_msg("Falha ao inicializar a Allegro");
        return -1;
    }

    al_init_font_addon();

    if (!al_init_image_addon()) {
        error_msg("Falha ao inicializar add-on allegro_image");
        return -1;
    }

    if (!al_init_ttf_addon()) {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    if (comeco == 0) {
        al_get_monitor_info(0, &info);
        res_x_comp = info.x2 - info.x1;
        res_y_comp = info.y2 - info.y1;

        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
        janela = al_create_display(res_x_comp, res_y_comp);

        /*janela = al_create_display(LARGURA_TELA, ALTURA_TELA);*/
        if (!janela) {
            error_msg("Falha ao criar janela");
            return -1;
        }
        al_set_window_title(janela, "Menu");

        float red_x = res_x_comp / (float)LARGURA_TELA;
        float red_y = res_y_comp / (float)ALTURA_TELA;

        ALLEGRO_TRANSFORM transformar;
        al_identity_transform(&transformar);
        al_scale_transform(&transformar, red_x, red_y);
        al_use_transform(&transformar);

        comeco = 1;
    }

    imagem = al_load_bitmap("Cenario/Menu.bmp");
    if (!imagem) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_install_mouse()) {
        error_msg("Falha ao inicializar o mouse");
        al_destroy_display(janela);
        return -1;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
        error_msg("Falha ao atribuir ponteiro do mouse");
        al_destroy_display(janela);
        return -1;
    }

    jogar = al_load_bitmap("sprites/botao_jogar.bmp");
    if (!jogar) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    instrucoes = al_load_bitmap("sprites/botao_tutorial.bmp");
    if (!instrucoes) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    menuDesenvolvedores = al_load_bitmap("sprites/botao_credito.bmp");
    if (!menuDesenvolvedores) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    botao_sair = al_load_bitmap("sprites/botao_quit.bmp");
    if (!botao_sair) {
        error_msg("Falha ao criar botão de saída");
        al_destroy_bitmap(jogar);
        al_destroy_bitmap(instrucoes);
        al_destroy_bitmap(menuDesenvolvedores);
        al_destroy_display(janela);
        return 0;
    }

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    int sair = 0;

    while (!sair) {
        al_draw_bitmap(imagem, 0, 0, 0);
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 300 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 500 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 150 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 200 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyMenu();
                    fase_atual = 1;
                    jogo();
                }

                if (evento.mouse.x >= 300 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 500 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 230 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 280 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyMenu();
                    tutorial();
                }

                if (evento.mouse.x >= 300 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 500 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 310 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 360 * (res_y_comp / (float)ALTURA_TELA)) {
                    destroyMenu();
                    desenvolvedores();
                }

                if (evento.mouse.x >= 300 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.x <= 500 * (res_x_comp / (float)LARGURA_TELA) &&
                    evento.mouse.y >= 390 * (res_y_comp / (float)ALTURA_TELA) &&
                    evento.mouse.y <= 440 * (res_y_comp / (float)ALTURA_TELA)) {
                    sair = 1;
                }
            }
        }

        al_set_target_bitmap(botao_sair);

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_bitmap(jogar, 300, 150, 0);
        al_draw_bitmap(instrucoes, 300, 230, 0);
        al_draw_bitmap(menuDesenvolvedores, 300, 310, 0);
        al_draw_bitmap(botao_sair, 300, 390, 0);

        al_flip_display();
    }

    destroyMenu();

    return 0;
}

int main(void) {
    menu();
    return 0;
}