// Inclui o arquivo de cabeçalho da biblioteca Allegro 5
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//fps = frames per second = atualizacoes de tela por segundo
#define FPS 60.0
#define LARGURA_TELA 800
#define ALTURA_TELA 600

typedef struct { int x, y, lim_x_1, lim_y_1, lim_x_2, lim_y_2, direcao_x, direcao_y; } caixa;
int VidaPlayer = 3, vidaInimigo = 3, start =0;
caixa* ultimoColidido = NULL;
//criação das variaveis dos objetos
caixa player, inimigosFase1[2];

ALLEGRO_DISPLAY* janela = NULL;
ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
ALLEGRO_FONT* fonte = NULL;
ALLEGRO_FONT* atacar = NULL;
ALLEGRO_FONT* item = NULL;
ALLEGRO_FONT* fugir = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_BITMAP* quadrado = NULL;
ALLEGRO_BITMAP* inimigo_subtracao = NULL;
ALLEGRO_BITMAP* direita = NULL;
ALLEGRO_BITMAP* esquerda = NULL;
ALLEGRO_BITMAP* cima = NULL;
ALLEGRO_BITMAP* baixo = NULL;
ALLEGRO_BITMAP* parado = NULL;
ALLEGRO_BITMAP* fundo = NULL;
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
ALLEGRO_FONT* confirmar = NULL;
ALLEGRO_FONT* backspace = NULL;
ALLEGRO_FONT* fonteCalculo = NULL;
ALLEGRO_FONT* calculitoBatalha = NULL;
ALLEGRO_FONT* subtracaoBatalha = NULL;
//ALLEGRO_FONT* vidaPlayer = 3;
ALLEGRO_FONT* fonteVida = NULL;


void error_msg(char* text) {
    al_show_native_message_box(janela, "ERRO",
        "Ocorreu o seguinte erro e o programa sera finalizado:",
        text, NULL, ALLEGRO_MESSAGEBOX_ERROR);
}

int inicializar() {
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

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela) {
        error_msg("Falha ao criar janela");
        al_destroy_timer(timer);
        return 0;
    }

    al_set_window_title(janela, "Utilizando o Teclado");

    fonte = al_load_font("arial.ttf", 48, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return 0;
    }

    //carrega a folha de sprites na variavel
    inimigo_subtracao = al_load_bitmap("sprites/Subtracao_andando_baixo.bmp");
    if (!inimigo_subtracao) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    //usa a cor rosa como transparencia
    al_convert_mask_to_alpha(inimigo_subtracao, al_map_rgb(255, 0, 255));


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
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    al_clear_to_color(al_map_rgb(255, 0, 0));
    al_set_target_bitmap(al_get_backbuffer(janela));
    fila_eventos, al_get_timer_event_source(timer);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    al_start_timer(timer);

    return 1;
}

//função que calcula a colisão
int colidiu(caixa box1, caixa box2) {
    if (box1.x + 80 > box2.x && box1.x < box2.x + 80 && box1.y + 170 > box2.y && box1.y < box2.y + 160) {
        return 1;
    }
    else {
        return 0;
    }
}

int deletaInimgo(caixa *inimigo) {
    inimigo->x = 2000;
    inimigo->y = 2000;

    printf("Chegou no deletaInimigos");
    al_destroy_display(janela);
    jogo();

    return 0;
}

//função que sorteia o primeiro numero
int * sorteioNumeros() {
    //variaveis para sorteio
    static int vetor[2];
    //sorteio dos numeros
    srand(time(0));
    for (int i = 0; i < 2; i++) {
        vetor[i] = rand() %10;
    }
    int aux;

    if (vetor[0] < vetor[1]) {
        aux = vetor[0];
        vetor[0] = vetor[1];
        vetor[1] = aux;
    }

    return vetor;
}

int resultadoCalculo(int num1, int num2) {
    int resultado = num1 - num2;
    printf("\n%d", resultado);
    return resultado;
}

void acertou() {
    char tcaixa[50] = "OLA";
    char titulo[100] = "PARABENS";
    char texto[200] = "VOCE ACETOU!!";
    //mostra a caixa de texto
    int r = al_show_native_message_box(NULL, tcaixa, titulo, texto, NULL, ALLEGRO_MESSAGEBOX_QUESTION);
    
    return vidaInimigo-= 1;
}

