#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define PIECE_NONE 0
#define PIECE_O 1
#define PIECE_X 1<<1

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define BOARD_WIDTH 400
#define BOARD_LINE_WIDTH 10
#define SQUARE_WIDTH 120
#define PIECE_WIDTH 100

#define GAME_IN_PROGRESS 0
#define GAME_END_PLAYER_1 1
#define GAME_END_PLAYER_2 2
#define GAME_END_DRAW 4

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint32_t current_player = 1;
static uint8_t placements[9];
static uint32_t gameState = GAME_IN_PROGRESS;
static SDL_FRect board[10];

void clearPlacements()
{
    for (int i = 0; i < 9; i++) {
        placements[i] = PIECE_NONE;
    }
}

int createPlacementRects(SDL_FRect *rects)
{
    float board_left = ((float) (WINDOW_WIDTH - BOARD_WIDTH) / 2.0f) + BOARD_LINE_WIDTH;
    float board_top = ((float) (WINDOW_HEIGHT - 16.0f - BOARD_WIDTH)) + BOARD_LINE_WIDTH;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rects[3*i + j].x = (float) board_left + j * (SQUARE_WIDTH + BOARD_LINE_WIDTH);
            rects[3*i + j].y = (float) board_top + i * (SQUARE_WIDTH + BOARD_LINE_WIDTH);
            rects[3*i + j].w = (float) SQUARE_WIDTH;
            rects[3*i + j].h = (float) SQUARE_WIDTH;
        }
    }

    return 9;
}

void createBoard(SDL_FRect *rects)
{
    rects[0].x = (float) (WINDOW_WIDTH - BOARD_WIDTH) / 2;
    rects[0].y = (float) WINDOW_HEIGHT - 16.0f - BOARD_WIDTH;
    rects[0].w = (float) BOARD_WIDTH;
    rects[0].h = (float) BOARD_WIDTH;

    float board_left = ((float) (WINDOW_WIDTH - BOARD_WIDTH) / 2.0f) + BOARD_LINE_WIDTH;
    float board_top = ((float) (WINDOW_HEIGHT - 16.0f - BOARD_WIDTH)) + BOARD_LINE_WIDTH;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rects[1 + 3*i + j].x = (float) board_left + j * (SQUARE_WIDTH + BOARD_LINE_WIDTH);
            rects[1 + 3*i + j].y = (float) board_top + i * (SQUARE_WIDTH + BOARD_LINE_WIDTH);
            rects[1 + 3*i + j].w = (float) SQUARE_WIDTH;
            rects[1 + 3*i + j].h = (float) SQUARE_WIDTH;
        }
    }
}

int getGameState()
{
    int totalPlacements = 0;
    // Check horizontals
    for (int i = 0; i < 3; i++) {
        int result = placements[3*i] & placements[3*i+1] & placements[3*i+2];
        SDL_Log("Result for row %" SDL_PRIu32 ": %" SDL_PRIu32, i, result);

        if (result == PIECE_O) {
            return GAME_END_PLAYER_1;
        } else if (result == PIECE_X) {
            return GAME_END_PLAYER_2;
        }
    }

    // Check verticals
    for (int i = 0; i < 3; i++) {
        int result = placements[i] & placements[i + 3] & placements[i + 6];
        SDL_Log("Result for col %" SDL_PRIu32 ": %" SDL_PRIu32, i, result);

        if (result == PIECE_O) {
            return GAME_END_PLAYER_1;
        } else if (result == PIECE_X) {
            return GAME_END_PLAYER_2;
        }
    }

    // Check diagnols
    int backDiag = placements[0] & placements[4] & placements[8];
    int forwardDiag = placements[2] & placements[4] & placements[6];
    if (backDiag == PIECE_O) {
        return GAME_END_PLAYER_1;
    } else if (backDiag == PIECE_X) {
        return GAME_END_PLAYER_2;
    }
    if (forwardDiag == PIECE_O) {
        return GAME_END_PLAYER_1;
    } else if (forwardDiag == PIECE_X) {
        return GAME_END_PLAYER_2;
    }

    // Check draw
    for(int i = 0; i < 9; i++) {
        if (placements[i] != PIECE_NONE) {
            totalPlacements += 1;
        }
    }
    if (totalPlacements == 9) {
        return GAME_END_DRAW;
    }

    return GAME_IN_PROGRESS;
}

int getPlacementAt(float x, float y)
{
    if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
        return -1;
    }

    for (int i = 1; i < 10; i++) {
        SDL_FRect square = board[i];
        if (x >= square.x && x <= square.x + square.w && y >= square.y && y <= square.y + square.h) {
            return i - 1;
        }
    }

    return -1;
}

void handlePlacementClick(SDL_MouseButtonEvent *mEvent) {
    if (mEvent->button != SDL_BUTTON_LEFT) {
        return;
    }

    int clickedSquare = getPlacementAt(mEvent->x, mEvent->y);
    SDL_Log("Left button click at %f, %f", mEvent->x, mEvent->y);
    SDL_Log("Square clicked: %" SDL_PRIs32, clickedSquare);

    if (clickedSquare != -1 && placements[clickedSquare] == PIECE_NONE) {
        if (current_player == 1) {
            placements[clickedSquare] = PIECE_O;
        } else {
            placements[clickedSquare] = PIECE_X;
        }

        gameState = getGameState();
        if (gameState == GAME_IN_PROGRESS) {
            current_player = current_player == 1 ? 2 : 1;
        }

        SDL_Log("Game State: %" SDL_PRIs32, gameState);
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

    createBoard(board);
    clearPlacements();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        SDL_MouseButtonEvent *mEvent = (SDL_MouseButtonEvent*) event;

        if (gameState == GAME_IN_PROGRESS) {
            handlePlacementClick(mEvent);
        } else {
            clearPlacements();
            current_player = 1;
            gameState = GAME_IN_PROGRESS;
        }
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
    if (gameState == GAME_IN_PROGRESS) {
        SDL_RenderDebugTextFormat(renderer, (float) ((WINDOW_WIDTH / 2.0f - (charsize * 8)) / 2), 5 + 1.5f * charsize, "Player %" SDL_PRIu32, current_player );
    } else if (gameState == GAME_END_DRAW) {
        SDL_RenderDebugTextFormat(renderer, (float) ((WINDOW_WIDTH / 2.0f - (charsize * 4)) / 2), 5 + 1.5f * charsize, "Draw");
    } else {
        SDL_RenderDebugTextFormat(renderer, (float) ((WINDOW_WIDTH / 2.0f - (charsize * 14)) / 2), 5 + 1.5f * charsize, "Player %" SDL_PRIu32 " wins!", current_player );
    }

    SDL_SetRenderScale(renderer, 1.0f, 1.0f);

    // Render game board
    // First a big square
    // Then squares to carve out the spots
    // screen: 500 ; board: 300; board placement: 100 - 400; x = (500 - 300)/2
    float board_left = board[0].x + BOARD_LINE_WIDTH;
    float board_top = board[0].y + BOARD_LINE_WIDTH;

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &board[0]);
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, board + 1, 9);


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

