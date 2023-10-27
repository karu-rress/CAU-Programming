#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>		// bool, true, false
#include <stdio.h>
#include <stdlib.h>
#include <time.h>			// time()
#include <Windows.h>		// Sleep()

// 셀의 표시(' ', X, O)를 상수로 정의
typedef enum {
	NONE		= ' ',
	PLAYER		= 'X',
	COMPUTER	= 'O',
} Cell, Winner;

// 보드판 타입 정의
typedef Cell board_t[3][3];

// 함수 선언
void init_board(board_t board);
void display_board(board_t board);
Winner get_player_move(board_t board);
Winner get_computer_move(board_t board);
bool check_finished(board_t board, Winner* win);


int main(void) {
	board_t board;
	Winner winner = NONE;
	int i;

	// 난수 시드 및 보드 초기화
	srand((unsigned int)time(NULL));
	init_board(board);

	// 종료 전까지 무한 반복
	for (i = 1; check_finished(board, &winner) == false; i++) {
		display_board(board);

		// 홀수: 플레이어, 짝수: 컴퓨터
		if (i % 2 == 1)
			winner = get_player_move(board);
		else
			winner = get_computer_move(board);
	}
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
void init_board(board_t board) {
	int x, y;
	for (x = 0; x < 3; x++)
		for (y = 0; y < 3; y++)
			board[x][y] = NONE;
}

/// <summary>
/// 보드판을 출력하는 함수
/// </summary>
void display_board(board_t board) {
	system("cls");

	// 반복문을 쓰기 어려운 상황이라 개별적으로 작성함
	printf("┏━━━┳━━━┳━━━┓\n");
	printf("┃ %c ┃ %c ┃ %c ┃\n", board[0][0], board[0][1], board[0][2]);
	printf("┣━━━╋━━━╋━━━┫\n");
	printf("┃ %c ┃ %c ┃ %c ┃\n", board[1][0], board[1][1], board[1][2]);
	printf("┣━━━╋━━━╋━━━┫\n");
	printf("┃ %c ┃ %c ┃ %c ┃\n", board[2][0], board[2][1], board[2][2]);
	printf("┗━━━┻━━━┻━━━┛\n");
}

/// <summary>
/// 사용자의 입력을 받아 보드판에 나타내는 함수
/// </summary>
/// <returns>항복한 경우 -1, 아닌 경우 0</returns>
Winner get_player_move(board_t board) {
	int x, y;
	printf("플레이어의 차례입니다.\n");
	printf("x, y 좌표를 쉼표로 구분하여 입력하세요.\n");
	printf("게임을 포기하고 항복하려면 -1, -1을 입력하세요. ▶▷ _, _\b\b\b\b");
	for (;;) {
		scanf("%d, %d", &x, &y);
		if (x == -1 && y == -1) {			// 항복한 경우
			printf("\n항복하였습니다.\n");
			return COMPUTER;
		}

		if (board[x][y] == NONE) {
			board[x][y] = PLAYER;
			return NONE;
		}
		if (x < 0 || x >= 3 || y < 0 || y >= 3)
			printf("\n좌표가 범위를 벗어났습니다. (0~2)\n");
		else
			printf("\n이미 칠해진 칸입니다.\n");
		printf("다시 입력하세요. ▶▷ _, _\b\b\b\b");
	}
}

Winner get_computer_move(board_t board) {
	const int MAX_ITER = 10000000;
	int x, y, i;
	
	/* 직접 테스트한 결과 1억번 루프 반복에 3.96초 소요.
	 * 1천만번은 약 0.4초이므로, 이 값을 MAX_ITER로 설정
	 * 이 반복 횟수 안에 비어있는 적절한 칸을 찾으면
	 * 해당 칸에 컴퓨터의 말을 놓고,
	 * 그렇지 않은 경우 컴퓨터의 기권(시간초과)으로 처리.
	 * (무한반복을 방지하기 위한 조치)
	 */
	for (i = 0; i < MAX_ITER; i++) {
		x = rand() % 3;
		y = rand() % 3;

		if (board[x][y] == NONE) {
			printf("컴퓨터의 선택을 기다리고 있습니다");
			for (i = 0; i < 3; i++) {
				Sleep(500);
				putchar('.');
			}
			Sleep(500);
			printf("\n선택을 완료했습니다.\n");
			Sleep(500);
			board[x][y] = COMPUTER;
			return 0;
		}
	}
	printf("컴퓨터가 기권했습니다.");
	return PLAYER;
}

/// <summary>
/// 게임이 종료되었는지 확인하는 함수
/// </summary>
///	<param name="win">win != NONE이면 즉시 false를 반환한다.</param>
/// <returns>게임이 종료되었다면 true를, 아니라면 false를 반환한다.</returns>
bool check_finished(board_t board, Winner *win) {
	int x, y;
	if (*win != NONE)
		return false;

	// 가로가 모두 동일한 줄이 있는가?
	for (x = 0; x < 3; x++) 
		if (board[x][0] == board[x][1]
			&& board[x][1] == board[x][2]
			&& board[x][0] != NONE) {
			*win = board[x][0];
			return true;
		}
	
	// 세로가 모두 동일한 줄이 있는가?
	for (y = 0; y < 3; y++) 
		if (board[0][y] == board[1][y]
			&& board[1][y] == board[2][y]
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

	for (x = 0; x < 3; x++)
		for (y = 0; y < 3; y++)
			if (board[x][y] == NONE) {
				return false;
			}
	return true;
}