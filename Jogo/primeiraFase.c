//int jogo(void) {
//    int i;
//    int tecla = 0;
//    //define quando a tela sera atualizada
//    int desenha = 1;
//    //quando o loop principal deve encerrar
//    int sair = 0;
//    //largura e altura de cada sprite dentro da folha
//    int altura_sprite = 170, largura_sprite = 160;
//    int altura_spriteI = 160, largura_spriteI = 160;
//    //quantos sprites tem em cada linha da folha, e a atualmente mostrada
//    int colunas_folha = 3, coluna_atual = 0;
//    int colunas_folhaI = 3, coluna_atualI = 0;
//    //quantos sprites tem em cada coluna da folha, e a atualmente mostrada
//    int linha_atual = 0, linhas_folha = 2;
//    int linha_atualI = 0, linhas_folhaI = 2;
//    //quantos frames devem se passar para atualizar para o proximo sprite
//    int frames_sprite = 6, cont_frames = 0;
//    int frames_spriteI = 6, cont_framesI = 0;
//    //posicao X Y da janela em que sera mostrado o sprite
//    int regiao_x_folha = 0, regiao_y_folha = 0;
//    int regiao_x_folhaI = 0, regiao_y_folhaI = 0;
//    vidaInimigo = 3;
//
//    if (start == 0) {
//        //inicialização do player
//        player.x = 300; player.y = 400; player.direcao_x = 3; player.direcao_y = 3;
//
//        //inicialização do inimigo 1
//        inimigosFase1[0].x = 600; inimigosFase1[0].y = 200;
//        inimigosFase1[0].lim_y_1 = 200; inimigosFase1[0].lim_y_2 = 400;
//        inimigosFase1[0].direcao_x = 0; inimigosFase1[0].direcao_y = 3;
//
//        //inicializacao do inimigo 2
//        inimigosFase1[1].x = 200; inimigosFase1[1].y = 100;
//        inimigosFase1[1].lim_x_1 = 200; inimigosFase1[1].lim_x_2 = 400;
//        inimigosFase1[1].direcao_x = 3; inimigosFase1[1].direcao_y = 0;
//
//        start = 1;
//    }
//
//    if (!inicializar()) {
//        return -1;
//    }
//
//    while (!sair) {
//        // Pegar as teclas digitadas
//        ALLEGRO_EVENT evento;
//        al_wait_for_event(fila_eventos, &evento);
//
//        //fila de inimigos
//        for (i = 0; i < (int)(sizeof(inimigosFase1) / sizeof(inimigosFase1[0])); i++) {
//            //quando colidi com o inimigo
//            if (colidiu(player, inimigosFase1[i])) {
//                ultimoColidido = &inimigosFase1[i];
//                al_destroy_display(janela);
//                menuBatalha();
//                return 0;
//            }
//            //quando não colidi o inimigo
//            else {
//                printf("Nao colidiu \n");
//            }
//        }
//
//        //movimentacao dos inimigos
//        if (evento.type == ALLEGRO_EVENT_TIMER) {
//            for (i = 0; i < (int)(sizeof(inimigosFase1) / sizeof(inimigosFase1[0])); i++) {
//
//                inimigosFase1[i].x += inimigosFase1[i].direcao_x;
//                inimigosFase1[i].y += inimigosFase1[i].direcao_y;
//
//                //se passou das bordas, inverte a direcao
//                if (inimigosFase1[i].x <= inimigosFase1[i].lim_x_1 ||
//                    inimigosFase1[i].x >= inimigosFase1[i].lim_x_2) {
//                    inimigosFase1[i].direcao_x *= -1;
//                }
//
//                if (inimigosFase1[i].y <= inimigosFase1[i].lim_y_1 ||
//                    inimigosFase1[i].y >= inimigosFase1[i].lim_y_2) {
//                    inimigosFase1[i].direcao_y *= -1;
//                }
//
//            }
//            desenha = 1;
//        }
//
//        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
//            switch (evento.keyboard.keycode) {
//            case ALLEGRO_KEY_UP:
//                tecla = 1;
//                break;
//            case ALLEGRO_KEY_DOWN:
//                tecla = 2;
//                break;
//            case ALLEGRO_KEY_LEFT:
//                tecla = 3;
//                break;
//            case ALLEGRO_KEY_RIGHT:
//                tecla = 4;
//                break;
//            }
//        }
//        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
//            tecla = 5;
//        }
//        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
//            sair = 1;
//        }
//
//        // Estrutura que realiza a movimentação do Personagem
//        if (tecla) {
//            al_clear_to_color(al_map_rgb(255, 255, 255));
//            switch (tecla) {
//            case 1:
//                //colisão de cima
//                if (player.y >= 5) {
//                    player.y -= player.direcao_y;
//                    break;
//                }
//                break;
//            case 2:
//                //colisão de baixo
//                if (player.y <= ALTURA_TELA - 180) {
//                    player.y += player.direcao_y;
//                    break;
//                }
//                break;
//            case 3:
//                //colisão da esquerda
//                if (player.x >= 70) {
//                    player.x -= player.direcao_x;
//                    break;
//                }
//                break;
//            case 4:
//                //colisão da direita
//                if (player.x <= LARGURA_TELA - 130) {
//                    player.x += player.direcao_x;
//                    break;
//                }
//                break;
//            }
//            desenha = 1;
//        }
//
//        if (evento.type == ALLEGRO_EVENT_TIMER) {
//            //a cada disparo do timer, incrementa cont_frames
//            cont_frames++;
//
//            //se alcancou a quantidade de frames que precisa passar para mudar para o proximo sprite
//            if (cont_frames >= frames_sprite) {
//                //reseta cont_frames
//                cont_frames = 0;
//                //incrementa a coluna atual, para mostrar o proximo sprite
//                coluna_atual++;
//                //se coluna atual passou da ultima coluna
//                if (coluna_atual >= colunas_folha) {
//                    //volta pra coluna inicial
//                    coluna_atual = 0;
//                    //incrementa a linha, se passar da ultima, volta pra primeira
//                    linha_atual = (linha_atual + 1) % linhas_folha;
//                    //calcula a posicao Y da folha que sera mostrada
//                    regiao_y_folha = linha_atual * altura_sprite;
//                }
//                //calcula a regiao X da folha que sera mostrada
//                regiao_x_folha = coluna_atual * largura_sprite;
//            }
//        }
//
//        //Loop de movimentacao do inimigo
//        if (evento.type == ALLEGRO_EVENT_TIMER) {
//            //a cada disparo do timer, incrementa cont_frames
//            cont_framesI++;
//            //se alcancou a quantidade de frames que precisa passar para mudar para o proximo sprite
//            if (cont_framesI >= frames_spriteI) {
//                //reseta cont_frames
//                cont_framesI = 0;
//                //incrementa a coluna atual, para mostrar o proximo sprite
//                coluna_atualI++;
//                //se coluna atual passou da ultima coluna
//                if (coluna_atualI >= colunas_folhaI) {
//                    //volta pra coluna inicial
//                    coluna_atualI = 0;
//                    //incrementa a linha, se passar da ultima, volta pra primeira
//                    linha_atualI = (linha_atualI + 1) % linhas_folhaI;
//                    //calcula a posicao Y da folha que sera mostrada
//                    regiao_y_folhaI = linha_atualI * altura_spriteI;
//                }
//                //calcula a regiao X da folha que sera mostrada
//                regiao_x_folhaI = coluna_atualI * largura_spriteI;
//            }
//        }
//
//        //Desenha a nova posição do Personagem na tela
//        if (desenha && al_is_event_queue_empty(fila_eventos)) {
//
//            al_draw_bitmap_region(fundo, 0, 0, LARGURA_TELA, ALTURA_TELA, 0, 0, 0);
//
//            if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 4) {
//                al_draw_bitmap_region(direita, regiao_x_folha,
//                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
//            }
//            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 3) {
//                al_draw_bitmap_region(esquerda, regiao_x_folha,
//                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
//            }
//            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 2) {
//                al_draw_bitmap_region(baixo, regiao_x_folha,
//                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
//            }
//            else if (desenha && al_is_event_queue_empty(fila_eventos) && tecla == 1) {
//                al_draw_bitmap_region(cima, regiao_x_folha,
//                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
//            }
//            else {
//                al_draw_bitmap_region(parado, regiao_x_folha,
//                    regiao_y_folha, largura_sprite, altura_sprite, player.x, player.y, 0);
//            }
//
//            //desenha os inimigos
//            for (i = 0; i < (int)(sizeof(inimigosFase1) / sizeof(inimigosFase1[0])); i++) {
//
//                if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase1[i].direcao_x > 0) {
//                    al_draw_bitmap_region(inimigo_subtracao_direita, regiao_x_folha,
//                        regiao_y_folha, largura_sprite, altura_sprite, inimigosFase1[i].x, inimigosFase1[i].y, 0);
//                }
//                else if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase1[i].direcao_x < 0) {
//                    al_draw_bitmap_region(inimigo_subtracao_esquerda, regiao_x_folha,
//                        regiao_y_folha, largura_sprite, altura_sprite, inimigosFase1[i].x, inimigosFase1[i].y, 0);
//                }
//                else if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase1[i].direcao_y > 0) {
//                    al_draw_bitmap_region(inimigo_subtracao, regiao_x_folha,
//                        regiao_y_folha, largura_sprite, altura_sprite, inimigosFase1[i].x, inimigosFase1[i].y, 0);
//                }
//                else if (desenha && al_is_event_queue_empty(fila_eventos) && inimigosFase1[i].direcao_y < 0) {
//                    al_draw_bitmap_region(inimigo_subtracao_costas, regiao_x_folha,
//                        regiao_y_folha, largura_sprite, altura_sprite, inimigosFase1[i].x, inimigosFase1[i].y, 0);
//                }
//
//            }
//            al_flip_display();
//            desenha = 0;
//        }
//    }
//
//    al_destroy_bitmap(direita);
//    al_destroy_bitmap(esquerda);
//    al_destroy_bitmap(baixo);
//    al_destroy_bitmap(cima);
//    al_destroy_bitmap(fundo);
//    al_destroy_bitmap(quadrado);
//    al_destroy_timer(timer);
//    al_destroy_display(janela);
//    al_destroy_event_queue(fila_eventos);
//
//    return 0;
//}