#pragma once
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <stddef.h>
class SerialPort
{
public:
	SerialPort(
		wchar_t* portNum,		// 串口号
		DWORD baudRate = 9600,			// 波特率
		BYTE  byteSize = 8,				// 数据位
		BYTE  parityBit = NOPARITY,		// 检验位
		BYTE  stopBit = ONESTOPBIT		// 停止位
	);

	~SerialPort();

public:

	bool openComm();										// 打开串口
	void closeComm();										// 关闭串口
	bool writeToComm(BYTE data[], DWORD dwLegnth);			// 发送数据
	bool readFromComm(char buffer[], DWORD dwLength);		// 读取数据

private:

	HANDLE m_hComm;		// 通信设备
	wchar_t* m_portNum; // 串口号
	DWORD m_dwBaudRate; // 波特率
	BYTE  m_byteSize;	// 数据位
	BYTE  m_parityBit;  // 校验位
	BYTE  m_stopBit;	// 停止位
	bool  m_bOpen;		// 串口开关标志
private:

	enum BufferSize
	{
		MIN_BUFFER_SIZE = 256,
		BUFFER_SIZE = 512,
		MAX_BUFFER_SIZE = 1024
	};

	// 设置串口号
	void setPortNum(wchar_t* portNum)
	{
		this->m_portNum = portNum;
	}
	// 设置波特率
	void setBaudRate(const DWORD baudRate)
	{
		this->m_dwBaudRate = baudRate;
	}
	// 设置数据位
	void setByteSize(const BYTE byteSize)
	{
		this->m_byteSize = byteSize;
	}
	// 设置检验位
	void setParityBit(const BYTE parityBit)
	{
		this->m_parityBit = parityBit;
	}
	// 设置停止位
	void setStopBit(const BYTE stopBit)
	{
		this->m_stopBit = stopBit;
	}

	// 获取串口号
	wchar_t* getPortNum() { return m_portNum; }
	// 获取波特率
	DWORD getBaudRate() { return m_dwBaudRate; }
	// 获取数据位
	BYTE getByteSize() { return m_byteSize; }
	// 获取检验位
	BYTE getParityBit() { return m_parityBit; }
	// 获取停止位
	BYTE getStopBit() { return m_stopBit; }
};
