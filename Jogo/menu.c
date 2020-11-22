#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "criaJanela.c"

ALLEGRO_BITMAP* janela = NULL;
ALLEGRO_BITMAP* imagem = NULL;
ALLEGRO_BITMAP* botao_sair = NULL;
ALLEGRO_BITMAP* jogar = 0;
ALLEGRO_BITMAP* instrucoes = 0;
ALLEGRO_BITMAP* menuDesenvolvedores = 0;
ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

int menu() {
    janela = criaJanela();

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

    /*fonte = al_load_font("arial.ttf", 48, 0);
    if (!fonte) {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }*/

    /*janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela) {
        error_msg("Falha ao criar janela");
        return -1;
    }
    al_set_window_title(janela, "Menu");*/

    imagem = al_load_bitmap("Cenario/Menu.bmp");
    if (!imagem) {
        error_msg("Falha ao carregar o arquivo de imagem");
        al_destroy_display(criaJanela());
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
                    al_destroy_event_queue(fila_eventos);
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