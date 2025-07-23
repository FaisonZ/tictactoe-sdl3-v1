#include "SDL3/SDL_init.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define PIECE_NONE 0
#define PIECE_X 1
#define PIECE_O 1<<1

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define BOARD_WIDTH 400
#define BOARD_LINE_WIDTH 10
#define SQUARE_WIDTH 120
#define PIECE_WIDTH 100

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint32_t current_player = 1;
static uint8_t placements[9];

void clearPlacements()
{
    for (int i = 0; i < 9; i++) {
        placements[i] = PIECE_NONE;
    }
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Tic Tac Toe Project", "1.0", "net.faisonz.games.tictactoe");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Tic Tac Toe", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    clearPlacements();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Centered Title
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_SetRenderScale(renderer, 2.0f, 2.0f);
    SDL_RenderDebugText(renderer, (float) ((WINDOW_WIDTH / 2.0f - (charsize * 11)) / 2), 5, "Tic Tac Toe");
    SDL_SetRenderDrawColor(renderer, current_player == 1 ? 200 : 0, current_player == 1 ? 0 : 200, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugTextFormat(renderer, (float) ((WINDOW_WIDTH / 2.0f - (charsize * 8)) / 2), 5 + 1.5f * charsize, "Player %" SDL_PRIu32, current_player );
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);

    // Render game board
    // First a big square
    // Then squares to carve out the spots
    // screen: 500 ; board: 300; board placement: 100 - 400; x = (500 - 300)/2
    SDL_FRect board_rect = {
        .x = (float) (WINDOW_WIDTH - BOARD_WIDTH) / 2,
        .y = (float) WINDOW_HEIGHT - 16.0f - BOARD_WIDTH,
        .w = (float) BOARD_WIDTH,
        .h = (float) BOARD_WIDTH,
    };

    float board_left = board_rect.x + BOARD_LINE_WIDTH;
    float board_top = board_rect.y + BOARD_LINE_WIDTH;

    SDL_FRect squares[9];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            squares[3*i + j].x = (float) board_left + j * (SQUARE_WIDTH + BOARD_LINE_WIDTH);
            squares[3*i + j].y = (float) board_top + i * (SQUARE_WIDTH + BOARD_LINE_WIDTH);
            squares[3*i + j].w = (float) SQUARE_WIDTH;
            squares[3*i + j].h = (float) SQUARE_WIDTH;
        }
    }

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &board_rect);
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, squares, 9);


    // Draw pieces
    int oPiecesToDraw = 0;
    int xPiecesToDraw = 0;
    for (int i = 0; i < 9; i++) {
        if (placements[i] == PIECE_O) {
            oPiecesToDraw += 1;
        } else if (placements[i] == PIECE_X) {
            xPiecesToDraw += 1;
        }
    }
    float square_padding = (SQUARE_WIDTH - PIECE_WIDTH) / 2.0f;
    SDL_FRect oPieces[oPiecesToDraw];
    SDL_FRect xPieces[xPiecesToDraw];
    int oCurrIndex = 0;
    int xCurrIndex = 0;
    for (int i = 0; i < 9; i++) {
        if (placements[i] != PIECE_NONE) {
            SDL_FRect *piece;
            int row = i / 3;

            if (placements[i] == PIECE_O) {
                piece = &oPieces[oCurrIndex];
                oCurrIndex += 1;
            } else {
                piece = &xPieces[xCurrIndex];
                xCurrIndex += 1;
            }
            piece->x = board_left + square_padding + (i%3) * (SQUARE_WIDTH + square_padding);
            piece->y = board_top + square_padding + row * (SQUARE_WIDTH + square_padding);
            piece->w = PIECE_WIDTH;
            piece->h = PIECE_WIDTH;
        }
    }

    SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, oPieces, oPiecesToDraw);
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, xPieces, xPiecesToDraw);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}

