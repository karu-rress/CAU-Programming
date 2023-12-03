/*
* 
*   tictactoe.c
* 
*   경영경제대학 산업보안학과
*   20234748 나선우
* 
*/

#define _CRT_SECURE_NO_WARNINGS
#define USE_ARROW_KEY		// 좌표 입력 대신 방향키 사용
#define BOARD_SIZE 3		// board의 크기

#include <stdbool.h>		// bool, true, false
#include <stdio.h>
#include <stdlib.h>
#include <time.h>			// time()
#include <Windows.h>		// Sleep()
#ifdef USE_ARROW_KEY
#include <conio.h>			// _getch()
#endif

// 셀의 표시(' ', X, O)를 상수로 정의
typedef enum {
	NONE		= ' ',
	PLAYER		= 'X',
	COMPUTER	= 'O',
#ifdef USE_ARROW_KEY
	CURSOR		= '*',
#endif
} Cell, Winner;

// 보드판 타입 정의
typedef Cell board_t[BOARD_SIZE][BOARD_SIZE];

// 함수 선언
void initialize_board(board_t board);
void display_board(board_t board);
Winner get_player_move(board_t board, bool *print_table);
Winner get_computer_move(board_t board, bool *print_table);
bool is_finished(board_t board, Winner* win);
#ifdef USE_ARROW_KEY
void get_cursor_location(board_t board, int *x_pos, int *y_pos);
void move_cursor(board_t board, int *x, int *y, int new_x, int new_y);
#endif

int main(void) {
	board_t board;
	Winner winner = NONE;
	int i;
	bool print_table = true;

	srand((unsigned int)time(NULL));
	initialize_board(board);

	// 게임 종료 전까지 무한 반복
	for (i = 1; !is_finished(board, &winner); i++) {
		display_board(board);

		winner = (i % 2 == 1)	// 홀수번째: 플레이어, 짝수번째: 컴퓨터
			? get_player_move(board, &print_table)
			: get_computer_move(board, &print_table);
	}

	// 게임 종료 화면
	if (print_table)
		display_board(board);

	switch (winner) {
	case PLAYER:
		printf("축하드립니다. 승리하셨습니다.\n");
		break;
	case COMPUTER:
		printf("컴퓨터가 승리하였습니다.\n");
		break;
	default:
		printf("무승부입니다. 게임을 종료합니다.\n");
		break;
	}

	return 0;
}

/// <summary>
/// 보드판을 초기화하는 함수
/// </summary>
void initialize_board(board_t board) {
	int x, y;
	for (x = 0; x < BOARD_SIZE; x++)
		for (y = 0; y < BOARD_SIZE; y++)
			board[x][y] = NONE;
}

/// <summary>
/// 보드판을 출력하는 함수
/// </summary>
void display_board(board_t board) {
	int i, j;
	// 화면 지우기
	system("cls");

	// 상단
	printf("┏");
	for (i = 0; i < BOARD_SIZE - 1; i++)
		printf("━━━┳");
	printf("━━━┓\n");

	// 중앙
	for (i = 0; i < BOARD_SIZE - 1; i++) {
		printf("┃");
		for (j = 0; j < BOARD_SIZE; j++)
			printf(" %c ┃", board[i][j]);
		printf("\n┣━━━╋━━━╋━━━┫\n");
	}
	printf("┃");
	for (i = 0; i < BOARD_SIZE; i++)
		printf(" %c ┃", board[BOARD_SIZE - 1][i]);

	// 하단
	printf("\n┗");
	for (i = 0; i < BOARD_SIZE - 1; i++)
		printf("━━━┻");
	printf("━━━┛\n\n");
}

#ifdef USE_ARROW_KEY
/// <summary>
/// 커서를 초기화한 후 위치를 가져오는 함수
/// </summary>
/// <param name="x_pos">x 좌표를 저장할 포인터</param>
/// <param name="y_pos">y 좌표를 저장할 포인터</param>
void get_cursor_location(board_t board, int *x_pos, int *y_pos) {
	int x, y;

	for (x = 0; x < BOARD_SIZE; x++)
		for (y = 0; y < BOARD_SIZE; y++)
			if (board[x][y] == NONE) {
				board[x][y] = CURSOR;
				*x_pos = x;
				*y_pos = y;
				return;
			}
}

