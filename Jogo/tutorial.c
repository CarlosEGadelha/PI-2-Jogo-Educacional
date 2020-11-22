////Função Tutorial
//int tutorial() {
//    ALLEGRO_DISPLAY* janela = NULL;
//    ALLEGRO_FONT* fonte = NULL;
//
//    if (!al_init()) {
//        error_msg("Falha ao inicializar a Allegro");
//        return -1;
//    }
//
//    al_init_font_addon();
//
//    if (!al_init_image_addon()) {
//        error_msg("Falha ao inicializar add-on allegro_image");
//        return -1;
//    }
//
//    if (!al_init_ttf_addon()) {
//        error_msg("Falha ao inicializar add-on allegro_ttf");
//        return -1;
//    }
//
//    fonte = al_load_font("arial.ttf", 32, 0);
//    if (!fonte) {
//        al_destroy_display(janela);
//        error_msg("Falha ao carregar fonte");
//        return -1;
//    }
//
//    fonteTitulo = al_load_font("arial.ttf", 42, 0);
//    if (!fonte) {
//        al_destroy_display(janela);
//        error_msg("Falha ao carregar fonte");
//        return -1;
//    }
//
//    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
//    if (!janela) {
//        error_msg("Falha ao criar janela");
//        return -1;
//    }
//    al_set_window_title(janela, "TUTORIAL");
//
//    if (!al_install_mouse()) {
//        error_msg("Falha ao inicializar o mouse");
//        al_destroy_display(janela);
//        return -1;
//    }
//
//    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
//        error_msg("Falha ao atribuir ponteiro do mouse");
//        al_destroy_display(janela);
//        return -1;
//    }
//
//    voltarMenu = al_load_bitmap("sprites/botao_quit.bmp");
//    if (!voltarMenu) {
//        error_msg("Falha ao criar botão de saída");
//        al_destroy_display(janela);
//        return 0;
//    }
//
//    fila_eventos = al_create_event_queue();
//    if (!fila_eventos) {
//        error_msg("Falha ao criar fila de eventos");
//        al_destroy_display(janela);
//        return 0;
//    }
//
//    al_register_event_source(fila_eventos, al_get_mouse_event_source());
//    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
//
//    int sair = 0;
//
//    while (!sair) {
//        al_draw_bitmap(imagem, 0, 0, 0);
//        while (!al_is_event_queue_empty(fila_eventos)) {
//            ALLEGRO_EVENT evento;
//            al_wait_for_event(fila_eventos, &evento);
//            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
//                if (evento.mouse.x >= 10 &&
//                    evento.mouse.x <= 210 &&
//                    evento.mouse.y >= 10 &&
//                    evento.mouse.y <= 60) {
//                    al_destroy_display(janela);
//                    sair = 1;
//                    menu();
//                }
//            }
//        }
//
//        // Preenchemos a tela com a cor ciza
//        al_clear_to_color(al_map_rgb(80, 80, 80));
//
//        // Texto tutorial
//        al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 100, ALLEGRO_ALIGN_CENTRE, "TUTORIAL");
//        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 200, ALLEGRO_ALIGN_CENTRE, "MOVIMENTACAO: W.A.S.D");
//        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 250,
//            ALLEGRO_ALIGN_CENTRE, "INTERACAO NA TELA DE COMBATE: A (Direita)");
//        al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 300, ALLEGRO_ALIGN_CENTRE, "B(Esquerda) - ENTER(Selecionar)");
//
//        al_set_target_bitmap(voltarMenu);
//
//        al_set_target_bitmap(al_get_backbuffer(janela));
//        al_draw_bitmap(voltarMenu, 10, 10, 0);
//
//        al_flip_display();
//    }
//
//    return 0;
//}