#include "stdafx.h"
#include "WindowButtonMenu.h"

ButtonMenu::ButtonMenu()
{
	button_text = _T("");
	menu_count  = 0;
	for ( int i = 0; i < MAX_MENU_ITEM; ++i )
 		menu_text[i] = _T("");
}


ButtonMenu::~ButtonMenu()
{
	;
}

MainButtonMenu::MainButtonMenu()
{
	button_menu = new ButtonMenu[NUM_MAIN_BUTTON];
	init_menu_table();
}

MainButtonMenu::~MainButtonMenu()
{
	delete [] button_menu;
	button_menu = NULL;
}

const LPCTSTR* MainButtonMenu::get_menu_text(int mainbutton)
{
	if ( mainbutton > NUM_MAIN_BUTTON || mainbutton < 0 )
	{
		return NULL;
	}
	return button_menu[mainbutton].menu_text;
}

const int MainButtonMenu::GetMenuCount(int mainbutton)
{
	if ( mainbutton >= NUM_MAIN_BUTTON || mainbutton < 0 )
	{
		return 0;
	}
	return button_menu[mainbutton].menu_count;
}

// main menu dialog text grid.
void MainButtonMenu::init_menu_table()
{
	button_menu[IDM_USER].menu_count = 2;
	button_menu[IDM_USER].menu_text[0] = _T("�û�����");
 	button_menu[IDM_USER].menu_text[1] = _T("�˳����");
// 	button_menu[winUserDefine].menu_text[2] = _T("��⹤��");

	button_menu[IDM_SETTING].menu_count = 5;
	button_menu[IDM_SETTING].menu_text[0] = _T("��е�ṹ����");
	button_menu[IDM_SETTING].menu_text[1] = _T("�ؽ������");
	button_menu[IDM_SETTING].menu_text[2] = _T("ԭ���趨");
	button_menu[IDM_SETTING].menu_text[3] = _T("IO���");
	button_menu[IDM_SETTING].menu_text[4] = _T("��ȫ����");
//  button_menu[winSetting].menu_text[5] = _T("ͨѶ����");
// 	button_menu[winSetting].menu_text[6] = _T("����������");

	button_menu[IDM_VARIABLE].menu_count = 4;
	button_menu[IDM_VARIABLE].menu_text[0] = _T("��������");
	button_menu[IDM_VARIABLE].menu_text[1] = _T("λ��");
	button_menu[IDM_VARIABLE].menu_text[2] = _T("�û�����ϵ");
	button_menu[IDM_VARIABLE].menu_text[3] = _T("��������ϵ");

	button_menu[IDM_LOG].menu_count = 0;
 
}
//////////////////////////////////////////////////////////////////////////

ChildWindowButtonMenu::ChildWindowButtonMenu():bRunFlag(false)
{
	 button_caption = new ButtonMenu*[winNum];
	 for ( int i = 0; i < winNum; ++i )
	 {
		 button_caption[i] = new ButtonMenu[NUM_CHILD_BUTTON];
 	 }
 	 
	 init_button_table();
	 
}

ChildWindowButtonMenu::~ChildWindowButtonMenu()
{
	for ( int i = 0; i < winNum; ++i )
	{
		delete [] button_caption[i];
	}

	delete [] button_caption;
 
}

const LPCTSTR ChildWindowButtonMenu::get_button_caption(ChilidWindowKind kind, int button_id)
{
 	return button_caption[kind][button_id].button_text;
}

const int ChildWindowButtonMenu::GetMenuCount(ChilidWindowKind kind, int button_id)
{
	return button_caption[kind][button_id].menu_count;
}

const LPCTSTR* ChildWindowButtonMenu::get_menu_text(ChilidWindowKind kind, int button_id)
{
	return button_caption[kind][button_id].menu_text;
}