/// <summary>
/// 커서의 위치를 옮기는 함수
/// </summary>
/// <param name="x">현재 x값을 가지고 있는 변수의 주소</param>
/// <param name="y">현재 y값을 가지고 있는 변수의 주소</param>
/// <param name="new_x">새 x값의 좌표</param>
/// <param name="new_y">새 y값의 좌표</param>
void move_cursor(board_t board, int *x, int *y, int new_x, int new_y) {
	board[*x][*y] = NONE;
	*x = new_x;
	*y = new_y;
	board[new_x][new_y] = CURSOR;
}

/// <summary>
/// 사용자의 입력을 받아 보드판에 나타내는 함수
/// </summary>
/// <returns>기권한 경우 -1, 아닌 경우 0</returns>
Winner get_player_move(board_t board, bool *print_table) {
	int x, y, i, buf;

	get_cursor_location(board, &x, &y);
	for (;;) {
		display_board(board);
		printf("플레이어의 차례입니다.\n");
		printf("방향키로 위치를 선택한 후, 엔터 키를 누르세요.\n");
		printf("기권하려면 ESC 키를 누르세요.");

		buf = _getch();
		if (buf == 27) {		// ESC
			*print_table = false;
			printf("\n\n기권하였습니다.\n");
			return COMPUTER;
		}
		if (buf == 13) {		// Enter / Return
			board[x][y] = PLAYER;
			return NONE;
		}

		if (buf == 0 || buf == 224) { // Arrow Key
			switch (_getch()) {
			case 72: // UP
				// 위쪽으로 빈 칸이 있는지 탐색
				for (i = x - 1; i >= 0; i--)
					if (board[i][y] == NONE) break;
				// 있다면 해당 좌표로 커서를 옮김
				if (i >= 0) 
					move_cursor(board, &x, &y, i, y);
				// 없으면 바로 윗줄부터 시작해서 빈 공간이 있는지 탐색
				else if (x != 0) {
					if (x == 2) {
						for (i = 0; i < BOARD_SIZE; i++)
							if (board[1][i] == NONE) break;
						if (i < BOARD_SIZE) {
							move_cursor(board, &x, &y, 1, i);
							break;
						}
					}
					for (i = 0; i < BOARD_SIZE; i++)
						if (board[0][i] == NONE) break;
					if (i < BOARD_SIZE)
						move_cursor(board, &x, &y, 0, i);
				}
				break;
			case 80: // DOWN
				for (i = x + 1; i < BOARD_SIZE; i++)
					if (board[i][y] == NONE) break;
				if (i < BOARD_SIZE) 
					move_cursor(board, &x, &y, i, y);
				else if (x != 2) {
					if (x == 0) {
						for (i = 0; i < BOARD_SIZE; i++)
							if (board[1][i] == NONE) break;
						if (i < BOARD_SIZE) {
							move_cursor(board, &x, &y, 1, i);
							break;
						}
					}
					for (i = 0; i < BOARD_SIZE; i++)
						if (board[2][i] == NONE) break;
					if (i < BOARD_SIZE)
						move_cursor(board, &x, &y, 2, i);
				}
				break;
			case 75: // LEFT
				for (i = y - 1; i >= 0; i--)
					if (board[x][i] == NONE) break;
				if (i >= 0)
					move_cursor(board, &x, &y, x, i);
				else if (y != 0) {
					if (y == 2) {
						for (i = 0; i < BOARD_SIZE; i++)
							if (board[i][1] == NONE) break;
						if (i < BOARD_SIZE) {
							move_cursor(board, &x, &y, i, 1);
							break;
						}
					}
					for (i = 0; i < BOARD_SIZE; i++)
						if (board[i][0] == NONE) break;
					if (i < BOARD_SIZE)
						move_cursor(board, &x, &y, i, 0);
				}
				break;
			case 77: // RIGHT
				for (i = y + 1; i < BOARD_SIZE; i++)
					if (board[x][i] == NONE) break;
				if (i < BOARD_SIZE)
					move_cursor(board, &x, &y, x, i);
				else if (y != 0) {
					if (y == 0) {
						for (i = 0; i < BOARD_SIZE; i++)
							if (board[i][1] == NONE) break;
						if (i < BOARD_SIZE) {
							move_cursor(board, &x, &y, i, 1);
							break;
						}
					}
					for (i = 0; i < BOARD_SIZE; i++)
						if (board[i][2] == NONE) break;
					if (i < BOARD_SIZE)
						move_cursor(board, &x, &y, i, 2);
				}
				break;
			}
		}
	}
}
#else
/// <summary>
/// 사용자의 입력을 받아 보드판에 나타내는 함수
/// </summary>
/// <returns>기권한 경우 -1, 아닌 경우 0</returns>
Winner get_player_move(board_t board, bool *print_table) {
	int x, y;

	printf("플레이어의 차례입니다.\n");
	printf("x, y 좌표를 쉼표로 구분하여 입력하세요.\n");
	printf("기권하려면 -1, -1을 입력하세요. ▶▷ _, _\b\b\b\b");

	for (;;) {
		scanf("%d, %d", &x, &y);

		if (x == -1 && y == -1) {				// 항복한 경우
			*print_table = false;
			printf("\n기권하였습니다.\n");
			return COMPUTER;
		}

		if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) // 범위 오류
			printf("\n좌표가 범위를 벗어났습니다. (0~2)\n");
		else if (board[x][y] != NONE)			// 이미 칠해진 칸
			printf("\n이미 칠해진 칸입니다.\n");
		else {									// 정상적인 입력
			board[x][y] = PLAYER;
			return NONE;
		}
		printf("다시 입력하세요. ▶▷ _, _\b\b\b\b");
	}
}
#endif

