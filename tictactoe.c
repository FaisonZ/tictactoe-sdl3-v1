#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint32_t current_player = 1;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define BOARD_WIDTH 400
#define BOARD_LINE_WIDTH 10
#define SQUARE_WIDTH 120

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

    SDL_FRect squares[9];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            squares[3*i + j].x = (float) board_rect.x + BOARD_LINE_WIDTH + j * (SQUARE_WIDTH + BOARD_LINE_WIDTH);
            squares[3*i + j].y = (float) board_rect.y + BOARD_LINE_WIDTH + i * (SQUARE_WIDTH + BOARD_LINE_WIDTH);
            squares[3*i + j].w = (float) SQUARE_WIDTH;
            squares[3*i + j].h = (float) SQUARE_WIDTH;
        }
    }

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &board_rect);
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, squares, 9);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}

