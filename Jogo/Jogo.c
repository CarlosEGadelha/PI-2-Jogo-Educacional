// Inclui o arquivo de cabeçalho da biblioteca Allegro 5
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

//fps = frames per second = atualizacoes de tela por segundo
#define FPS 60.0
#define LARGURA_TELA 800
#define ALTURA_TELA 600

ALLEGRO_DISPLAY* janela = NULL;
ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
ALLEGRO_FONT* fonte = NULL;
ALLEGRO_FONT* atacar = NULL;
ALLEGRO_FONT* item = NULL;
ALLEGRO_FONT* fugir = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_BITMAP* quadrado = NULL;
ALLEGRO_BITMAP* direita = NULL;
ALLEGRO_BITMAP* esquerda = NULL;
ALLEGRO_BITMAP* cima = NULL;
ALLEGRO_BITMAP* baixo = NULL;
ALLEGRO_BITMAP* parado = NULL;
ALLEGRO_BITMAP* fundo = NULL;
ALLEGRO_AUDIO_STREAM* musica = NULL;
ALLEGRO_BITMAP* imagem = NULL;
ALLEGRO_BITMAP* botao_sair = NULL;
ALLEGRO_BITMAP* jogar = 0;
ALLEGRO_BITMAP* instrucoes = 0;
ALLEGRO_BITMAP* menuDesenvolvedores = 0;
ALLEGRO_BITMAP* voltarMenu = 0;
ALLEGRO_BITMAP* fonteTitulo = 0;

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

    //Inicia a imagem do inimigo
    quadrado = al_create_bitmap(50, 50);
    if (!quadrado) {
        error_msg("Falha ao criar inimigo");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        return 0;
    }
    al_set_target_bitmap(quadrado);


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

//Função para a batalha
int batalha(void) {
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

    atacar = al_create_bitmap(200, 65);
    if (!atacar) {
        error_msg("Falha ao criar bitmap");
        al_destroy_display(janela);
        return -1;
    }

    item = al_create_bitmap(200, 65);
    if (!item) {
        error_msg("Falha ao criar bitmap");
        al_destroy_display(janela);
        return -1;
    }

    fugir = al_create_bitmap(200, 65);
    if (!fugir) {
        error_msg("Falha ao criar bitmap");
        al_destroy_display(janela);
        return -1;
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
        al_clear_to_color(al_map_rgb(0, 0, 0));
        while (!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            //Verifica se o mouse esta em algum dos botões
            if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
                if (evento.mouse.x >= 50 &&
                    evento.mouse.x <= 250 &&
                    evento.mouse.y >= 450 &&
                    evento.mouse.y <= 515) {
                    menuAtacar = 1;
                }
                else {
                    menuAtacar = 0;
                }

                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 450 &&
                    evento.mouse.y <= 515) {
                    menuItem = 1;
                }
                else {
                    menuItem = 0;
                }

                if (evento.mouse.x >= 550 &&
                    evento.mouse.x <= 750 &&
                    evento.mouse.y >= 450 &&
                    evento.mouse.y <= 515) {
                    menuFugir = 1;
                }
                else {
                    menuFugir = 0;
                }
            }
        }

        //Muda o cor do botão se o mouse estiver em cima
        al_set_target_bitmap(atacar);
        if (!menuAtacar) {
            al_clear_to_color(al_map_rgb(255, 255, 255));
        }
        else {
            al_clear_to_color(al_map_rgb(0, 255, 0));
        }

        al_set_target_bitmap(item);
        if (!menuItem) {
            al_clear_to_color(al_map_rgb(255, 255, 255));
        }
        else {
            al_clear_to_color(al_map_rgb(0, 0, 255));
        }

        al_set_target_bitmap(fugir);
        if (!menuFugir) {
            al_clear_to_color(al_map_rgb(255, 255, 255));
        }
        else {
            al_clear_to_color(al_map_rgb(255, 255, 0));
        }

        al_set_target_bitmap(al_get_backbuffer(janela));
        al_draw_bitmap(atacar, 50, 450, 0);
        al_draw_bitmap(item, 300, 450, 0);
        al_draw_bitmap(fugir, 550, 450, 0);

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
    int tecla = 0;
    //define quando a tela sera atualizada
    int desenha = 1;
    //quando o loop principal deve encerrar
    int sair = 0;
    //posicao do quadrado e quanto ele andara a cada disparo do timer, para coordenada X e Y
    int posx = 300, posxI= 700;
    int posy = 400, posyI = 500;
    int direcao = 3, direcaoI = 3;
    //largura e altura de cada sprite dentro da folha
    int altura_sprite = 170, largura_sprite = 160;
    //quantos sprites tem em cada linha da folha, e a atualmente mostrada
    int colunas_folha = 3, coluna_atual = 0;
    //quantos sprites tem em cada coluna da folha, e a atualmente mostrada
    int linha_atual = 0, linhas_folha = 2;
    //quantos frames devem se passar para atualizar para o proximo sprite
    int frames_sprite = 6, cont_frames = 0;
    //posicao X Y da janela em que sera mostrado o sprite
    int regiao_x_folha = 0, regiao_y_folha = 0;

    if (!inicializar()) {
        return -1;
    }

    while (!sair) {
        // Pegar as teclas digitadas
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        //Loop de movimentacao do inimigo
        if (evento.type == ALLEGRO_EVENT_TIMER) {

            posyI += direcaoI;
            //se passou das bordas, inverte a direcao
            if (posyI <= 490 || posyI >= 600)
                direcaoI *= -1; 
           
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
                if (posy >= 5) {
                    posy -= direcao;
                    break;
                }
                break;
            case 2:
                //colisão de baixo
                if (posy <= ALTURA_TELA - 180) {
                    posy += direcao;
                    break;
                }
                break;
            case 3:
                //colisão da esquerda
                if (posx >= 70) {
                    posx -= direcao;
                    break;
                }
                break;
            case 4:
                //colisão da direita
                if (posx <= LARGURA_TELA - 130) {
                    posx += direcao;
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


        //Desenha a nova posição do Personagem na tela
        if (desenha && al_is_event_queue_empty(fila_eventos)) {

            al_draw_bitmap_region(fundo, 0, 0, LARGURA_TELA, ALTURA_TELA, 0, 0, 0);

            if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 4) {
                al_draw_bitmap_region(direita, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, posx, posy, 0);
            }
            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 3) {
                al_draw_bitmap_region(esquerda, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, posx, posy, 0);
            }
            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 2) {
                al_draw_bitmap_region(baixo, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, posx, posy, 0);
            }
            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 1) {
                al_draw_bitmap_region(cima, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, posx, posy, 0);
            }
            else {
                al_draw_bitmap_region(parado, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, posx, posy, 0);
            }

            //desenha o quadrado na tela nas posicoes X e Y
            al_draw_bitmap(quadrado, posxI, posyI, 0);
            
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
                }

                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 230 &&
                    evento.mouse.y <= 280) {
                    al_destroy_display(janela);
                    sair = 1;
                    tutorial();
                }

                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 310 &&
                    evento.mouse.y <= 360) {
                    al_destroy_display(janela);
                    sair = 1;
                    desenvolvedores();
                }

                if (evento.mouse.x >= 300 &&
                    evento.mouse.x <= 500 &&
                    evento.mouse.y >= 390 &&
                    evento.mouse.y <= 440) {
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