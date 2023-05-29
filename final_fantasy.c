#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

int random(int n){
	return rand()%n;
}

int randInt(int min, int max){
	return min+random(max-min+1);
}


#define MAXTAM 100
#define NAVEGACAO 0
#define BATALHA 1
#define TAM_HERO 20
#define PASSO 20
#define TAM_OBJ 50
#define MAX_DIST 20
#define ATACAR 0
#define ESPECIAL 1
#define FUGIR 2
#define NOACTION 3

#define DIREITA 1
#define ESQUERDA 2
#define CIMA 3
#define BAIXO  4

#define FRACO 0
#define FORTE 1


const float FPS = 100;

const int SCREEN_W = 960 ;
const int SCREEN_H = 540 ;

#define hero_battle_x_up_left  650  * SCREEN_W / 960
#define hero_battle_y_up_left  150 * SCREEN_H / 540
#define hero_battle_x_down_right  800 * SCREEN_W / 960
#define hero_battle_y_down_right 300 * SCREEN_H / 540

#define monster_battle_x 250 * SCREEN_W / 960
#define monster_battle_y (150 + 300)/2 * SCREEN_H / 540
#define monster_battle_radius 150 * SCREEN_W * SCREEN_H / (960 * 540)

 // 650, 150, 800, 300


	//al_draw_filled_rectangle(hero_battle_x_up_left, hero_battle_y_up_left, hero_battle_x_down_right,hero_battle_y_down_right, hero.cor);

	//al_draw_filled_rectangle(650, 150, 800, 300, hero.cor);

// variaveis globais

int CEU_H;
int X_OPT;
int Y_OPT;
ALLEGRO_FONT *FONTE; // eh um ponteiro (medo)
ALLEGRO_SAMPLE *musica = NULL;
ALLEGRO_SAMPLE_INSTANCE *musicaInstance = NULL;
ALLEGRO_SAMPLE *tiro = NULL;
ALLEGRO_SAMPLE_INSTANCE *tiroInstance = NULL;
ALLEGRO_SAMPLE *murloc = NULL;
ALLEGRO_SAMPLE_INSTANCE *murlocInstance;
ALLEGRO_SAMPLE *loktar = NULL;
ALLEGRO_SAMPLE_INSTANCE *loktarInstance;
ALLEGRO_SAMPLE *levelup = NULL;
ALLEGRO_SAMPLE_INSTANCE *levelupInstance;
ALLEGRO_SAMPLE *walk = NULL;
ALLEGRO_SAMPLE_INSTANCE *walkInstance;
ALLEGRO_SAMPLE *freedom = NULL;
ALLEGRO_SAMPLE_INSTANCE *freedomInstance = NULL;
//variavel do tipo char[] que recebe um texto
char my_text[20];
//variavel que recebe o valor de entrada
int pontos = 0;

