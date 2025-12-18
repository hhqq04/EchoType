#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define MAX_INPUT_SIZE 10000
#define MAX_SIMULATION_COUNT 30

void sendUnicodeChar(WCHAR ch) {
	INPUT input[2] = {0};
	
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = 0;
	input[0].ki.wScan = ch;
	input[0].ki.dwFlags = KEYEVENTF_UNICODE;
	
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = 0;
	input[1].ki.wScan = ch;
	input[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	
	SendInput(2, input, sizeof(INPUT));
}

void simulateEnterKey() {
	INPUT input[2] = {0};
	
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = VK_RETURN;
	
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = VK_RETURN;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP;
	
	SendInput(2, input, sizeof(INPUT));
}

WCHAR* gbkToUnicode(const char* gbkStr, int* unicodeLen) {
	int len = MultiByteToWideChar(936, 0, gbkStr, -1, NULL, 0);
	if (len == 0) {
		*unicodeLen = 0;
		return NULL;
	}
	
	WCHAR* unicodeStr = (WCHAR*)malloc(len * sizeof(WCHAR));
	if (unicodeStr == NULL) {
		*unicodeLen = 0;
		return NULL;
	}
	
	MultiByteToWideChar(936, 0, gbkStr, -1, unicodeStr, len);
	*unicodeLen = len - 1;
	
	return unicodeStr;
}

int main() {
	system("chcp 936 > nul");
	
	int simulationCount = 0;
	
	printf("===============================================\n");
	printf("           Auto Keyboard Simulator\n");
	printf("===============================================\n");
	
	while (simulationCount < MAX_SIMULATION_COUNT) {
		char input[MAX_INPUT_SIZE];
		int ch;
		int index = 0;
		
		printf("Task %d/%d - Please input text (Ctrl+Z to confirm):\n", 
			simulationCount + 1, MAX_SIMULATION_COUNT);
		
		while ((ch = getchar()) != EOF && index < MAX_INPUT_SIZE - 1) {
			input[index++] = (char)ch;
		}
		
		if (index == 0 && ch == EOF) {
			break;
		}
		
		input[index] = '\0';
		
		printf("\nText captured.\n");
		printf("Please select the target input window and press [ENTER] to start typing...\n");
		
		while(GetAsyncKeyState(VK_RETURN) & 0x8000) {
			Sleep(10);
		}
		
		while (1) {
			if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
				break;
			}
			Sleep(10);
		}
		
		Sleep(200);
		
		int unicodeLen;
		WCHAR* unicodeStr = gbkToUnicode(input, &unicodeLen);
		
		if (unicodeStr != NULL) {
			for (int i = 0; i < unicodeLen; i++) {
				if (unicodeStr[i] == L'\n') {
					simulateEnterKey();
				} else {
					sendUnicodeChar(unicodeStr[i]);
				}
				Sleep(2);
			}
			
			free(unicodeStr);
			printf("\nDone!\n\n");
		}
		
		simulationCount++;
		
		if (simulationCount >= MAX_SIMULATION_COUNT) {
			break;
		}
		
		int c;
		while ((c = getchar()) != '\n' && c != EOF);
	}
	
	return 0;
}
