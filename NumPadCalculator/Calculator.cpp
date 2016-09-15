#include "stdafx.h"
#include "Calculator.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>

#ifdef _WIN64
#define CHROMASDKDLL        _T("RzChromaSDK64.dll")
#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
#endif
using namespace ChromaSDK;
using namespace ChromaSDK::Keyboard;
using namespace std;

typedef RZRESULT(*INIT)(void);
typedef RZRESULT(*UNINIT)(void);
typedef RZRESULT(*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*SETEFFECT)(RZEFFECTID EffectId);
typedef RZRESULT(*DELETEEFFECT)(RZEFFECTID EffectId);
typedef RZRESULT(*REGISTEREVENTNOTIFICATION)(HWND hWnd);
typedef RZRESULT(*UNREGISTEREVENTNOTIFICATION)(void);
typedef RZRESULT(*QUERYDEVICE)(RZDEVICEID DeviceId, ChromaSDK::DEVICE_INFO_TYPE &DeviceInfo);

INIT Init = NULL;
UNINIT UnInit = NULL;
CREATEEFFECT CreateEffect = NULL;
CREATEKEYBOARDEFFECT CreateKeyboardEffect = NULL;
SETEFFECT SetEffect = NULL;
DELETEEFFECT DeleteEffect = NULL;
QUERYDEVICE QueryDevice = NULL;



BOOL Num_Pad_Calculator::IsDeviceConnected(RZDEVICEID DeviceId)
{
	if (QueryDevice != NULL)
	{
		ChromaSDK::DEVICE_INFO_TYPE DeviceInfo = {};
		RZRESULT Result = QueryDevice(DeviceId, DeviceInfo);

		ASSERT(Result == RZRESULT_SUCCESS);

		return DeviceInfo.Connected;
	}

	return FALSE;
}


Num_Pad_Calculator::Num_Pad_Calculator() :m_ChromaSDKModule(NULL)
{
}
Num_Pad_Calculator::~Num_Pad_Calculator()
{
}

BOOL Num_Pad_Calculator::Initialize()
{
	if (m_ChromaSDKModule == NULL)
	{
		m_ChromaSDKModule = LoadLibrary(CHROMASDKDLL);
		if (m_ChromaSDKModule == NULL)
		{
			ASSERT(GetLastError() == ERROR_SUCCESS);
			return FALSE;
		}
	}

	if (Init == NULL)
	{
		RZRESULT Result = RZRESULT_INVALID;
		Init = (INIT)GetProcAddress(m_ChromaSDKModule, "Init");
		if (Init)
		{
			Result = Init();
			if (Result == RZRESULT_SUCCESS)
			{
				CreateEffect = (CREATEEFFECT)GetProcAddress(m_ChromaSDKModule, "CreateEffect");
				CreateKeyboardEffect = (CREATEKEYBOARDEFFECT)GetProcAddress(m_ChromaSDKModule, "CreateKeyboardEffect");
				SetEffect = (SETEFFECT)GetProcAddress(m_ChromaSDKModule, "SetEffect");
				DeleteEffect = (DELETEEFFECT)GetProcAddress(m_ChromaSDKModule, "DeleteEffect");
				QueryDevice = (QUERYDEVICE)GetProcAddress(m_ChromaSDKModule, "QueryDevice");

				if (CreateEffect &&
					CreateKeyboardEffect &&
					SetEffect &&
					DeleteEffect &&
					QueryDevice)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

void Num_Pad_Calculator::ResetEffects(UINT DeviceType) {
	if (CreateKeyboardEffect) {
		CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_NONE, NULL, NULL);
	}
}

BOOL Num_Pad_Calculator::no_operation_flash(ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE keyboard_effect) {
	// Source for the key indexes
	// http://developer.razerzone.com/chroma/razer-chroma-led-profiles/
	int count = 0;
	RZRESULT Result_Keyboard;
	while (count < 10) {
		count++;
		//Custom Color the Chroma Keyboard
		keyboard_effect.Color[2][21] = RED;		//Add Key
		keyboard_effect.Color[1][21] = RED;		//Sub Key
		keyboard_effect.Color[1][20] = RED;		//Mul Key
		keyboard_effect.Color[1][19] = RED;		//Div Key

		//Now we apply the effect to the keyboard
		Result_Keyboard = CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &keyboard_effect, NULL);
		Sleep(50);
		//Custom Color the Chroma Keyboard
		keyboard_effect.Color[2][21] = BACKGROUND;		//Add Key
		keyboard_effect.Color[1][21] = BACKGROUND;		//Sub Key
		keyboard_effect.Color[1][20] = BACKGROUND;		//Mul Key
		keyboard_effect.Color[1][19] = BACKGROUND;		//Div Key

		Result_Keyboard = CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &keyboard_effect, NULL);
		Sleep(50);
	}
	//You can work with the Result as well, e.g. checking if everythings ok

	return Result_Keyboard;

}

BOOL Num_Pad_Calculator::flash_result(int result, ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE keyboard_effect) {
	cout << " Result: " << result << endl;
	RZRESULT Result_Keyboard;
	std::vector <std::pair<int, int>> result_vector;
	Num_Pad_Calculator calc;
	keyboard_effect = calc.set_background_effect();

	bool negative_flag = false;
	if (result < 0) {
		result *= -1;
		negative_flag = true;
	}
	do {
		int digit = result % 10;
		switch (digit) {
		case 0:
			result_vector.insert(result_vector.begin(), std::make_pair(5, 19));
			break;
		case 1:
			result_vector.insert(result_vector.begin(), std::make_pair(4, 18));
			break;
		case 2:
			result_vector.insert(result_vector.begin(), std::make_pair(4, 19));
			break;
		case 3:
			result_vector.insert(result_vector.begin(), std::make_pair(4, 20));
			break;
		case 4:
			result_vector.insert(result_vector.begin(), std::make_pair(3, 18));
			break;
		case 5:
			result_vector.insert(result_vector.begin(), std::make_pair(3, 19));
			break;
		case 6:
			result_vector.insert(result_vector.begin(), std::make_pair(3, 20));
			break;
		case 7:
			result_vector.insert(result_vector.begin(), std::make_pair(2, 18));
			break;
		case 8:
			result_vector.insert(result_vector.begin(), std::make_pair(2, 19));
			break;
		case 9:
			result_vector.insert(result_vector.begin(), std::make_pair(2, 20));
			break;
		}
		result /= 10;
	} while (result >= 1);

	if (negative_flag) {
		result_vector.insert(result_vector.begin(), std::make_pair(1, 21));
	}

	//Custom Color the Chroma Keyboard
	for (const std::pair<int, int> map : result_vector) {
		keyboard_effect.Color[map.first][map.second] = WHITE;
		//Now we apply the effect to the keyboard
		Result_Keyboard = CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &keyboard_effect, NULL);
		Sleep(500);
		keyboard_effect.Color[map.first][map.second] = BACKGROUND;
		Result_Keyboard = CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_CUSTOM, &keyboard_effect, NULL);
		Sleep(100);
	}

	//You can work with the Result as well, e.g. checking if everythings ok

	return Result_Keyboard;

}