/// <summary>
/// 랜덤으로 좌표를 결정하여 보드판에 나타내는 함수
/// </summary>
/// <returns>컴퓨터가 기권한 경우 -1, 아닌 경우 0</returns>
Winner get_computer_move(board_t board, bool *print_table) {
	const int MAX_ITER = 10000000;
	int x, y, i;
	
	/* 직접 테스트한 결과 1억번 루프 반복에 3.96초 소요.
	 * 1천만번은 약 0.4초이므로, 이 값을 MAX_ITER로 설정
	 * 이 반복 횟수 안에 비어있는 적절한 칸을 찾으면
	 * 해당 칸에 컴퓨터의 말을 놓고,
	 * 그렇지 않은 경우 컴퓨터의 기권(시간초과)으로 처리.
	 * (무한반복을 방지하기 위한 조치)
	 */
	printf("컴퓨터의 선택을 기다리고 있습니다");
	for (i = 0; i < MAX_ITER; i++) {
		x = rand() % BOARD_SIZE;
		y = rand() % BOARD_SIZE;

		// 랜덤으로 뽑은 칸이 빈칸일 때까지 반복
		if (board[x][y] != NONE)
			continue;

		for (i = 0; i < 3; i++) {
			Sleep(500);
			putchar('.');
		}
		Sleep(500);

		board[x][y] = COMPUTER;
		return NONE;
	}
	// 시간이 초과된 경우 (반복 횟수 초과) 기권 처리
	printf("컴퓨터가 기권했습니다.");
	*print_table = false;
	return PLAYER;
}

/// <summary>
/// 게임이 종료되었는지 확인하는 함수
/// </summary>
///	<param name="win">win != NONE이면 즉시 false를 반환한다.</param>
/// <returns>게임이 종료되었다면 true를, 아니라면 false를 반환한다.</returns>
bool is_finished(board_t board, Winner *win) {
	int x, y;
	if (*win != NONE)
		return true;

	// 가로가 모두 동일한 줄이 있는가?
	for (x = 0; x < BOARD_SIZE; x++) 
		if (board[x][0] == board[x][1] && board[x][1] == board[x][2]
			&& board[x][0] != NONE) {
			*win = board[x][0];
			return true;
		}
	
	// 세로가 모두 동일한 줄이 있는가?
	for (y = 0; y < BOARD_SIZE; y++)
		if (board[0][y] == board[1][y] && board[1][y] == board[2][y]
			&& board[0][y] != NONE) {
			*win = board[0][y];
			return true;
		}

	// 대각선으로 모두 동일한 줄이 있는가?
	if (((board[0][0] == board[1][1] && board[1][1] == board[2][2])
		|| (board[0][2] == board[1][1] && board[1][1] == board[2][0]))
		&& board[1][1] != NONE) {
		*win = board[1][1];
		return true;
	}

	for (x = 0; x < BOARD_SIZE; x++)
		for (y = 0; y < BOARD_SIZE; y++)
			if (board[x][y] == NONE) {
				return false;
			}
	return true;
}