int errou() {
    char tcaixa[50] = "OLA";
    char titulo[100] = "ERROU...";
    char texto[200] = "BOA SORTE NA PROXIMA!!";
    //mostra a caixa de texto
    int r = al_show_native_message_box(fonte, tcaixa, titulo, texto, NULL, ALLEGRO_MESSAGEBOX_QUESTION);

    return VidaPlayer -= 1;

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

    confirmar = al_load_bitmap("sprites/botao_calculadora_confirmar.bmp");
    if (!confirmar) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    backspace = al_load_bitmap("sprites/botao_calculadora_voltar.bmp");
    if (!backspace) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(janela);
        return 0;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela) {
        error_msg("Falha ao criar janela");
        return -1;
    }
    al_set_window_title(janela, "Batalha");

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
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

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
    int num1, num2;
    numeros = sorteioNumeros();
    num1 = numeros[0];
    num2 = numeros[1];
    int resultado = resultadoCalculo(num1, num2);

    while (!menuConfirmar) {
        al_clear_to_color(al_map_rgb(45, 45, 45));
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            if (vidaInimigo == 0) {
                deletaInimgo(ultimoColidido);
                return 0;
            }
            //Verifica se o mouse esta em algmenuUm dos botões
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 350 &&
                    evento.mouse.x <= 450 &&
                    evento.mouse.y >= 500 &&
                    evento.mouse.y <= 550) {
                   if (resultado == 0) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    } 

                }

                if (evento.mouse.x >= 100 &&
                    evento.mouse.x <= 200 &&
                    evento.mouse.y >= 400 &&
                    evento.mouse.y <= 450) {
                    if (resultado == 1) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                if (evento.mouse.x >= 350 &&
                    evento.mouse.x <= 450 &&
                    evento.mouse.y >= 400 &&
                    evento.mouse.y <= 450) {
                    if (resultado == 2) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                if (evento.mouse.x >= 600 &&
                    evento.mouse.x <= 700 &&
                    evento.mouse.y >= 400 &&
                    evento.mouse.y <= 450) {
                    if (resultado == 3) {
                        acertou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                if (evento.mouse.x >= 100 &&
                    evento.mouse.x <= 200 &&
                    evento.mouse.y >= 300 &&
                    evento.mouse.y <= 350) {
                    if (resultado == 4) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                if (evento.mouse.x >= 350 &&
                    evento.mouse.x <= 450 &&
                    evento.mouse.y >= 300 &&
                    evento.mouse.y <= 350) {
                    if (resultado == 5) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                if (evento.mouse.x >= 600 &&
                    evento.mouse.x <= 700 &&
                    evento.mouse.y >= 300 &&
                    evento.mouse.y <= 350) {
                    if (resultado == 6) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                if (evento.mouse.x >= 100 &&
                    evento.mouse.x <= 200 &&
                    evento.mouse.y >= 200 &&
                    evento.mouse.y <= 250) {
                    if (resultado == 7) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                if (evento.mouse.x >= 350 &&
                    evento.mouse.x <= 450 &&
                    evento.mouse.y >= 200 &&
                    evento.mouse.y <= 250) {
                    if (resultado == 8) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                if (evento.mouse.x >= 600 &&
                    evento.mouse.x <= 700 &&
                    evento.mouse.y >= 200 &&
                    evento.mouse.y <= 250) {
                    if (resultado == 9) {
                        acertou();
                    }
                    else {
                        errou();
                    }
                    
                    al_destroy_display(janela);

                    if (VidaPlayer > 0) {
                        menuBatalha();
                        return 0;
                    }
                    else {
                        gameOver();
                        return 0;
                    }
                }

                /*if (evento.mouse.x >= 550 &&
                    evento.mouse.x <= 750 &&
                    evento.mouse.y >= 500 &&
                    evento.mouse.y <= 550) {
                    menuConfirmar = 1;
                    al_destroy_display(janela);
                    menuBatalha();
                }
                else {
                    errou();
                }*/

                /*if (evento.mouse.x >= 50 &&
                    evento.mouse.x <= 250 &&
                    evento.mouse.y >= 500 &&
                    evento.mouse.y <= 550) {
                    menuBackspace = 1;
                }
                else {
                    errou();
                }*/
            }
        }

        //Muda o cor do botão se o mouse estiver em cima
        /*al_set_target_bitmap(confirmar);
        if (!menuConfirmar) {
            al_clear_to_color(al_map_rgb(255, 255, 255));
        }
        else {
            al_clear_to_color(al_map_rgb(255, 255, 0));
        }

        al_set_target_bitmap(backspace);
        if (!menuBackspace) {
            al_clear_to_color(al_map_rgb(255, 255, 255));
        }
        else {
            al_clear_to_color(al_map_rgb(255, 255, 0));
        }*/

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_textf(fonteCalculo, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 80, ALLEGRO_ALIGN_CENTRE, "%d - %d", num1, num2);
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
        //al_draw_bitmap(backspace, 50, 500, 0);
        //al_draw_bitmap(confirmar, 550, 500, 0);

        al_flip_display();
    }

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
    al_destroy_bitmap(confirmar);
    al_destroy_bitmap(backspace);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}

int gameOver(void) {
    ALLEGRO_DISPLAY* janela = NULL;
    ALLEGRO_FONT* fonte = NULL;

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

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela) {
        error_msg("Falha ao criar janela");
        return -1;
    }
    al_set_window_title(janela, "TUTORIAL");

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
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    int sair = 0;

    while (!sair) {
        al_draw_bitmap(imagem, 0, 0, 0);
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 10 &&
                    evento.mouse.x <= 210 &&
                    evento.mouse.y >= 10 &&
                    evento.mouse.y <= 60) {
                    al_destroy_display(janela);
                    sair = 1;
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

    return 0;
}

//Função para a batalha
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

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela) {
        error_msg("Falha ao criar janela");
        return -1;
    }
    al_set_window_title(janela, "Batalha");

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
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));


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
                if (evento.mouse.x >= 50 &&
                    evento.mouse.x <= 250 &&
                    evento.mouse.y >= 500 &&
                    evento.mouse.y <= 565) {
                    al_destroy_display(janela);
                    menuAtacar = 1;
                    calculadora();
                }
                else {
                    menuAtacar = 0;
                }

                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 500 &&
                    evento.mouse.y <= 565) {
                    menuItem = 1;
                }
                else {
                    menuItem = 0;
                }

                if (evento.mouse.x >= 550 &&
                    evento.mouse.x <= 750 &&
                    evento.mouse.y >= 500 &&
                    evento.mouse.y <= 565) {
                    menuFugir = 1;
                }
                else {
                    menuFugir = 0;
                }
            }
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_textf(fonteVida, al_map_rgb(255, 255, 255), 120, 30, ALLEGRO_ALIGN_CENTRE, "Player: %d", VidaPlayer);
        al_draw_textf(fonteVida, al_map_rgb(255, 255, 255), 620, 30, ALLEGRO_ALIGN_RIGHT, "SubtraCao: %d", vidaInimigo);
        al_draw_bitmap(calculitoBatalha, 100, 290, 0);
        al_draw_bitmap(subtracaoBatalha, 460, 80, 0);
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

