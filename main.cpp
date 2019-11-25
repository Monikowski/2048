#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768



struct gamestate {
	int number;
	int occupied;
	int x;
	int y;
};

struct coord {
	int x;
	int y;
	int number;
	int source;
};

struct offs {
	int x;
	int y;
};

gamestate board[6][6];
gamestate boardold[6][6];
coord target[6][6];
offs offset[6][6];

void fillGamestate(gamestate board[6][6], int size) {
	if (size == 5) {
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 6; j++) {
				board[i][j].x = (SCREEN_WIDTH) / 2 + ((i - 2) * 100);
				board[i][j].y = (SCREEN_HEIGHT / 5) + 230 + (j - 2) * 100;
				board[i][j].occupied = 0;
				board[i][j].number = 0;
			}
		}
	}
	else if (size == 4) {
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 6; j++) {
				board[i][j].x = (SCREEN_WIDTH) / 2 + (i - 2) * 100 + 49;
				board[i][j].y = (SCREEN_HEIGHT / 5) + 279 + (j - 2) * 100 ;
				board[i][j].occupied = 0;
				board[i][j].number = 0;
			}
		}
	}
	else if (size == 6) {
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 6; j++) {
				board[i][j].x = (SCREEN_WIDTH) / 2 + (i - 2) * 100 - 50;
				board[i][j].y = (SCREEN_HEIGHT / 5) + 180 + (j - 2) * 100;
				board[i][j].occupied = 0;
				board[i][j].number = 0;
			}
		}
	}
}

// narysowanie napisu txt na powierzchni screen, zaczynajπc od punktu (x, y)
// charset to bitmapa 128x128 zawierajπca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)

void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.h = sprite->h;
	dest.w = sprite->w;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
// bπdü poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

//spawning a tile in a free space
void spawnTile(gamestate board[6][6], int size) {
	srand(time(NULL));
	while (true) {
		int x = rand() % size;
		int y = rand() % size;
		if (board[x][y].occupied == 0) {
			board[x][y].occupied = 1;
			board[x][y].number = 2;
			break;
		}
	}
}

void drawTiles(gamestate board[6][6], gamestate boardold[6][6], SDL_Surface *screen, SDL_Surface *middle, offs offset[6][6], char bmpname[30], int size, char middlename[30]) {
	sprintf(middlename, "./bitmaps/middle%d.bmp", size);
	middle = SDL_LoadBMP(middlename);
	DrawSurface(screen, middle, (SCREEN_WIDTH) / 2, (SCREEN_HEIGHT / 5) + 230);
	SDL_FreeSurface(middle);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (boardold[i][j].occupied == 1 && offset[i][j].x == 0 && offset[i][j].y == 0) {
				sprintf(bmpname, "./bitmaps/tile%d.bmp", boardold[i][j].number);
				SDL_Surface *tiletodisplay;
				tiletodisplay = SDL_LoadBMP(bmpname);
				DrawSurface(screen, tiletodisplay, board[i][j].x, board[i][j].y);
				SDL_FreeSurface(tiletodisplay);
			}
		}
	}
}

void drawTilesall(gamestate board[6][6], gamestate boardold[6][6], SDL_Surface *screen, SDL_Surface *middle, offs offset[6][6], char bmpname[30], int size, char middlename[30]) {
	sprintf(middlename, "./bitmaps/middle%d.bmp", size);
	middle = SDL_LoadBMP(middlename);
	DrawSurface(screen, middle, (SCREEN_WIDTH) / 2, (SCREEN_HEIGHT / 5) + 230);
	SDL_FreeSurface(middle);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (board[i][j].occupied == 1) {
				sprintf(bmpname, "./bitmaps/tile%d.bmp", board[i][j].number);
				SDL_Surface *tiletodisplay;
				tiletodisplay = SDL_LoadBMP(bmpname);
				DrawSurface(screen, tiletodisplay, board[i][j].x, board[i][j].y);
				SDL_FreeSurface(tiletodisplay);
			}
		}
	}
}