// bottom button menu item.
void ChildWindowButtonMenu::set_win_logfile()
{
	button_caption[winLogInformation][0].button_text = _T("ȫ��ȷ��");
	button_caption[winLogInformation][1].button_text = _T("��־����");
	button_caption[winLogInformation][2].button_text = _T("��ǰ��ѯ");
	button_caption[winLogInformation][3].button_text = _T("����ѯ");
	button_caption[winLogInformation][4].button_text = _T("");
	button_caption[winLogInformation][5].button_text = _T("");

// 	button_caption[winLogInformation][0].menu_count = 7;
// 	button_caption[winLogInformation][0].menu_text[0] = _T("ȫ����¼");
// 	button_caption[winLogInformation][0].menu_text[1] = _T("������¼");
// 	button_caption[winLogInformation][0].menu_text[2] = _T("�����¼");
// 	button_caption[winLogInformation][0].menu_text[3] = _T("ִ�м�¼");
// 	button_caption[winLogInformation][0].menu_text[4] = _T("������¼");
// 	button_caption[winLogInformation][0].menu_text[5] = _T("ϵͳ��¼");
// 	button_caption[winLogInformation][0].menu_text[6] = _T(" ");
}

void ChildWindowButtonMenu::set_win_posteach()
{
	button_caption[winPostionTeach][0].button_text = _T("��ʾ����");
	button_caption[winPostionTeach][1].button_text = _T("����ģʽ");
	button_caption[winPostionTeach][2].button_text = _T("ʾ�����");
	button_caption[winPostionTeach][3].button_text = _T("ʾ�̱���");
	button_caption[winPostionTeach][4].button_text = _T("�㶯�ٶ�");
	button_caption[winPostionTeach][5].button_text = _T("�㶯");

	button_caption[winPostionTeach][0].menu_count = 4;
	button_caption[winPostionTeach][0].menu_text[0] = _T("�����ֵ");
	button_caption[winPostionTeach][0].menu_text[1] = _T("�ؽ�����");
	button_caption[winPostionTeach][0].menu_text[2] = _T("��������");
	button_caption[winPostionTeach][0].menu_text[3] = _T("�û�����");

	button_caption[winPostionTeach][1].menu_count = 3;
	button_caption[winPostionTeach][1].menu_text[0] = _T("ʾ��ģʽ");
	button_caption[winPostionTeach][1].menu_text[1] = _T("����ģʽ");
	button_caption[winPostionTeach][1].menu_text[2] = _T("Զ��ģʽ");
	 
	button_caption[winPostionTeach][2].menu_count = 0;
// 	button_caption[winPostionTeach][2].menu_text[0] = _T("�ؽ�����");
// 	button_caption[winPostionTeach][2].menu_text[1] = _T("ֱ������");
// 	button_caption[winPostionTeach][2].menu_text[2] = _T("��������");
// 	button_caption[winPostionTeach][2].menu_text[3] = _T("�û�����");

// 	button_caption[winPostionTeach][3].menu_count = 1;
// 	button_caption[winPostionTeach][3].menu_text[0] = _T("�ؽ�����%");

 	// λ�ù���:  �˵�
	button_caption[winPostionTeach][4].menu_count   = 6;
	button_caption[winPostionTeach][4].menu_text[0] = _T("100%");
	button_caption[winPostionTeach][4].menu_text[1] = _T("50%");
	button_caption[winPostionTeach][4].menu_text[2] = _T("25%");
	button_caption[winPostionTeach][4].menu_text[3] = _T("10%");
	button_caption[winPostionTeach][4].menu_text[4] = _T("1%");
	button_caption[winPostionTeach][4].menu_text[5] = _T("0.1%");

	button_caption[winPostionTeach][5].menu_count   = 5;
	button_caption[winPostionTeach][5].menu_text[0] = _T("�ؽ�");
	button_caption[winPostionTeach][5].menu_text[1] = _T("ֱ��");
	button_caption[winPostionTeach][5].menu_text[2] = _T("����");
	button_caption[winPostionTeach][5].menu_text[3] = _T("�û�");
	button_caption[winPostionTeach][5].menu_text[4] = _T("�ص�ԭ��");
}

void ChildWindowButtonMenu::set_win_userdefine()
{
	// 
	button_caption[winUserDefine][0].button_text = _T("����");
	button_caption[winUserDefine][1].button_text = _T("����");
	button_caption[winUserDefine][2].button_text = _T("");
	button_caption[winUserDefine][3].button_text = _T("");
	button_caption[winUserDefine][4].button_text = _T("");
	button_caption[winUserDefine][5].button_text = _T("");
}