int jogo(void) {
    int i;
    int tecla = 0;
    //define quando a tela sera atualizada
    int desenha = 1;
    //quando o loop principal deve encerrar
    int sair = 0;
    //largura e altura de cada sprite dentro da folha
    int altura_sprite = 170, largura_sprite = 160;
    int altura_spriteI = 160, largura_spriteI = 160;
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
    vidaInimigo = 3;

    if (start == 0) {
        //inicialização do player
        player.x = 300; player.y = 400;player.direcao_x = 3;player.direcao_y = 3;

        //inicialização do inimigo 1
        inimigosFase1[0].x = 600; inimigosFase1[0].y = 200;
        inimigosFase1[0].lim_y_1 = 200; inimigosFase1[0].lim_y_2 = 400;
        inimigosFase1[0].direcao_x = 0;inimigosFase1[0].direcao_y = 3;

        //inicializacao do inimigo 2
        inimigosFase1[1].x = 200; inimigosFase1[1].y = 100;
        inimigosFase1[1].lim_x_1 = 200; inimigosFase1[1].lim_x_2 = 400;
        inimigosFase1[1].direcao_x = 3;inimigosFase1[1].direcao_y = 0;
    
        start = 1;
    }

    if (!inicializar()) {
        return -1;
    }

    while (!sair) {
        // Pegar as teclas digitadas
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        //fila de inimigos
        for (i = 0; i < (int)(sizeof(inimigosFase1) / sizeof(inimigosFase1[0])); i++) {
            //quando colidi com o inimigo
            if (colidiu(player, inimigosFase1[i])) {
                ultimoColidido = &inimigosFase1[i];
                al_destroy_display(janela);
                menuBatalha();
                return 0;
            }
            //quando não colidi o inimigo
            else {
                printf("Nao colidiu \n");
            }   
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
                if (player.y >= 5) {
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

        //Desenha a nova posição do Personagem na tela
        if (desenha && al_is_event_queue_empty(fila_eventos)) {

            al_draw_bitmap_region(fundo, 0, 0, LARGURA_TELA, ALTURA_TELA, 0, 0, 0);

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

            for (i = 0; i < (int)(sizeof(inimigosFase1) / sizeof(inimigosFase1[0])); i++) {
                //desenha o inimigo na tela nas posicoes X e Y
                al_draw_bitmap_region(inimigo_subtracao, regiao_x_folhaI,
                regiao_y_folhaI, largura_spriteI, altura_spriteI, inimigosFase1[i].x, inimigosFase1[i].y, 0);
            }
            al_flip_display();
            desenha = 0;
        }
    }

    al_destroy_bitmap(direita);
    al_destroy_bitmap(esquerda);
    al_destroy_bitmap(baixo);
    al_destroy_bitmap(cima);
    al_destroy_bitmap(fundo);
    al_destroy_bitmap(quadrado);
    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}


//Função Tutorial
int tutorial() {
    ALLEGRO_DISPLAY* janela = NULL;
    ALLEGRO_FONT* fonte = NULL;

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

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela) {
        error_msg("Falha ao criar janela");
        return -1;
    }
    al_set_window_title(janela, "TUTORIAL");

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
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    int sair = 0;

    while (!sair) {
        al_draw_bitmap(imagem, 0, 0, 0);
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 10 &&
                    evento.mouse.x <= 210 &&
                    evento.mouse.y >= 10 &&
                    evento.mouse.y <= 60) {
                    al_destroy_display(janela);
                    sair = 1;
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


//Função Desenvolvedores
int desenvolvedores() {
    ALLEGRO_DISPLAY* janela = NULL;
    ALLEGRO_FONT* fonte = NULL;

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

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela) {
        error_msg("Falha ao criar janela");
        return -1;
    }
    al_set_window_title(janela, "DESENVOLVEDORES");

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
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    int sair = 0;

    while (!sair) {
        al_draw_bitmap(imagem, 0, 0, 0);
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                if (evento.mouse.x >= 10 &&
                    evento.mouse.x <= 210 &&
                    evento.mouse.y >= 10 &&
                    evento.mouse.y <= 60) {
                    al_destroy_display(janela);
                    sair = 1;
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


//Função Menu
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

    fonte = al_load_font("arial.ttf", 48, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela) {
        error_msg("Falha ao criar janela");
        return -1;
    }
    al_set_window_title(janela, "Menu");

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
                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 150 &&
                    evento.mouse.y <= 200) {
                    al_destroy_display(janela);
                    sair = 1;
                    jogo();
                    return 0;
                }

                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 230 &&
                    evento.mouse.y <= 280) {
                    al_destroy_display(janela);
                    sair = 1;
                    tutorial();
                    return 0;
                }

                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 310 &&
                    evento.mouse.y <= 360) {
                    al_destroy_display(janela);
                    sair = 1;
                    desenvolvedores();
                    return 0;
                }

                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 390 &&
                    evento.mouse.y <= 440) {
                    sair = 1;
                    return 0;
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

    al_destroy_bitmap(botao_sair);
    al_destroy_bitmap(jogar);
    al_destroy_bitmap(instrucoes);
    al_destroy_bitmap(menuDesenvolvedores);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}

int main(void) {
    menu();
    //batalha();
    return 0;
}