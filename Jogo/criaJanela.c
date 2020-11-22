#include <allegro5/allegro.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600

int criaJanela(void){
    ALLEGRO_DISPLAY* janela = NULL;
    
    al_init();

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);

    return 0;
}