#include "inputclass.h"

InputClass::InputClass()
{

}
InputClass::InputClass(const InputClass& other)
{

}
InputClass::~InputClass()
{}

void InputClass::Initialize()
{
	int i;

	//모든 키들을 초기화
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
	return;
}

void InputClass::KeyDown(unsigned int input)
{
	//키가 눌렸다면 상태 저장
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	//키가 떼어지면 상태 false 저장
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	//현재 키가 눌림 / 땜 상태인지 반환
	return m_keys[key];
}