void initGlobais() {

	CEU_H = SCREEN_H / 4;
	X_OPT = 3*SCREEN_W / 4;
	Y_OPT = 3*SCREEN_H / 4;


	int tam_fonte = 32;
	if (SCREEN_H < 300){
		tam_fonte = 20;
	}
	//carrega o arquivo symtext.ttf da fonte symtext e define que sera usado o tamanho 32 (segundo parametro)
    FONTE = al_load_font("symtext.ttf", tam_fonte, 1);
	if(FONTE == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

}



float dist(int x1, int y1, int x2, int y2){
	return sqrt( pow( x1-x2 ,2) + pow( y1-y2 ,2) );
}

void drawTelaFinal(){
	al_clear_to_color(al_map_rgb(0,0,0));
}

 // ------------------------ NAVEGACAO ------------------------

//ALLEGRO_FONT *size_32 = al_load_font("arialarial.ttf", 32, 1);

void draw_scenario_NAVEGACAO(ALLEGRO_BITMAP *background3){
	sprintf(my_text, "Level: %d", pontos);

	//al_draw_text(FONTE, al_map_rgb(255, 255, 255), X_OPT + 15, Y_OPT + 15, 0, my_text);

	al_draw_bitmap(background3, 0, 0 , 0);

	//al_clear_to_color(al_map_rgb(5,105,50));
	//al_draw_filled_rectangle(SCREEN_W - TAM_OBJ, 0, SCREEN_W, TAM_OBJ, al_map_rgb(102,88,91));

	al_play_sample_instance(musicaInstance);

	al_draw_text(FONTE, al_map_rgb(235,213,70), 0, 0, 0, my_text);

}

typedef struct Str_Hero{

	//exploracao
	int x,y;
	ALLEGRO_COLOR cor;
	int HP;

	//batalha
	int acao;
	int executar;
	int x_old, y_old;


} T_Hero;

void initHero(T_Hero *hero, ALLEGRO_BITMAP *heroPIC){
	hero->y = SCREEN_H - al_get_bitmap_height(heroPIC); // - TAM_HERO;
	hero->x = 0;
	hero->cor = al_map_rgb(225, 32 ,5);
	hero->HP = 100;

	hero->acao = ATACAR;
	hero->executar = 0;
	hero->x_old = hero->x;
	hero->y_old = hero->y;
}

void draw_Hero_NAVEGACAO(T_Hero hero, ALLEGRO_BITMAP *heroPIC){
	al_draw_bitmap(heroPIC, hero.x, hero.y, 0);
 }

void processaTeclaNaveg(T_Hero *hero, int tecla, ALLEGRO_BITMAP *heroDesenho, int *direcao){

	hero->x_old = hero->x;
	hero->y_old = hero->y;

	switch(tecla){

		case ALLEGRO_KEY_UP:
			al_play_sample_instance(walkInstance);
			if (hero->y - PASSO >= 0)
				hero->y -= PASSO;
			*direcao = CIMA;
			break;

		case ALLEGRO_KEY_DOWN:
			al_play_sample_instance(walkInstance);
			if (hero->y + al_get_bitmap_height(heroDesenho) + PASSO <= SCREEN_H)
				hero->y += PASSO;
			*direcao = BAIXO;
			break;

		case ALLEGRO_KEY_LEFT:
			al_play_sample_instance(walkInstance);
			if (hero->x - PASSO >= 0)
				hero->x -= PASSO;
			*direcao = ESQUERDA;
			break;

		case ALLEGRO_KEY_RIGHT:
			al_play_sample_instance(walkInstance);
			if (hero->x + al_get_bitmap_width(heroDesenho) + PASSO <= SCREEN_W)
				hero->x += PASSO;
			*direcao = DIREITA;
			break;
	}
}

void drawHP_NAVEGACAO(T_Hero *hero, ALLEGRO_BITMAP *sangue){
	al_draw_filled_rectangle(SCREEN_W * 13/16 , SCREEN_H * 16/18, SCREEN_W * 16/17 , SCREEN_H * 17/18, al_map_rgb(0,0,0));

	al_draw_filled_rectangle(SCREEN_W * 13/16 , SCREEN_H * 16/18, SCREEN_W*16/17 - (SCREEN_W*16/17 - SCREEN_W * 13/16)* (100 - hero->HP)/100 , SCREEN_H * 17/18, al_map_rgb(80,0,0));

	al_draw_bitmap(sangue, SCREEN_W * 13/16 - 50, SCREEN_H * 14/16, 0);

}

 int chegouObjetivo(T_Hero hero, ALLEGRO_BITMAP *heroPIC){

	 if (hero.x + al_get_bitmap_width(heroPIC)>= SCREEN_W - TAM_OBJ && hero.y - al_get_bitmap_height(heroPIC) <= TAM_OBJ) return 1;
	 else return 0;

 }


 // ------------------------ BATALHA ------------------------

void draw_scenario_BATALHA(ALLEGRO_BITMAP *madeira, T_Hero *hero){
	al_clear_to_color(al_map_rgb(5,105,50));
	al_draw_filled_rectangle(0,0,SCREEN_W,CEU_H, al_map_rgb(0,0,255));

	//al_draw_filled_rectangle(X_OPT, Y_OPT, SCREEN_W, SCREEN_H, al_map_rgb(20,16,77));

	al_draw_filled_rectangle(X_OPT - 5, Y_OPT - 5, SCREEN_W, SCREEN_H, al_map_rgb(0,0,0));
	al_draw_bitmap(madeira, X_OPT, Y_OPT, 0);

	if (hero->acao == ATACAR){
		al_draw_text(FONTE, al_map_rgb(210,210,90), X_OPT + 15, Y_OPT + 10, 0, "ATACAR");
		al_draw_text(FONTE, al_map_rgb(140,140,140), X_OPT + 15, Y_OPT + 40, 0, "ESPECIAL");
		al_draw_text(FONTE, al_map_rgb(140,140,140), X_OPT + 15, Y_OPT + 70, 0, "FUGIR");
	}
	else if (hero->acao == ESPECIAL){
		al_draw_text(FONTE, al_map_rgb(140,140,140), X_OPT + 15, Y_OPT + 10, 0, "ATACAR");
		al_draw_text(FONTE, al_map_rgb(210,210,90), X_OPT + 15, Y_OPT + 40, 0, "ESPECIAL");
		al_draw_text(FONTE, al_map_rgb(140,140,140), X_OPT + 15, Y_OPT + 70, 0, "FUGIR");
	}
	else if (hero->acao == FUGIR || hero->acao == NOACTION){
		al_draw_text(FONTE, al_map_rgb(140,140,140), X_OPT + 15, Y_OPT + 10, 0, "ATACAR");
		al_draw_text(FONTE, al_map_rgb(140,140,140), X_OPT + 15, Y_OPT + 40, 0, "ESPECIAL");
		al_draw_text(FONTE, al_map_rgb(210,210,90), X_OPT + 15, Y_OPT + 70, 0, "FUGIR");
	}

 }

void draw_hero_BATALHA(T_Hero *hero, ALLEGRO_BITMAP *hero3){

	//printf("%d", hero_battle_x_down_right / 100 * hero->HP);

	// p/ o retangulo

	//al_draw_filled_rectangle(hero_battle_x_up_left, hero_battle_y_up_left, hero_battle_x_down_right,hero_battle_y_down_right, hero->cor);

	//printf("%f %f %f %f", hero_battle_x_up_left, hero_battle_y_up_left, hero_battle_x_down_right,hero_battle_y_down_right);
	//al_draw_filled_rectangle(650, 150, 800, 300, hero.cor);

	al_draw_bitmap(hero3, hero_battle_x_up_left, hero_battle_y_up_left, 0);

	//HP DO HEROI

	al_draw_filled_rectangle(hero_battle_x_up_left, hero_battle_y_up_left - 35,  hero_battle_x_up_left + al_get_bitmap_width(hero3), hero_battle_y_up_left - 10,al_map_rgb(0,0,0));
	al_draw_filled_rectangle(hero_battle_x_up_left, hero_battle_y_up_left - 35, hero_battle_x_up_left + al_get_bitmap_width(hero3) - (hero_battle_x_up_left + al_get_bitmap_width(hero3) - hero_battle_x_up_left) * (100 - hero->HP) / 100 , hero_battle_y_up_left - 10,al_map_rgb(135,35,35));



}


typedef struct Str_Monster{
	int x,y;
	int HP;
	int tipo;
	ALLEGRO_COLOR color;

	//batalha
	int acao;
	int executar;

} T_Monster;


void initMonster(T_Monster vetorMonstros[], int n){
	if (n % 2 == 0){
		for(int i = 0; i < n/2; i++){
			srand(time(NULL)*(i)*(i));
			vetorMonstros[i].x = randInt(0 + MAX_DIST, SCREEN_W - MAX_DIST);
			vetorMonstros[i].y = randInt(0 + MAX_DIST, SCREEN_H - MAX_DIST);
			printf("Monstro %d:\n-x: %d \n-y: %d\n\n", i, vetorMonstros[i].x, vetorMonstros[i].y);

			vetorMonstros[i].color = al_map_rgb(rand()%255,rand()%255,rand()%255);
			vetorMonstros[i].HP = 70;

			vetorMonstros[i].tipo = FRACO;
		}
		for(int i = n/2; i < n; i++){
			srand(time(NULL)*(i)*(i));
			vetorMonstros[i].x = randInt(0 + MAX_DIST, SCREEN_W - MAX_DIST);
			vetorMonstros[i].y = randInt(0 + MAX_DIST, SCREEN_H - MAX_DIST);
			printf("Monstro %d:\n-x: %d \n-y: %d\n\n", i, vetorMonstros[i].x, vetorMonstros[i].y);

			vetorMonstros[i].color = al_map_rgb(rand()%255,rand()%255,rand()%255);
			vetorMonstros[i].HP = 100;

			vetorMonstros[i].tipo = FORTE;
		}
	} else if (n % 2 == 1) {
		for(int i = 0; i < (n + 1) /2; i++){
			srand(time(NULL)*(i)*(i));
			vetorMonstros[i].x = randInt(0 + MAX_DIST, SCREEN_W - MAX_DIST);
			vetorMonstros[i].y = randInt(0 + MAX_DIST, SCREEN_H - MAX_DIST);
			printf("Monstro %d:\n-x: %d \n-y: %d\n\n", i, vetorMonstros[i].x, vetorMonstros[i].y);

			vetorMonstros[i].color = al_map_rgb(rand()%255,rand()%255,rand()%255);
			vetorMonstros[i].HP = 100;

			vetorMonstros[i].tipo = FRACO;
		}
		for(int i = (n + 1) /2 ; i < n; i++){
			srand(time(NULL)*(i)*(i));
			vetorMonstros[i].x = randInt(0 + MAX_DIST, SCREEN_W - MAX_DIST);
			vetorMonstros[i].y = randInt(0 + MAX_DIST, SCREEN_H - MAX_DIST);
			printf("Monstro %d:\n-x: %d \n-y: %d\n\n", i, vetorMonstros[i].x, vetorMonstros[i].y);

			vetorMonstros[i].color = al_map_rgb(rand()%255,rand()%255,rand()%255);
			vetorMonstros[i].HP = 100;

			vetorMonstros[i].tipo = FORTE;
		}
	}

}


void draw_Monster_BATALHA(T_Monster vetorMonstros[], int indice_monstro_encontrado, ALLEGRO_BITMAP *monster1, ALLEGRO_BITMAP *monster2){
	//p/ o circulo
	//al_draw_filled_circle(monster_battle_x,monster_battle_y, monster_battle_radius, vetorMonstros[indice_monstro_encontrado].color);

	if (vetorMonstros[indice_monstro_encontrado].tipo == FORTE)
		al_draw_bitmap(monster1, monster_battle_x - monster_battle_radius, monster_battle_y - monster_battle_radius, 0);
	else if (vetorMonstros[indice_monstro_encontrado].tipo == FRACO)
		al_draw_bitmap(monster2, monster_battle_x - monster_battle_radius, monster_battle_y - monster_battle_radius, 0);

	//HP DO MONSTRO
	if (vetorMonstros[indice_monstro_encontrado].tipo == FORTE){
		al_draw_filled_rectangle(monster_battle_x - monster_battle_radius, monster_battle_y - monster_battle_radius - 35, monster_battle_x - monster_battle_radius + al_get_bitmap_width(monster1), monster_battle_y - monster_battle_radius - 10,al_map_rgb(0,0,0));
		al_draw_filled_rectangle(monster_battle_x - monster_battle_radius,
								monster_battle_y - monster_battle_radius - 35,
								monster_battle_x - monster_battle_radius + al_get_bitmap_width(monster1) - (monster_battle_x - monster_battle_radius + al_get_bitmap_width(monster1) - (monster_battle_x - monster_battle_radius)) * ( 100 - vetorMonstros[indice_monstro_encontrado].HP) / 100,
								monster_battle_y - monster_battle_radius - 10,al_map_rgb(135,35,35));
	}
	else if (vetorMonstros[indice_monstro_encontrado].tipo == FRACO){
		al_draw_filled_rectangle(monster_battle_x - monster_battle_radius, monster_battle_y - monster_battle_radius - 35, monster_battle_x - monster_battle_radius + al_get_bitmap_width(monster1), monster_battle_y - monster_battle_radius - 10,al_map_rgb(0,0,0));
		al_draw_filled_rectangle(monster_battle_x - monster_battle_radius,
								monster_battle_y - monster_battle_radius - 35,
								monster_battle_x - monster_battle_radius + al_get_bitmap_width(monster1) - (monster_battle_x - monster_battle_radius + al_get_bitmap_width(monster1) - (monster_battle_x - monster_battle_radius)) * ( 70 - vetorMonstros[indice_monstro_encontrado].HP) / 70,
								monster_battle_y - monster_battle_radius - 10,al_map_rgb(135,35,35));
	}
}

typedef struct EST_Bullet{
	int x, y;
	int v_x; // (v_y)
	ALLEGRO_COLOR color;
	int tam;
} T_Bullet;

void initBullet_normal(T_Bullet *bullet){
	bullet->x = hero_battle_x_up_left;
	bullet->y = (hero_battle_y_down_right + hero_battle_y_up_left ) / 2;
	bullet->v_x = 1;
	bullet->tam = 20;
	bullet->color = al_map_rgb(0,0,0);
}

void initBullet_especial(T_Bullet *bullet){
	bullet->x = hero_battle_x_up_left;
	bullet->y = (hero_battle_y_down_right + hero_battle_y_up_left ) / 2;
	bullet->v_x = 1;
	bullet->tam = 20;
	bullet->color = al_map_rgb(255,30,30);
}

void initBullet_monstro(T_Bullet *bullet){
	bullet->x = monster_battle_x + monster_battle_radius;
	bullet->y = monster_battle_y;
	bullet->v_x = 1;
	bullet->tam = 25;
	bullet->color = al_map_rgb(0,255,25);
}

void drawBULLET_hero(T_Bullet *bullet){
	al_draw_filled_circle(bullet->x, bullet->y, bullet->tam, bullet->color );

	bullet->x -= bullet->v_x;
}

void drawBULLET_monstro(T_Bullet *bullet, T_Monster vetorMonstros[], int indice_monstro_encontrado){
	if (vetorMonstros[indice_monstro_encontrado].tipo == FORTE)
		al_draw_filled_circle(bullet->x, bullet->y, bullet->tam, bullet->color = al_map_rgb(0,255,25));
	else if (vetorMonstros[indice_monstro_encontrado].tipo == FRACO)
		al_draw_filled_circle(bullet->x, bullet->y, bullet->tam, bullet->color = al_map_rgb(60,160,250));
	bullet->x += bullet->v_x;
}

int detectouMonstro(T_Hero hero, T_Monster vetorMonstros[], int n, int *indice_monstro_encontrado){

	for(int i = 0; i < n; i++){
		if (dist(hero.x, hero.y, vetorMonstros[i].x, vetorMonstros[i].y) <= MAX_DIST){
			*indice_monstro_encontrado = i;
			return 1;
		}
	}
	return 0;

 }

void processaTeclaBat(T_Hero *hero, int tecla){

	int chance = 0;

	switch(tecla){

		case ALLEGRO_KEY_1:
			hero->acao = ATACAR;
			break;

		case ALLEGRO_KEY_2:
			hero->acao = ESPECIAL;
			break;

		case ALLEGRO_KEY_3:
			chance = randInt(1,100);
			printf("\nchance de fuga: %d", chance);
			if (chance < 45)
				hero->acao = FUGIR;
			else
				hero->acao = NOACTION;
			break;

		case ALLEGRO_KEY_ENTER:
			hero->executar = 1;
			break;

	}

 }

int processaAcaoHero(T_Hero *hero, int *para_desenhar_bullet_normal, int *para_desenhar_bullet_especial, T_Monster vetorMonstros[], int *indice_monstro_encontrado, int *turno_do_heroi, int *hero_pode_atacar, int *just_killed, int *just_runned){

	int numero_da_probabilidade = 0;

	if (hero->executar == 1){

		hero->executar=0;

		if (*hero_pode_atacar == 1){
			if (hero->acao == ATACAR){
				*para_desenhar_bullet_normal = 1;
				*hero_pode_atacar = 0;

			}

			if (hero->acao == ESPECIAL){

				*para_desenhar_bullet_especial = 1;
				*hero_pode_atacar = 0;
			}

			if (hero->acao == FUGIR){

				*hero_pode_atacar = 0;
				*turno_do_heroi = 0;
				
				al_play_sample_instance(freedomInstance);
				
				*just_runned = 1; // ACABOU DE FUGIR

				hero->x = hero->x_old;
				hero->y = hero->y_old;

				return NAVEGACAO;
			}

			if (hero->acao == NOACTION){
				
				// NECESSARIO PARA MUDAR A CHANCE DE FUGA APÓS A FALHA DA FUGA !!! SE NAO, NECESSARIO APERTAR 3, NOVAMENTE
				int chance = randInt(1,100);
				printf("\nchance de fuga: %d", chance);
				if (chance < 45)
					hero->acao = FUGIR;
				else
					hero->acao = NOACTION;
				//
				
				*hero_pode_atacar = 0;
				*turno_do_heroi = 0;
			}
		}
	}

	return BATALHA;

}


void ataqueMonstro(T_Hero *hero, T_Monster vetorMonstros[], int indice_monstro_encontrado){
	int chance = 0;

	if (vetorMonstros[indice_monstro_encontrado].tipo == FORTE)
		hero->HP -= randInt(20,25);
		
	if (vetorMonstros[indice_monstro_encontrado].tipo == FRACO)
		hero->HP -= randInt(12, 17);
	
}	

int main(int argc, char **argv){

	srand(time(NULL));

	FILE *f_records_app = fopen("records.txt", "a");

	if (f_records_app == NULL){
		printf("\nErro ao abrir arquivo !");
	}

	//cria as variaveis para as figuras
	ALLEGRO_DISPLAY *display = NULL;

	ALLEGRO_EVENT_QUEUE *event_queue = NULL;

	ALLEGRO_TIMER *timer = NULL;

    ALLEGRO_BITMAP *heroPIC = NULL;

    ALLEGRO_BITMAP *hero3 = NULL;

	ALLEGRO_BITMAP *monster1 = NULL;

	ALLEGRO_BITMAP *monster2 = NULL;

	ALLEGRO_BITMAP *madeira = NULL;

	ALLEGRO_BITMAP *background3 = NULL;

	ALLEGRO_BITMAP *sangue = NULL;

	ALLEGRO_BITMAP *hero_esq = NULL;

	ALLEGRO_BITMAP *hero_dir = NULL;

	ALLEGRO_BITMAP *hero_bax = NULL;

	//----------------------- rotinas de inicializacao ---------------------------------------

	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }

	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}

	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

 	//inicicializa o modulo allegro para audio
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(10);

	//inicializar o modulo de imagens
    al_init_image_addon();

    //carregar as figuras
    hero_dir = al_load_bitmap("hero_dir.png");
	hero3 = al_load_bitmap("hero3.png");
	monster1 = al_load_bitmap("monster1.png");
	monster2 = al_load_bitmap("monster2.png");
	madeira = al_load_bitmap("madeira.png");
	background3 = al_load_bitmap("background3.png");
	sangue = al_load_bitmap("sangue.png");
	hero_esq = al_load_bitmap("hero_esq.png");
	hero_bax = al_load_bitmap("hero_bax.png");

	heroPIC = hero_dir;

	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

	//carrega os audios e suas instancias
	musica = al_load_sample("kaiowas.ogg");

	musicaInstance = al_create_sample_instance(musica);

	tiro = al_load_sample("escopeta.ogg");

	tiroInstance = al_create_sample_instance(tiro);

	murloc = al_load_sample("murloc.ogg");

	murlocInstance = al_create_sample_instance(murloc);

	loktar = al_load_sample("loktar.ogg");

	loktarInstance  = al_create_sample_instance(loktar);

	levelup = al_load_sample("levelup.ogg");

	levelupInstance  = al_create_sample_instance(levelup);

	walk = al_load_sample("walk.ogg");

	walkInstance = al_create_sample_instance(walk);
	
	freedom = al_load_sample("freedom.ogg");

	freedomInstance = al_create_sample_instance(freedom);

	al_attach_sample_instance_to_mixer(musicaInstance, al_get_default_mixer());

	al_attach_sample_instance_to_mixer(tiroInstance, al_get_default_mixer());

	al_attach_sample_instance_to_mixer(murlocInstance, al_get_default_mixer());

	al_attach_sample_instance_to_mixer(loktarInstance, al_get_default_mixer());

	al_attach_sample_instance_to_mixer(levelupInstance, al_get_default_mixer());

	al_attach_sample_instance_to_mixer(walkInstance, al_get_default_mixer());
	
	al_attach_sample_instance_to_mixer(freedomInstance, al_get_default_mixer());

	al_set_sample_instance_playmode(musicaInstance, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(musicaInstance, 0.8);

	al_set_sample_instance_playmode(tiroInstance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(tiroInstance, 0.8);

	al_set_sample_instance_playmode(murlocInstance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(murlocInstance, 0.8);

	al_set_sample_instance_playmode(loktarInstance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(loktarInstance, 0.8);

	al_set_sample_instance_playmode(levelupInstance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(levelupInstance, 0.8);

	al_set_sample_instance_playmode(walkInstance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(walkInstance, 0.8);
	
	al_set_sample_instance_playmode(freedomInstance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(freedomInstance, 0.8);


	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());


	//inicia o temporizador
	al_start_timer(timer);

	int direcao = 0;

	int ganhou = 0, perdeu = 0;

	int playing = 1;
	int modo_jogo = NAVEGACAO;

	T_Hero hero;

	initHero(&hero, heroPIC);

	int turno_do_heroi = 1;

	int hero_pode_atacar = 1;

	int just_killed = 0;

	int just_runned = 0;

	int numero_monstros = 22;

	int indice_monstro_encontrado = -1;

	T_Monster vetorMonstros[numero_monstros];

	initMonster(vetorMonstros, numero_monstros);

	T_Bullet bullet_normal;

	initBullet_normal(&bullet_normal);

	int para_desenhar_bullet_normal = 0;

	T_Bullet bullet_especial;

	initBullet_especial(&bullet_especial);

	int para_desenhar_bullet_especial = 0;

	T_Bullet bullet_monstro;

	initBullet_monstro(&bullet_monstro);

	initGlobais();

	while(playing) {

		ALLEGRO_EVENT ev;

		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);


		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {


			if (hero.HP <= 0){
					perdeu = 1;
					playing = 0;
				}

			if (modo_jogo == NAVEGACAO){
				draw_scenario_NAVEGACAO(background3);
				draw_Hero_NAVEGACAO(hero, heroPIC);
				drawHP_NAVEGACAO(&hero, sangue);

				if (detectouMonstro(hero, vetorMonstros, numero_monstros, &indice_monstro_encontrado)){
					modo_jogo = BATALHA;
					just_killed = 0;
					just_runned = 0;
				}

				if(chegouObjetivo(hero, heroPIC)){
					playing=0;
					ganhou = 1;
				}

			}
			else { // i. e. estamos no modo batalha


				draw_scenario_BATALHA(madeira, &hero);
				draw_hero_BATALHA(&hero, hero3);
				draw_Monster_BATALHA(vetorMonstros, indice_monstro_encontrado, monster1, monster2);

				if (turno_do_heroi == 1){
					modo_jogo = processaAcaoHero(&hero, &para_desenhar_bullet_normal, &para_desenhar_bullet_especial, vetorMonstros, &indice_monstro_encontrado, &turno_do_heroi, &hero_pode_atacar, &just_killed, &just_runned);
				}

				if (para_desenhar_bullet_normal == 1){       //&& turno_do_heroi == 1){
					al_play_sample_instance(tiroInstance);
					drawBULLET_hero(&bullet_normal);

					if (bullet_normal.x - bullet_normal.tam < monster_battle_x + monster_battle_radius ){

						vetorMonstros[indice_monstro_encontrado].HP -= 35;

						if (vetorMonstros[indice_monstro_encontrado].HP <= 0){
							vetorMonstros[indice_monstro_encontrado].x = -100;
							vetorMonstros[indice_monstro_encontrado].y = -100;
							pontos+=1;
							al_play_sample_instance(levelupInstance);
							if (hero.HP <= 90 && hero.HP > 0)
								hero.HP += 10;
							else
								hero.HP = 100;

							turno_do_heroi = 0;
							modo_jogo = NAVEGACAO;
							
							printf("\nHP hero: %d", hero.HP);
							
							just_killed = 1;
						}

						para_desenhar_bullet_normal = 0;
						initBullet_normal(&bullet_normal);
						turno_do_heroi = 0;

					}
				}

				if (para_desenhar_bullet_especial == 1){     //&& turno_do_heroi == 1){

					al_play_sample_instance(tiroInstance);
					drawBULLET_hero(&bullet_especial);
					if (bullet_especial.x - bullet_especial.tam < monster_battle_x + monster_battle_radius){

						int quanto_tirar = 0;
						quanto_tirar = randInt(20,65);
						vetorMonstros[indice_monstro_encontrado].HP -= quanto_tirar;

						if (vetorMonstros[indice_monstro_encontrado].HP <= 0){
							vetorMonstros[indice_monstro_encontrado].x = -100;
							vetorMonstros[indice_monstro_encontrado].y = -100;
							pontos+=1;
							al_play_sample_instance(levelupInstance);
							if (hero.HP <= 90 && hero.HP > 0)
								hero.HP += 10;
							else
								hero.HP = 100;

							turno_do_heroi = 0;
							modo_jogo = NAVEGACAO;
							
							printf("\nHP hero: %d", hero.HP);
							
							just_killed = 1;
						}

						para_desenhar_bullet_especial = 0;
						initBullet_especial(&bullet_especial);
						turno_do_heroi = 0;
					}
				}

				if (turno_do_heroi == 0){
					if (just_killed == 0 && just_runned == 0){
						if (vetorMonstros[indice_monstro_encontrado].tipo == FRACO)
							al_play_sample_instance(murlocInstance);
						else if (vetorMonstros[indice_monstro_encontrado].tipo == FORTE)
							al_play_sample_instance(loktarInstance);
						drawBULLET_monstro(&bullet_monstro, vetorMonstros, indice_monstro_encontrado);
					}
					if (bullet_monstro.x + bullet_monstro.tam > hero_battle_x_up_left){
						ataqueMonstro(&hero, vetorMonstros, indice_monstro_encontrado);
						initBullet_monstro(&bullet_monstro);
						turno_do_heroi = 1;

						hero_pode_atacar = 1;
						
						printf("\nHP hero: %d", hero.HP);
					}
				}
			}



			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();


		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um clique de mouse
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y);
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			//printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			if (modo_jogo == NAVEGACAO){
				processaTeclaNaveg(&hero,  ev.keyboard.keycode , heroPIC, &direcao);
				if (direcao == DIREITA || direcao == 0)
					heroPIC = hero_dir;
				else if (direcao == ESQUERDA)
					heroPIC = hero_esq;
				else if (direcao == BAIXO)
					heroPIC = hero_bax;
				else if (direcao == CIMA)
					heroPIC = hero_dir;
			}
			else{

				processaTeclaBat(&hero, ev.keyboard.keycode);

			}
		}

	} //fim do while

	FILE *f_records_rd = fopen("records.txt", "r");

	if (f_records_rd == NULL){
		printf("\nErro ao abrir arquivo !");
		}

	int eh_record = 1;
	
	int maior = 0;

	if (ganhou == 1){
		fprintf(f_records_app, " %d ", pontos);

		int valor_posicao = -1;

		while (1){
			fscanf(f_records_rd,"%d", &valor_posicao);
			printf("\n\nLevel obtido: %d", valor_posicao);
			if (valor_posicao >= maior)
				maior = valor_posicao;
			if (pontos <= valor_posicao){
				eh_record = 0;
			}
			if (feof(f_records_rd))
				break;
		}
	}

	//printf("\n\n %d \n\n", eh_record);


	ALLEGRO_FONT *size_32 = al_load_font("symtext.ttf", 32, 1);
	//printf("\n\n%d", pontos);
	char my_text2[100];
	sprintf(my_text2, "Chegou ! Level: %d ", pontos);
	char my_text3[100];
	sprintf(my_text3, "NOVO RECORD !!!");
	char my_text4[100];
	sprintf(my_text4, "RECORD ATUAL: %d", maior);
	//printf("\n\n%d", pontos);
	al_clear_to_color(al_map_rgb(0,0,0));

	if (ganhou == 1){
		if (eh_record == 1){
			al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text2);
			al_draw_text(size_32, al_map_rgb(255, 0, 0), SCREEN_W/3 , SCREEN_H/2 - (SCREEN_H/2) * 2/10, 0, my_text3);
		}
		else{
			al_draw_text(size_32, al_map_rgb(255, 0, 0), SCREEN_W/3 , SCREEN_H/2 - (SCREEN_H/2) * 2/10, 0, my_text4);
			al_draw_text(size_32, al_map_rgb(0, 200, 30), SCREEN_W/3, SCREEN_H/2, 0, my_text2);
		}
	}
	if (perdeu == 1){
		al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, "Perdeu :(");
	}
	al_flip_display();
    al_rest(4);


	fclose(f_records_app);
	fclose(f_records_rd);

	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)

	al_destroy_sample(musica); // "destruir" musica
	al_destroy_sample_instance(musicaInstance);
	al_destroy_sample(tiro);
	al_destroy_sample_instance(tiroInstance);
	al_destroy_sample(murloc);
	al_destroy_sample_instance(murlocInstance);
	al_destroy_sample(loktar);
	al_destroy_sample_instance(loktarInstance);
	al_destroy_sample(walk);
	al_destroy_sample_instance(walkInstance);
	al_destroy_sample(freedom);
	al_destroy_sample_instance(freedomInstance);  
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);


	return 0;
}