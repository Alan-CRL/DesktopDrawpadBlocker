#pragma once

#include "DdbMain.h"

struct SetListStruct
{
	SetListStruct()
	{
		sleepTime = 5000;

		mode = 0;
		hostPath = L"";
		hostOn = false, restartHost = true;

		memset(InterceptWindow, true, sizeof(InterceptWindow));
	}

	int sleepTime;

	int mode; // 0 ����ģʽ 1 �����������ر� 2 ��������ر�
	wstring hostPath;
	bool hostOn, restartHost; // restartHost�������޶���ģʽ�����������򱻹رպ����ص���������Ĵ��ں�������������

	bool InterceptWindow[10];
	/* InterceptWindow �б�
	 *
	 * 0 AiClass ����������
	 * 1 ϣ�ְװ� ����������
	 * 2 ϣ��Ʒ�Σ�������������PPT�ؼ���
	 * 3 ϣ��Ʒ�� ���滭��
	 * 4 ϣ��PPTС����
	 *
	 */
};
extern SetListStruct setList;

bool ConfigurationChange();
bool CloseSoftware();

bool ReadSetting();
bool WriteSetting(bool close = false);