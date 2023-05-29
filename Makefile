ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: final_fantasy.exe

final_fantasy.exe: final_fantasy.o
	gcc -o final_fantasy.exe final_fantasy.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

final_fantasy.o: final_fantasy.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c final_fantasy.c	

clean:
	del final_fantasy.o
	del final_fantasy.exe

