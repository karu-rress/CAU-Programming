//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(int argc, char* argv[]) {
	printf("test c\n\n"); // printf

	int usd;
	int krw;

	printf("달러화 금액을 입력하시오: ");
	scanf("%d", &usd);

	krw = 1200 * usd;

	printf("달러화 %d달러는 %d원입니다.\n", usd, krw);


	const int constant = 30;
	int* bad = &constant;

	*bad = 40;

	printf("constant: %d", constant);

	return 0;
}