void moveTiles(gamestate board[6][6], gamestate boardold[6][6], coord target[6][6], offs offset[6][6], int insidesmall, int czarny, SDL_Surface *screen, SDL_Surface *middle, int direction, int *points, int size, char bmpname[30], SDL_Texture *scrtex, SDL_Renderer *renderer, char middlename[30]) {
	int move = 0;
	for (int i = 0; i < size; i++) {
		for (int z = 0; z < size; z++) {
			offset[i][z].x = 0;
			offset[i][z].y = 0;
			target[i][z].source = 0;
			target[i][z].number = 0;
		}
	}
	if (direction == 1) { //up
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (board[i][j].occupied == 0) {
					for (int z = j; z < size; z++) {
						if (board[i][z].occupied == 1) {
							board[i][z].occupied = 0;
							board[i][j].occupied = 1;
							board[i][j].number = board[i][z].number;
							board[i][z].number = 0;

							target[i][z].y = board[i][j].y;
							target[i][z].x = board[i][j].x;
							offset[i][z].y = board[i][z].y - target[i][z].y;
							offset[i][z].x = board[i][z].x - target[i][z].x;
							target[i][z].number = board[i][j].number;
							target[i][z].source = target[i][z].number;
						
							move = 1;
							break;
						}
					}
				}

				if (board[i][j].occupied == 1) {
					for (int z = j + 1; z < size; z++) {
						if (board[i][z].occupied == 1) {
							if (board[i][z].number == board[i][j].number) {
								board[i][j].number = board[i][j].number * 2;
								board[i][z].occupied = 0;
								board[i][z].number = 0;

								target[i][z].y = board[i][j].y;
								target[i][z].x = board[i][j].x;
								offset[i][z].y = board[i][z].y - target[i][z].y;
								offset[i][z].x = board[i][z].x - target[i][z].x;
								target[i][z].number = board[i][j].number;
								target[i][z].source = target[i][z].number / 2;

								*points = *points + board[i][j].number;
								move = 1;
								break;
							}
							else break;
						}
					}
				}

			}
		}
	}
	else if (direction == 2) { //down
		for (int i = size - 1; i >= 0; i--) {
			for (int j = size - 1; j >= 0; j--) {
				if (board[i][j].occupied == 0) {
					for (int z = j; z >= 0; z--) {
						if (board[i][z].occupied == 1) {
							board[i][z].occupied = 0;
							board[i][j].occupied = 1;
							board[i][j].number = board[i][z].number;
							board[i][z].number = 0;

							target[i][z].y = board[i][j].y;
							target[i][z].x = board[i][j].x;
							offset[i][z].y = board[i][z].y - target[i][z].y;
							offset[i][z].x = board[i][z].x - target[i][z].x;
							target[i][z].number = board[i][j].number;
							target[i][z].source = target[i][z].number;

							move = 1;
							break;
						}
					}
				}

				if (board[i][j].occupied == 1) {
					for (int z = j - 1; z >= 0; z--) {
						if (board[i][z].occupied == 1) {
							if (board[i][z].number == board[i][j].number) {
								board[i][j].number = board[i][j].number * 2;
								board[i][z].occupied = 0;
								board[i][z].number = 0;

								target[i][z].y = board[i][j].y;
								target[i][z].x = board[i][j].x;
								offset[i][z].y = board[i][z].y - target[i][z].y;
								offset[i][z].x = board[i][z].x - target[i][z].x;
								target[i][z].number = board[i][j].number;
								target[i][z].source = target[i][z].number / 2;

								*points = *points + board[i][j].number;
								move = 1;
								break;
							}
							else break;
						}
					}
				}
			}
		}
	}
	else if (direction == 3) { //left
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (board[j][i].occupied == 0) {
					for (int z = j; z < size; z++) {
						if (board[z][i].occupied == 1) {
							board[z][i].occupied = 0;
							board[j][i].occupied = 1;
							board[j][i].number = board[z][i].number;
							board[z][i].number = 0;

							target[z][i].y = board[j][i].y;
							target[z][i].x = board[j][i].x;
							offset[z][i].y = board[z][i].y - target[z][i].y;
							offset[z][i].x = board[z][i].x - target[z][i].x;
							target[z][i].number = board[j][i].number;
							target[z][i].source = target[z][i].number;

							move = 1;
							break;
						}
					}
				}

				if (board[j][i].occupied == 1) {
					for (int z = j + 1; z < size; z++) {
						if (board[z][i].occupied == 1) {
							if (board[z][i].number == board[j][i].number) {
								board[j][i].number = board[j][i].number * 2;
								board[z][i].occupied = 0;
								board[z][i].number = 0;

								target[z][i].y = board[j][i].y;
								target[z][i].x = board[j][i].x;
								offset[z][i].y = board[z][i].y - target[z][i].y;
								offset[z][i].x = board[z][i].x - target[z][i].x;
								target[z][i].number = board[j][i].number;
								target[z][i].source = target[z][i].number / 2;

								*points = *points + board[j][i].number;
								move = 1;
								break;
							}
							else break;
						}
					}
				}
			}
		}
	}
	else if (direction == 4) { //right
		for (int i = size - 1; i >= 0; i--) {
			for (int j = size - 1; j >= 0; j--) {
				if (board[j][i].occupied == 0) {
					for (int z = j; z >= 0; z--) {
						if (board[z][i].occupied == 1) {
							board[z][i].occupied = 0;
							board[j][i].occupied = 1;
							board[j][i].number = board[z][i].number;
							board[z][i].number = 0;

							target[z][i].y = board[j][i].y;
							target[z][i].x = board[j][i].x;
							offset[z][i].y = board[z][i].y - target[z][i].y;
							offset[z][i].x = board[z][i].x - target[z][i].x;
							target[z][i].number = board[j][i].number;
							target[z][i].source = target[z][i].number;

							move = 1;
							break;
						}
					}
				}
				if (board[j][i].occupied == 1) {
					for (int z = j - 1; z >= 0; z--) {
						if (board[z][i].occupied == 1) {
							if (board[z][i].number == board[j][i].number) {
								board[j][i].number = board[j][i].number * 2;
								board[z][i].occupied = 0;
								board[z][i].number = 0;

								target[z][i].y = board[j][i].y;
								target[z][i].x = board[j][i].x;
								offset[z][i].y = board[z][i].y - target[z][i].y;
								offset[z][i].x = board[z][i].x - target[z][i].x;
								target[z][i].number = board[j][i].number;
								target[z][i].source = target[z][i].number / 2;

								*points = *points + board[j][i].number;
								move = 1;
								break;
							}
							else break;
						}
					}
				}
			}
		}
	}
	for (int d = 0; d < 21; d++) {
		drawTiles(board, boardold, screen, middle, offset, bmpname, size, middlename);
	for (int i = 0; i < size; i++) {
		for (int z = 0; z < size; z++) {
			if (offset[i][z].x != 0 || offset[i][z].y != 0) {		
							sprintf(bmpname, "./bitmaps/tile%d.bmp", target[i][z].source);
							SDL_Surface *tiletodisplay;
							tiletodisplay = SDL_LoadBMP(bmpname);
							DrawSurface(screen, tiletodisplay, board[i][z].x - (offset[i][z].x / 20) * d, board[i][z].y - (offset[i][z].y / 20) * d);
							SDL_FreeSurface(tiletodisplay);
				}
			}
		}
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
	}
	if (move == 1) {
		spawnTile(board, size);
	}
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {

	int t1, t2, quit, frames, rc, points = 0, pointsold = 0, option = 0, enter = 0, size = 0, z = 0, d = 0, win = 0;
	int * pointsptr = &points;
	int * dptr = &d;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	SDL_Event event;
	SDL_Surface *screen, *charset, *middle = 0, *nonselect, *select;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;


	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe≥noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "2048 | Mateusz Monikowski 175641");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy≥πczenie widocznoúci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);
	
	nonselect = SDL_LoadBMP("./bitmaps/nonselect.bmp");
	select = SDL_LoadBMP("./bitmaps/select.bmp");

	char text[128], bmpname[30], middlename[30];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int inside = SDL_MapRGB(screen->format, 224, 209, 159);
	int insidesmall = SDL_MapRGB(screen->format, 204, 194, 163);
	int tile = SDL_MapRGB(screen->format, 232, 227, 213);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;


	while (!quit) {

		SDL_FillRect(screen, NULL, czarny);

		//background
		DrawRectangle(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, czarny, inside);

		//4x4, 5x5, 6x6 MENU/SIZE SELECTION
		while (enter == 0 && !quit) {
			for (int i = 0; i < 3; i++) {
				DrawSurface(screen, nonselect, (SCREEN_WIDTH) / 2, (SCREEN_HEIGHT / 5) + i * 130 + 100);
			}
			DrawSurface(screen, select, (SCREEN_WIDTH) / 2, (SCREEN_HEIGHT / 5) + (option % 3) * 130 + 100);
			sprintf(text, "4x4");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 5) + 100, text, charset);
			sprintf(text, "5x5");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 5) + 230, text, charset);
			sprintf(text, "6x6");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 5) + 360, text, charset);

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			//		SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						quit = 1;
						break;
					}
					else if (event.key.keysym.sym == SDLK_UP) {
						option = option - 1;
						if (option == -1) {
							option = 2;
						}
					}
					else if (event.key.keysym.sym == SDLK_DOWN) {
						option = option + 1;
						if (option == 3) {
							option = 0;
						}
					}
					else if (event.key.keysym.sym == SDLK_RETURN) {
						size = option + 4;
						fillGamestate(board, size);
						spawnTile(board, size);
						for (int i = 0; i < size; i++) {
							for (int j = 0; j < size; j++) {
								boardold[i][j].occupied = board[i][j].occupied;
								boardold[i][j].number = board[i][j].number;
							}
						}
						sprintf(middlename, "./bitmaps/middle%d.bmp", size);
						middle = SDL_LoadBMP(middlename);
						enter = 1;
					}
					break;
				case SDL_KEYUP:
					break;
				case SDL_QUIT:
					enter = 1;
					quit = 1;
					break;
				}
			}
			z = SDL_GetTicks();
		}
		if (enter == 1){

		t2 = SDL_GetTicks() - z;
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		worldTime = worldTime + delta;

		DrawSurface(screen, middle, (SCREEN_WIDTH) / 2, (SCREEN_HEIGHT / 5) + 230);
		drawTilesall(board, boardold, screen, middle, offset, bmpname, size, middlename);


		//header
		DrawRectangle(screen, 0, 0, SCREEN_WIDTH, 60, czarny, czarny);
		sprintf(text, "Mateusz Monikowski 175641");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 5, text, charset);
		sprintf(text, "Game time = %.1lf s", worldTime);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 25, text, charset);
		sprintf(text, "escape - exit | n - new game | u - undo move | arrows - move | points: %d", points);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 40, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		int counter = 0;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (board[i][j].number == 2048) {
					win = 1;
					break;
				}
				if (board[i][j].occupied == 1 && board[i][j].number != board[i + 1][j].number && board[i][j].number != board[i - 1][j].number && board[i][j].number != board[i][j + 1].number && board[i][j].number != board[i][j - 1].number) {
					counter++;
				}
			}
			if (win == 1) break;
		}
		while (counter == size * size || win == 1) {
			DrawSurface(screen, nonselect, (SCREEN_WIDTH) / 2, (SCREEN_HEIGHT / 5) + 100);
			if (counter == size * size) {
				sprintf(text, "You lost!");
			}
			else if (win == 1) {
				sprintf(text, "You won!");
			}
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 5) + 70, text, charset);
			sprintf(text, "Final score: %d", points);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 5) + 90, text, charset);
			sprintf(text, "| escape - exit |");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 5) + 100, text, charset);
			sprintf(text, "| n - new game |");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 5) + 110, text, charset);
			sprintf(text, "| u - undo move |");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 5) + 120, text, charset);

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						win = 0;
						counter = 0;
						quit = 1;
					}
					else if (event.key.keysym.sym == SDLK_n) {
						counter = 0;
						worldTime = 0;
						points = 0;
						pointsold = 0;
						fillGamestate(board, size);
						spawnTile(board, size);
						for (int i = 0; i < size; i++) {
							for (int j = 0; j < size; j++) {
								boardold[i][j].occupied = board[i][j].occupied;
								boardold[i][j].number = board[i][j].number;
							}
						}
					}
					else if (event.key.keysym.sym == SDLK_u) {
						win = 0;
						counter = counter - 1;
						points = pointsold;
						for (int i = 0; i < size; i++) {
							for (int j = 0; j < size; j++) {
								board[i][j].occupied = boardold[i][j].occupied;
								board[i][j].number = boardold[i][j].number;
							}
						}
					}

					break;
				case SDL_KEYUP:
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
		}

		//obs≥uga zdarzeÒ(o ile jakieú zasz≥y) / handling of events(if there were any)
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_UP) {
					for (int i = 0; i < size; i++) {
						for (int j = 0; j < size; j++) {
							boardold[i][j].occupied = board[i][j].occupied;
							boardold[i][j].number = board[i][j].number;
						}
					}
					pointsold = points;
					moveTiles(board, boardold, target, offset, insidesmall, czarny, screen, middle, 1, pointsptr, size, bmpname, scrtex, renderer, middlename);
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					for (int i = 0; i < size; i++) {
						for (int j = 0; j < size; j++) {
							boardold[i][j].occupied = board[i][j].occupied;
							boardold[i][j].number = board[i][j].number;
						}
					}
					pointsold = points;
					moveTiles(board, boardold, target, offset, insidesmall, czarny, screen, middle, 2, pointsptr, size, bmpname, scrtex, renderer, middlename);
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					for (int i = 0; i < size; i++) {
						for (int j = 0; j < size; j++) {
							boardold[i][j].occupied = board[i][j].occupied;
							boardold[i][j].number = board[i][j].number;
						}
					}
					pointsold = points;
					moveTiles(board, boardold, target, offset, insidesmall, czarny, screen, middle, 3, pointsptr, size, bmpname, scrtex, renderer, middlename);
				}
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					for (int i = 0; i < size; i++) {
						for (int j = 0; j < size; j++) {
							boardold[i][j].occupied = board[i][j].occupied;
							boardold[i][j].number = board[i][j].number;
						}
					}
					pointsold = points;
					moveTiles(board, boardold, target, offset, insidesmall, czarny, screen, middle, 4, pointsptr, size, bmpname, scrtex, renderer, middlename);
				}
				else if (event.key.keysym.sym == SDLK_n) {
					worldTime = 0;
					points = 0;
					pointsold = 0;
					fillGamestate(board, size);
					spawnTile(board, size);
					for (int i = 0; i < size; i++) {
						for (int j = 0; j < size; j++) {
							boardold[i][j].occupied = board[i][j].occupied;
							boardold[i][j].number = board[i][j].number;
						}
					}
				}
				else if (event.key.keysym.sym == SDLK_u) {
					points = pointsold;
					for (int i = 0; i < size; i++) {
						for (int j = 0; j < size; j++) {
							board[i][j].occupied = boardold[i][j].occupied;
							board[i][j].number = boardold[i][j].number;
						}
					}
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
	}
		};		
	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