void ChildWindowButtonMenu::init_button_table()
{
	set_win_userdefine(); // first window

	//second setting -------------------------------------------------------------
	button_caption[winSetting][0].button_text = _T("�������");
	button_caption[winSetting][1].button_text = _T("�޸Ĳ���");
	button_caption[winSetting][2].button_text = _T("�����޸�");
	button_caption[winSetting][3].button_text = _T("�����޸�");
	button_caption[winSetting][4].button_text = _T("");
	button_caption[winSetting][5].button_text = _T("�ر�");

	// ����������------------------------------------------------------
	button_caption[winVariableData][0].button_text = _T("�༭");
	button_caption[winVariableData][1].button_text = _T("ʾ��");
	button_caption[winVariableData][2].button_text = _T("���");
	button_caption[winVariableData][3].button_text = _T("�½�");
	button_caption[winVariableData][4].button_text = _T("ɾ��");
	button_caption[winVariableData][5].button_text = _T("ˢ��");

	// ��������:  �˵�
	button_caption[winVariableData][0].menu_count   = 4;
 	button_caption[winVariableData][0].menu_text[0] = _T("ճ��");
	button_caption[winVariableData][0].menu_text[1] = _T("����");
	button_caption[winVariableData][0].menu_text[2] = _T("����");
	button_caption[winVariableData][0].menu_text[3] = _T("������");
 
	
	// ��Ŀ������--------------------------------------------
	button_caption[winProject][0].button_text = _T("�½�");
	button_caption[winProject][1].button_text = _T("����");
	button_caption[winProject][2].button_text = _T("����");
	button_caption[winProject][3].button_text = _T("������");
	button_caption[winProject][4].button_text = _T("ɾ��");
	button_caption[winProject][5].button_text = _T("����");

	// ���������---------------------------------------------
	button_caption[winProgram][0].button_text = _T("�޸�");
	button_caption[winProgram][1].button_text = _T("����");
	button_caption[winProgram][2].button_text = _T("�½�");
	button_caption[winProgram][3].button_text = _T("�༭");
	button_caption[winProgram][4].button_text = _T("����");
	button_caption[winProgram][5].button_text = _T("�߼�");

	button_caption[winProgram][1].menu_count   = 3;
	button_caption[winProgram][1].menu_text[0] = _T("����PC");
	button_caption[winProgram][1].menu_text[1] = _T("��������");
	button_caption[winProgram][1].menu_text[2] = _T("��������");

	//����༭�˵�--------------------------------------------
	button_caption[winProgram][3].menu_count   = 6;
	button_caption[winProgram][3].menu_text[0] = _T("�½���");
	button_caption[winProgram][3].menu_text[1] = _T("ɾ����");
	button_caption[winProgram][3].menu_text[2] = _T("ɾ��ָ��");
	button_caption[winProgram][3].menu_text[3] = _T("ճ��");
	button_caption[winProgram][3].menu_text[4] = _T("����");
	button_caption[winProgram][3].menu_text[5] = _T("����");
 
	button_caption[winProgram][5].menu_count = 6; // ���߼����Ӳ˵�
	button_caption[winProgram][5].menu_text[0] = _T("����");
	button_caption[winProgram][5].menu_text[1] = _T("����");
	button_caption[winProgram][5].menu_text[2] = _T("�˸�");
	button_caption[winProgram][5].menu_text[3] = _T("����");
	button_caption[winProgram][5].menu_text[4] = _T("��ע��");
	button_caption[winProgram][5].menu_text[5] = _T("ȥע��");

	// ����༭�Ӵ���---------------------------------------------
	button_caption[winEditCommand][0].button_text = _T("����");
	button_caption[winEditCommand][1].button_text = _T("�滻");
	button_caption[winEditCommand][2].button_text = _T("����");
	button_caption[winEditCommand][3].button_text = _T("ɾ��");
	button_caption[winEditCommand][4].button_text = _T("ȡ��");
	button_caption[winEditCommand][5].button_text = _T("ȷ��");


	// λ�ù�����---------------------------------------------
	set_win_posteach();
 
	// ��ʷ������--------------------------------------------------
	set_win_logfile();
  
	// �½�����
	button_caption[winNewScriptVariable][0].button_text = _T("");
	button_caption[winNewScriptVariable][1].button_text = _T("");
	button_caption[winNewScriptVariable][2].button_text = _T("");
	button_caption[winNewScriptVariable][3].button_text = _T("");
	button_caption[winNewScriptVariable][4].button_text = _T("ȡ��");
	button_caption[winNewScriptVariable][5].button_text = _T("ȷ��");

	// ��������豸���--------------------------------------------------
	button_caption[winInOutDeviece][0].button_text = _T("����");
	button_caption[winInOutDeviece][1].button_text = _T("ͨ������");
	button_caption[winInOutDeviece][2].button_text = _T("ͨ�����");
	button_caption[winInOutDeviece][3].button_text = _T("ר��IO");
	button_caption[winInOutDeviece][4].button_text = _T("�ͷ�ȫ��");
	button_caption[winInOutDeviece][5].button_text = _T("");

	// �������� �����۳�-------------------------------------------------
	button_caption[winSettingMechanism][0].button_text = _T("�������");
	button_caption[winSettingMechanism][1].button_text = _T(" ");
	button_caption[winSettingMechanism][2].button_text = _T("�����޸�");
	button_caption[winSettingMechanism][3].button_text = _T("�����޸�");
	button_caption[winSettingMechanism][4].button_text = _T("");
	button_caption[winSettingMechanism][5].button_text = _T("�ر�");
	// �������� �������-------------------------------------------------
	button_caption[winSettingJoints][0].button_text = _T("");
	button_caption[winSettingJoints][1].button_text = _T("");
	button_caption[winSettingJoints][2].button_text = _T("");
	button_caption[winSettingJoints][3].button_text = _T("");
	button_caption[winSettingJoints][4].button_text = _T("ȡ��");
	button_caption[winSettingJoints][5].button_text = _T("ȷ��");
	// �������� ԭ���趨-------------------------------------------------
	button_caption[winSettingOrigin][0].button_text = _T("����ԭ��");
	button_caption[winSettingOrigin][1].button_text = _T("��������");
	button_caption[winSettingOrigin][2].button_text = _T("����ѡ��");
	button_caption[winSettingOrigin][3].button_text = _T("��ȡ����");
	button_caption[winSettingOrigin][4].button_text = _T("��ԭ��");
	button_caption[winSettingOrigin][5].button_text = _T("�ر�");

	////ʾ�̵�λ����
	button_caption[winTeachDLG][0].button_text = _T(" ");
	button_caption[winTeachDLG][1].button_text = _T("ʾ��");
	button_caption[winTeachDLG][2].button_text = _T("����");
	button_caption[winTeachDLG][3].button_text = _T(" ");
	button_caption[winTeachDLG][4].button_text = _T("����");
	button_caption[winTeachDLG][5].button_text = _T("��ֹ");

	////�û�����ϵ����
	button_caption[winUserCoSysDLG][0].button_text = _T("");
	button_caption[winUserCoSysDLG][1].button_text = _T("");
	button_caption[winUserCoSysDLG][2].button_text = _T("");
	button_caption[winUserCoSysDLG][3].button_text = _T("");
	button_caption[winUserCoSysDLG][4].button_text = _T("");
	button_caption[winUserCoSysDLG][5].button_text = _T("����");

	////��������ϵ����
	button_caption[winToolCoSysDLG][0].button_text = _T("");
	button_caption[winToolCoSysDLG][1].button_text = _T("");
	button_caption[winToolCoSysDLG][2].button_text = _T("");
	button_caption[winToolCoSysDLG][3].button_text = _T("");
	button_caption[winToolCoSysDLG][4].button_text = _T("");
	button_caption[winToolCoSysDLG][5].button_text = _T("����");

	////������ô���
	button_caption[winPalletDLG][0].button_text = _T(" ");
	button_caption[winPalletDLG][1].button_text = _T("ʾ��");
	button_caption[winPalletDLG][2].button_text = _T("����");
	button_caption[winPalletDLG][3].button_text = _T(" ");
	button_caption[winPalletDLG][4].button_text = _T("����");
	button_caption[winPalletDLG][5].button_text = _T("��ֹ");

	////ָ�����ô���
	button_caption[winInstructionDLG][0].button_text = _T("");
	button_caption[winInstructionDLG][1].button_text = _T(" ");
	button_caption[winInstructionDLG][2].button_text = _T(" ");
	button_caption[winInstructionDLG][3].button_text = _T(" ");
	button_caption[winInstructionDLG][4].button_text = _T("ȡ��");
	button_caption[winInstructionDLG][5].button_text = _T("ȷ��");

	////ָ��༭����
	button_caption[winInstructionEdit][0].button_text = _T("����");
	button_caption[winInstructionEdit][1].button_text = _T("ʾ��");
	button_caption[winInstructionEdit][2].button_text = _T("");   // ���
	button_caption[winInstructionEdit][3].button_text = _T("");   // ֵ
	button_caption[winInstructionEdit][4].button_text = _T("ȡ��");
	button_caption[winInstructionEdit][5].button_text = _T("ȷ��");

	button_caption[winInstructionEdit][0].menu_count = 4 ;
	button_caption[winInstructionEdit][0].menu_text[0] = _T("ѡ��");
	button_caption[winInstructionEdit][0].menu_text[1] = _T("�½�");
	button_caption[winInstructionEdit][0].menu_text[2] = _T("����");
	button_caption[winInstructionEdit][0].menu_text[3] = _T("ɾ����ֵ");

	////�����½������ô���
	button_caption[winVariableBuilder][0].button_text = _T("");
	button_caption[winVariableBuilder][1].button_text = _T("");
	button_caption[winVariableBuilder][2].button_text = _T("");
	button_caption[winVariableBuilder][3].button_text = _T("");
	button_caption[winVariableBuilder][4].button_text = _T("ȡ��");
	button_caption[winVariableBuilder][5].button_text = _T("����");

	////�����½������ô���
	button_caption[winSecurityArea][0].button_text = _T("����");
	button_caption[winSecurityArea][1].button_text = _T("");
	button_caption[winSecurityArea][2].button_text = _T("");
	button_caption[winSecurityArea][3].button_text = _T("����");
	button_caption[winSecurityArea][4].button_text = _T("����");
	button_caption[winSecurityArea][5].button_text = _T("����");

	// ʾ����������------------------------------------------------------
	button_caption[winTeachPanelDlg][0].button_text = _T("��ͨ�ŷ�");
	button_caption[winTeachPanelDlg][1].button_text = _T("����ʾ��");
	button_caption[winTeachPanelDlg][2].button_text = _T("�л�����");
	button_caption[winTeachPanelDlg][3].button_text = _T("ʾ�̱���");
	button_caption[winTeachPanelDlg][4].button_text = _T("���ò���");
	button_caption[winTeachPanelDlg][5].button_text = _T("����");

	// ʾ����������:  �˵�
	button_caption[winTeachPanelDlg][2].menu_count   = 4;
	button_caption[winTeachPanelDlg][2].menu_text[0] = _T("�ؽ�");
	button_caption[winTeachPanelDlg][2].menu_text[1] = _T("ֱ��");
	button_caption[winTeachPanelDlg][2].menu_text[2] = _T("����");
	button_caption[winTeachPanelDlg][2].menu_text[3] = _T("�û�");
	 
}