int Num_Pad_Calculator::do_math(int number1, int number2, int operation) {
	if (number2 == 0) {
		return 0;
	}
	cout << "Number1: " << number1 << " || Number2: " << number2 << " OPP: " << operation << " |||| ";
	switch (operation) {
	case 90 :						//Add
		return number1 += number2;
	case 91 :						//Subtract
		return number1 -= number2;
	case 92 :						//Multiply
		return number1 *= number2;
	case 93 :						//Divide
		return number1 /= number2;
	}
	return 1;
}

ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE Num_Pad_Calculator::set_background_effect() {
	//Creates a blank ChromaSDK Keyboard effect object to add in background effects
	//for the rest of the keyboard, which is returned to the calling function.
	//Currently onld does Static effects. Background color is found in Calculator.h
	ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE keyboard = {};

	for (UINT row = 0; row < ChromaSDK::Keyboard::MAX_ROW; row++) {
		for (UINT col = 0; col < ChromaSDK::Keyboard::MAX_COLUMN; col++) {
			keyboard.Color[row][col] = BACKGROUND;
		}
	}
	return keyboard;
}


int main() {

	Num_Pad_Calculator impl_test;
	BOOL test_for_init = impl_test.Initialize();
	ChromaSDK::Keyboard::CUSTOM_EFFECT_TYPE keyboard = impl_test.set_background_effect();
	bool initial_num = true;
	if (test_for_init) {
		while (true) {
			//Register hotkeys to take in numbers/operations from keypad.
			RegisterHotKey(0, 0, NULL, VK_NUMPAD0);
			RegisterHotKey(0, 1, NULL, VK_NUMPAD1);
			RegisterHotKey(0, 2, NULL, VK_NUMPAD2);
			RegisterHotKey(0, 3, NULL, VK_NUMPAD3);
			RegisterHotKey(0, 4, NULL, VK_NUMPAD4);
			RegisterHotKey(0, 5, NULL, VK_NUMPAD5);
			RegisterHotKey(0, 6, NULL, VK_NUMPAD6);
			RegisterHotKey(0, 7, NULL, VK_NUMPAD7);
			RegisterHotKey(0, 8, NULL, VK_NUMPAD8);
			RegisterHotKey(0, 9, NULL, VK_NUMPAD9);
			RegisterHotKey(0, 90, NULL, VK_ADD);
			RegisterHotKey(0, 91, NULL, VK_SUBTRACT);
			RegisterHotKey(0, 92, NULL, VK_MULTIPLY);
			RegisterHotKey(0, 93, NULL, VK_DIVIDE);
			RegisterHotKey(0, 99, NULL, VK_RETURN);
			int num1 = 0, num2 = 0;
			int operation = -1;
			MSG msg;
			while (GetMessage(&msg, 0, 0, 0)) {
				PeekMessage(&msg, 0, 0, 0, 0x0001);
				if (msg.wParam < 20) {
					if (operation == -1) {
						if (initial_num) {			//Removes the intital run through the loop with a 1 that is not user input.
							num1 = 0;
							initial_num = false;
						}
						else {
							num1 = num1 * 10 + msg.wParam;
						}
					}
					else {
						num2 = num2 * 10 + msg.wParam;
					}
				}
				else if (msg.wParam >= 90 && msg.wParam <= 95) {
					if (operation == -1) {
						operation = msg.wParam;
					}
					else {
						num1 = impl_test.do_math(num1, num2, operation);
						num2 = 0;
						operation = msg.wParam;
					}
				}
				else {
					if (operation == -1) {
						impl_test.no_operation_flash(keyboard);
					}
					else {
						num1 = impl_test.do_math(num1, num2, operation);
						impl_test.flash_result(num1, keyboard);
						operation = -1;
						num2 = 0;
						num1 = 0;
					}
				}
			}
			
		}
	}
	return 0;
}
