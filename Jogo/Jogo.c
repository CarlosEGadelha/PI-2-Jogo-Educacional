// Inclui o arquivo de cabeçalho da biblioteca Allegro 5
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

//fps = frames per second = atualizacoes de tela por segundo
#define FPS 60.0
#define LARGURA_TELA 640
#define ALTURA_TELA 480

ALLEGRO_DISPLAY* janela = NULL;
ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
ALLEGRO_FONT* fonte = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_BITMAP* quadrado = NULL;

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
        error_msg("Falha ao carregar \"arial.ttf\"");
        al_destroy_display(janela);
        return 0;
    }

    quadrado = al_create_bitmap(50, 50);
    if (!quadrado) {
        error_msg("Falha ao criar bitmap");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        return 0;
    }

    al_set_target_bitmap(quadrado);
    al_clear_to_color(al_map_rgb(255, 0, 0));
    al_set_target_bitmap(al_get_backbuffer(janela));

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_bitmap(quadrado);
        return 0;
    }

    //registra duas fontes de eventos na fila. o da janela, e do teclado
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    quadrado = al_create_bitmap(50, 50);
    if (!quadrado) {
        error_msg("Falha ao criar bitmap");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        return 0;
    }

    al_set_target_bitmap(quadrado);
    al_clear_to_color(al_map_rgb(255, 0, 0));
    al_set_target_bitmap(al_get_backbuffer(janela));
    fila_eventos, al_get_timer_event_source(timer);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    al_start_timer(timer);

    return 1;
}

int main(void) {
    int tecla = 0;
    //define quando a tela sera atualizada
    int desenha = 1;
    //quando o loop principal deve encerrar
    int sair = 0;
    //posicao do quadrado e quanto ele andara a cada disparo do timer, para coordenada X e Y
    int posx = 200;
    int posy = 200, direcao = 5;

    if (!inicializar()) {
        return -1;
    }

    while (!sair) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);
        // Pegar as teclas digitadas
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
                posy -= direcao;
                break;
            case 2:
                posy += direcao;
                break;
            case 3:
                posx -= direcao;
                break;
            case 4:
                posx += direcao;
                break;
            }
            desenha = 1;
        }

        //Desenha a nova posição do Personagem na tela
        if (desenha && al_is_event_queue_empty(fila_eventos)) {

            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_bitmap(quadrado, posx, posy, 0);
            al_flip_display();
            desenha = 0;
        }
    }

    al_destroy_bitmap(quadrado);
    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}