// modify button caption when run program.
void ChildWindowButtonMenu::init_table_running()
{
 	for ( int i = 0; i < winNum; ++i )
	{
		for ( int j = 0; j < NUM_CHILD_BUTTON; ++j )
		{
			button_caption[i][j].button_text = _T("");
 		}
	}
	// ���������---------------------------------------------
	button_caption[winProgram][0].button_text = _T("");
	button_caption[winProgram][1].button_text = _T("����");
	button_caption[winProgram][2].button_text = _T("����");
	button_caption[winProgram][3].button_text = _T("");
	button_caption[winProgram][4].button_text = _T("��ͣ");
	button_caption[winProgram][5].button_text = _T("��ֹ");

	button_caption[winProgram][5].menu_count = 0;

	set_win_userdefine();
	set_win_posteach();
	set_win_logfile();
}

void ChildWindowButtonMenu::reset_button_table(bool bIsRunning)
{
	if ( bIsRunning )
	{
		if ( !bRunFlag )
		{
 			init_table_running();
 		}
	}
	else
	{
		if ( bRunFlag )
		{
			init_button_table();
 		}
	}
	bRunFlag = bIsRunning;
}

void ChildWindowButtonMenu::update_button_text( ChilidWindowKind iWin, int iButton, LPCTSTR strText )
{
	button_caption[iWin][iButton].button_text = strText;
}