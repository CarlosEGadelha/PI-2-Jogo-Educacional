// Inclui o arquivo de cabeçalho da biblioteca Allegro 5
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

//fps = frames per second = atualizacoes de tela por segundo
#define FPS 60.0
#define LARGURA_TELA 800
#define ALTURA_TELA 600

ALLEGRO_DISPLAY* janela = NULL;
ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
ALLEGRO_FONT* fonte = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_BITMAP* direita = NULL;
ALLEGRO_BITMAP* esquerda = NULL;
ALLEGRO_BITMAP* cima = NULL;
ALLEGRO_BITMAP* baixo = NULL;
ALLEGRO_BITMAP* fundo = NULL;
ALLEGRO_AUDIO_STREAM* musica = NULL;

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
        error_msg("Falha ao carregar \"arial.ttf\"");
        al_destroy_display(janela);
        return 0;
    }

    //Inicia a imagem para a movimentação do personagem para a Direita
    direita = al_load_bitmap("movimento_direita.bmp");
    if (!direita) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(direita, al_map_rgb(255, 0, 255));

    //Inicia a imagem para a movimentação do personagem para a Esquerda
    esquerda = al_load_bitmap("movimento_esquerda.bmp");
    if (!esquerda) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(esquerda, al_map_rgb(255, 0, 255));

    //Inicia a imagem para a movimentação do personagem para a Cima
    cima = al_load_bitmap("movimento_cima.bmp");
    if (!cima) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(cima, al_map_rgb(255, 0, 255));

    //Inicia a imagem para a movimentação do personagem para a Baixo
    baixo = al_load_bitmap("movimento_baixo.bmp");
    if (!baixo) {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }
    al_convert_mask_to_alpha(baixo, al_map_rgb(255, 0, 255));

    //Inicia a imagem de fundo
    fundo = al_load_bitmap("mapa_base_montanha.bmp");
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

    //al_set_target_bitmap(quadrado);
    al_clear_to_color(al_map_rgb(255, 0, 0));
    al_set_target_bitmap(al_get_backbuffer(janela));

    fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        //al_destroy_bitmap(quadrado);
        al_destroy_audio_stream(musica);

        return 0;
    }

    //registra duas fontes de eventos na fila. o da janela, e do teclado
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    //al_set_target_bitmap(quadrado);
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
    int posx = 300;
    int posy = 400, direcao = 3;
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
                //colisão de cima
                if (posy >= 5) {
                    posy -= direcao;
                    break;
                }
                break;
            case 2:
                //colisão de baixo
                if (posy <= ALTURA_TELA-180) {
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
                if (posx <= LARGURA_TELA-130) {
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
                al_draw_bitmap_region(baixo, regiao_x_folha,
                    regiao_y_folha, largura_sprite, altura_sprite, posx, posy, 0);
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
    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}