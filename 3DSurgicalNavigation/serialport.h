#pragma once
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <stddef.h>
class SerialPort
{
public:
	SerialPort(
		wchar_t* portNum,		// ���ں�
		DWORD baudRate = 9600,			// ������
		BYTE  byteSize = 8,				// ����λ
		BYTE  parityBit = NOPARITY,		// ����λ
		BYTE  stopBit = ONESTOPBIT		// ֹͣλ
	);

	~SerialPort();

public:

	bool openComm();										// �򿪴���
	void closeComm();										// �رմ���
	bool writeToComm(BYTE data[], DWORD dwLegnth);			// ��������
	bool readFromComm(char buffer[], DWORD dwLength);		// ��ȡ����

private:

	HANDLE m_hComm;		// ͨ���豸
	wchar_t* m_portNum; // ���ں�
	DWORD m_dwBaudRate; // ������
	BYTE  m_byteSize;	// ����λ
	BYTE  m_parityBit;  // У��λ
	BYTE  m_stopBit;	// ֹͣλ
	bool  m_bOpen;		// ���ڿ��ر�־
private:

	enum BufferSize
	{
		MIN_BUFFER_SIZE = 256,
		BUFFER_SIZE = 512,
		MAX_BUFFER_SIZE = 1024
	};

	// ���ô��ں�
	void setPortNum(wchar_t* portNum)
	{
		this->m_portNum = portNum;
	}
	// ���ò�����
	void setBaudRate(const DWORD baudRate)
	{
		this->m_dwBaudRate = baudRate;
	}
	// ��������λ
	void setByteSize(const BYTE byteSize)
	{
		this->m_byteSize = byteSize;
	}
	// ���ü���λ
	void setParityBit(const BYTE parityBit)
	{
		this->m_parityBit = parityBit;
	}
	// ����ֹͣλ
	void setStopBit(const BYTE stopBit)
	{
		this->m_stopBit = stopBit;
	}

	// ��ȡ���ں�
	wchar_t* getPortNum() { return m_portNum; }
	// ��ȡ������
	DWORD getBaudRate() { return m_dwBaudRate; }
	// ��ȡ����λ
	BYTE getByteSize() { return m_byteSize; }
	// ��ȡ����λ
	BYTE getParityBit() { return m_parityBit; }
	// ��ȡֹͣλ
	BYTE getStopBit() { return m_stopBit; }
};
