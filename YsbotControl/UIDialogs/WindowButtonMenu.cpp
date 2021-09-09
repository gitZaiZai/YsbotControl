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
	button_menu[IDM_USER].menu_text[0] = _T("用户管理");
 	button_menu[IDM_USER].menu_text[1] = _T("退出软件");
// 	button_menu[winUserDefine].menu_text[2] = _T("码垛工艺");

	button_menu[IDM_SETTING].menu_count = 5;
	button_menu[IDM_SETTING].menu_text[0] = _T("机械结构参数");
	button_menu[IDM_SETTING].menu_text[1] = _T("关节轴参数");
	button_menu[IDM_SETTING].menu_text[2] = _T("原点设定");
	button_menu[IDM_SETTING].menu_text[3] = _T("IO监控");
	button_menu[IDM_SETTING].menu_text[4] = _T("安全区域");
//  button_menu[winSetting].menu_text[5] = _T("通讯参数");
// 	button_menu[winSetting].menu_text[6] = _T("编码器设置");

	button_menu[IDM_VARIABLE].menu_count = 4;
	button_menu[IDM_VARIABLE].menu_text[0] = _T("变量管理");
	button_menu[IDM_VARIABLE].menu_text[1] = _T("位置");
	button_menu[IDM_VARIABLE].menu_text[2] = _T("用户坐标系");
	button_menu[IDM_VARIABLE].menu_text[3] = _T("工具坐标系");

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
	button_caption[winLogInformation][0].button_text = _T("全部确认");
	button_caption[winLogInformation][1].button_text = _T("日志清理");
	button_caption[winLogInformation][2].button_text = _T("向前查询");
	button_caption[winLogInformation][3].button_text = _T("向后查询");
	button_caption[winLogInformation][4].button_text = _T("");
	button_caption[winLogInformation][5].button_text = _T("");

// 	button_caption[winLogInformation][0].menu_count = 7;
// 	button_caption[winLogInformation][0].menu_text[0] = _T("全部记录");
// 	button_caption[winLogInformation][0].menu_text[1] = _T("报警记录");
// 	button_caption[winLogInformation][0].menu_text[2] = _T("错误记录");
// 	button_caption[winLogInformation][0].menu_text[3] = _T("执行记录");
// 	button_caption[winLogInformation][0].menu_text[4] = _T("操作记录");
// 	button_caption[winLogInformation][0].menu_text[5] = _T("系统记录");
// 	button_caption[winLogInformation][0].menu_text[6] = _T(" ");
}

void ChildWindowButtonMenu::set_win_posteach()
{
	button_caption[winPostionTeach][0].button_text = _T("显示数据");
	button_caption[winPostionTeach][1].button_text = _T("控制模式");
	button_caption[winPostionTeach][2].button_text = _T("示教面板");
	button_caption[winPostionTeach][3].button_text = _T("示教变量");
	button_caption[winPostionTeach][4].button_text = _T("点动速度");
	button_caption[winPostionTeach][5].button_text = _T("点动");

	button_caption[winPostionTeach][0].menu_count = 4;
	button_caption[winPostionTeach][0].menu_text[0] = _T("电机数值");
	button_caption[winPostionTeach][0].menu_text[1] = _T("关节坐标");
	button_caption[winPostionTeach][0].menu_text[2] = _T("世界坐标");
	button_caption[winPostionTeach][0].menu_text[3] = _T("用户坐标");

	button_caption[winPostionTeach][1].menu_count = 3;
	button_caption[winPostionTeach][1].menu_text[0] = _T("示教模式");
	button_caption[winPostionTeach][1].menu_text[1] = _T("再现模式");
	button_caption[winPostionTeach][1].menu_text[2] = _T("远程模式");
	 
	button_caption[winPostionTeach][2].menu_count = 0;
// 	button_caption[winPostionTeach][2].menu_text[0] = _T("关节坐标");
// 	button_caption[winPostionTeach][2].menu_text[1] = _T("直角坐标");
// 	button_caption[winPostionTeach][2].menu_text[2] = _T("工具坐标");
// 	button_caption[winPostionTeach][2].menu_text[3] = _T("用户坐标");

// 	button_caption[winPostionTeach][3].menu_count = 1;
// 	button_caption[winPostionTeach][3].menu_text[0] = _T("关节坐标%");

 	// 位置管理:  菜单
	button_caption[winPostionTeach][4].menu_count   = 6;
	button_caption[winPostionTeach][4].menu_text[0] = _T("100%");
	button_caption[winPostionTeach][4].menu_text[1] = _T("50%");
	button_caption[winPostionTeach][4].menu_text[2] = _T("25%");
	button_caption[winPostionTeach][4].menu_text[3] = _T("10%");
	button_caption[winPostionTeach][4].menu_text[4] = _T("1%");
	button_caption[winPostionTeach][4].menu_text[5] = _T("0.1%");

	button_caption[winPostionTeach][5].menu_count   = 5;
	button_caption[winPostionTeach][5].menu_text[0] = _T("关节");
	button_caption[winPostionTeach][5].menu_text[1] = _T("直角");
	button_caption[winPostionTeach][5].menu_text[2] = _T("工具");
	button_caption[winPostionTeach][5].menu_text[3] = _T("用户");
	button_caption[winPostionTeach][5].menu_text[4] = _T("回到原点");
}

void ChildWindowButtonMenu::set_win_userdefine()
{
	// 
	button_caption[winUserDefine][0].button_text = _T("设置");
	button_caption[winUserDefine][1].button_text = _T("管理");
	button_caption[winUserDefine][2].button_text = _T("");
	button_caption[winUserDefine][3].button_text = _T("");
	button_caption[winUserDefine][4].button_text = _T("");
	button_caption[winUserDefine][5].button_text = _T("");
}

void ChildWindowButtonMenu::init_button_table()
{
	set_win_userdefine(); // first window

	//second setting -------------------------------------------------------------
	button_caption[winSetting][0].button_text = _T("重启软件");
	button_caption[winSetting][1].button_text = _T("修改参数");
	button_caption[winSetting][2].button_text = _T("撤销修改");
	button_caption[winSetting][3].button_text = _T("保存修改");
	button_caption[winSetting][4].button_text = _T("");
	button_caption[winSetting][5].button_text = _T("关闭");

	// 变量管理窗口------------------------------------------------------
	button_caption[winVariableData][0].button_text = _T("编辑");
	button_caption[winVariableData][1].button_text = _T("示教");
	button_caption[winVariableData][2].button_text = _T("检查");
	button_caption[winVariableData][3].button_text = _T("新建");
	button_caption[winVariableData][4].button_text = _T("删除");
	button_caption[winVariableData][5].button_text = _T("刷新");

	// 变量管理:  菜单
	button_caption[winVariableData][0].menu_count   = 4;
 	button_caption[winVariableData][0].menu_text[0] = _T("粘贴");
	button_caption[winVariableData][0].menu_text[1] = _T("复制");
	button_caption[winVariableData][0].menu_text[2] = _T("剪切");
	button_caption[winVariableData][0].menu_text[3] = _T("重命名");
 
	
	// 项目管理窗口--------------------------------------------
	button_caption[winProject][0].button_text = _T("新建");
	button_caption[winProject][1].button_text = _T("导入");
	button_caption[winProject][2].button_text = _T("备份");
	button_caption[winProject][3].button_text = _T("重命名");
	button_caption[winProject][4].button_text = _T("删除");
	button_caption[winProject][5].button_text = _T("导出");

	// 程序管理窗口---------------------------------------------
	button_caption[winProgram][0].button_text = _T("修改");
	button_caption[winProgram][1].button_text = _T("运行");
	button_caption[winProgram][2].button_text = _T("新建");
	button_caption[winProgram][3].button_text = _T("编辑");
	button_caption[winProgram][4].button_text = _T("保存");
	button_caption[winProgram][5].button_text = _T("高级");

	button_caption[winProgram][1].menu_count   = 3;
	button_caption[winProgram][1].menu_text[0] = _T("设置PC");
	button_caption[winProgram][1].menu_text[1] = _T("连续运行");
	button_caption[winProgram][1].menu_text[2] = _T("单步运行");

	//程序编辑菜单--------------------------------------------
	button_caption[winProgram][3].menu_count   = 6;
	button_caption[winProgram][3].menu_text[0] = _T("新建行");
	button_caption[winProgram][3].menu_text[1] = _T("删除行");
	button_caption[winProgram][3].menu_text[2] = _T("删除指令");
	button_caption[winProgram][3].menu_text[3] = _T("粘贴");
	button_caption[winProgram][3].menu_text[4] = _T("复制");
	button_caption[winProgram][3].menu_text[5] = _T("剪切");
 
	button_caption[winProgram][5].menu_count = 6; // 【高级】子菜单
	button_caption[winProgram][5].menu_text[0] = _T("键盘");
	button_caption[winProgram][5].menu_text[1] = _T("缩进");
	button_caption[winProgram][5].menu_text[2] = _T("退格");
	button_caption[winProgram][5].menu_text[3] = _T("查找");
	button_caption[winProgram][5].menu_text[4] = _T("加注释");
	button_caption[winProgram][5].menu_text[5] = _T("去注释");

	// 程序编辑子窗口---------------------------------------------
	button_caption[winEditCommand][0].button_text = _T("更改");
	button_caption[winEditCommand][1].button_text = _T("替换");
	button_caption[winEditCommand][2].button_text = _T("新增");
	button_caption[winEditCommand][3].button_text = _T("删除");
	button_caption[winEditCommand][4].button_text = _T("取消");
	button_caption[winEditCommand][5].button_text = _T("确定");


	// 位置管理窗口---------------------------------------------
	set_win_posteach();
 
	// 历史管理窗口--------------------------------------------------
	set_win_logfile();
  
	// 新建变量
	button_caption[winNewScriptVariable][0].button_text = _T("");
	button_caption[winNewScriptVariable][1].button_text = _T("");
	button_caption[winNewScriptVariable][2].button_text = _T("");
	button_caption[winNewScriptVariable][3].button_text = _T("");
	button_caption[winNewScriptVariable][4].button_text = _T("取消");
	button_caption[winNewScriptVariable][5].button_text = _T("确认");

	// 输入输出设备监控--------------------------------------------------
	button_caption[winInOutDeviece][0].button_text = _T("概览");
	button_caption[winInOutDeviece][1].button_text = _T("通用输入");
	button_caption[winInOutDeviece][2].button_text = _T("通用输出");
	button_caption[winInOutDeviece][3].button_text = _T("专用IO");
	button_caption[winInOutDeviece][4].button_text = _T("释放全部");
	button_caption[winInOutDeviece][5].button_text = _T("");

	// 参数设置 机构臂长-------------------------------------------------
	button_caption[winSettingMechanism][0].button_text = _T("重启软件");
	button_caption[winSettingMechanism][1].button_text = _T(" ");
	button_caption[winSettingMechanism][2].button_text = _T("撤销修改");
	button_caption[winSettingMechanism][3].button_text = _T("保存修改");
	button_caption[winSettingMechanism][4].button_text = _T("");
	button_caption[winSettingMechanism][5].button_text = _T("关闭");
	// 参数设置 电机控制-------------------------------------------------
	button_caption[winSettingJoints][0].button_text = _T("");
	button_caption[winSettingJoints][1].button_text = _T("");
	button_caption[winSettingJoints][2].button_text = _T("");
	button_caption[winSettingJoints][3].button_text = _T("");
	button_caption[winSettingJoints][4].button_text = _T("取消");
	button_caption[winSettingJoints][5].button_text = _T("确认");
	// 参数设置 原点设定-------------------------------------------------
	button_caption[winSettingOrigin][0].button_text = _T("定义原点");
	button_caption[winSettingOrigin][1].button_text = _T("重置所有");
	button_caption[winSettingOrigin][2].button_text = _T("重置选定");
	button_caption[winSettingOrigin][3].button_text = _T("读取数据");
	button_caption[winSettingOrigin][4].button_text = _T("回原点");
	button_caption[winSettingOrigin][5].button_text = _T("关闭");

	////示教点位窗口
	button_caption[winTeachDLG][0].button_text = _T(" ");
	button_caption[winTeachDLG][1].button_text = _T("示教");
	button_caption[winTeachDLG][2].button_text = _T("帮助");
	button_caption[winTeachDLG][3].button_text = _T(" ");
	button_caption[winTeachDLG][4].button_text = _T("允许");
	button_caption[winTeachDLG][5].button_text = _T("禁止");

	////用户坐标系窗口
	button_caption[winUserCoSysDLG][0].button_text = _T("");
	button_caption[winUserCoSysDLG][1].button_text = _T("");
	button_caption[winUserCoSysDLG][2].button_text = _T("");
	button_caption[winUserCoSysDLG][3].button_text = _T("");
	button_caption[winUserCoSysDLG][4].button_text = _T("");
	button_caption[winUserCoSysDLG][5].button_text = _T("设置");

	////工具坐标系窗口
	button_caption[winToolCoSysDLG][0].button_text = _T("");
	button_caption[winToolCoSysDLG][1].button_text = _T("");
	button_caption[winToolCoSysDLG][2].button_text = _T("");
	button_caption[winToolCoSysDLG][3].button_text = _T("");
	button_caption[winToolCoSysDLG][4].button_text = _T("");
	button_caption[winToolCoSysDLG][5].button_text = _T("设置");

	////码垛设置窗口
	button_caption[winPalletDLG][0].button_text = _T(" ");
	button_caption[winPalletDLG][1].button_text = _T("示教");
	button_caption[winPalletDLG][2].button_text = _T("帮助");
	button_caption[winPalletDLG][3].button_text = _T(" ");
	button_caption[winPalletDLG][4].button_text = _T("允许");
	button_caption[winPalletDLG][5].button_text = _T("禁止");

	////指令设置窗口
	button_caption[winInstructionDLG][0].button_text = _T("");
	button_caption[winInstructionDLG][1].button_text = _T(" ");
	button_caption[winInstructionDLG][2].button_text = _T(" ");
	button_caption[winInstructionDLG][3].button_text = _T(" ");
	button_caption[winInstructionDLG][4].button_text = _T("取消");
	button_caption[winInstructionDLG][5].button_text = _T("确定");

	////指令编辑窗口
	button_caption[winInstructionEdit][0].button_text = _T("变量");
	button_caption[winInstructionEdit][1].button_text = _T("示教");
	button_caption[winInstructionEdit][2].button_text = _T("");   // 添加
	button_caption[winInstructionEdit][3].button_text = _T("");   // 值
	button_caption[winInstructionEdit][4].button_text = _T("取消");
	button_caption[winInstructionEdit][5].button_text = _T("确定");

	button_caption[winInstructionEdit][0].menu_count = 4 ;
	button_caption[winInstructionEdit][0].menu_text[0] = _T("选择");
	button_caption[winInstructionEdit][0].menu_text[1] = _T("新建");
	button_caption[winInstructionEdit][0].menu_text[2] = _T("键盘");
	button_caption[winInstructionEdit][0].menu_text[3] = _T("删除：值");

	////变量新建、设置窗口
	button_caption[winVariableBuilder][0].button_text = _T("");
	button_caption[winVariableBuilder][1].button_text = _T("");
	button_caption[winVariableBuilder][2].button_text = _T("");
	button_caption[winVariableBuilder][3].button_text = _T("");
	button_caption[winVariableBuilder][4].button_text = _T("取消");
	button_caption[winVariableBuilder][5].button_text = _T("保存");

	////变量新建、设置窗口
	button_caption[winSecurityArea][0].button_text = _T("区域");
	button_caption[winSecurityArea][1].button_text = _T("");
	button_caption[winSecurityArea][2].button_text = _T("");
	button_caption[winSecurityArea][3].button_text = _T("设置");
	button_caption[winSecurityArea][4].button_text = _T("启用");
	button_caption[winSecurityArea][5].button_text = _T("禁用");

	// 示教面板管理窗口------------------------------------------------------
	button_caption[winTeachPanelDlg][0].button_text = _T("接通伺服");
	button_caption[winTeachPanelDlg][1].button_text = _T("开启示教");
	button_caption[winTeachPanelDlg][2].button_text = _T("切换坐标");
	button_caption[winTeachPanelDlg][3].button_text = _T("示教变量");
	button_caption[winTeachPanelDlg][4].button_text = _T("设置参数");
	button_caption[winTeachPanelDlg][5].button_text = _T("返回");

	// 示教面板管理窗口:  菜单
	button_caption[winTeachPanelDlg][2].menu_count   = 4;
	button_caption[winTeachPanelDlg][2].menu_text[0] = _T("关节");
	button_caption[winTeachPanelDlg][2].menu_text[1] = _T("直角");
	button_caption[winTeachPanelDlg][2].menu_text[2] = _T("工具");
	button_caption[winTeachPanelDlg][2].menu_text[3] = _T("用户");
	 
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
	// 程序管理窗口---------------------------------------------
	button_caption[winProgram][0].button_text = _T("");
	button_caption[winProgram][1].button_text = _T("继续");
	button_caption[winProgram][2].button_text = _T("单步");
	button_caption[winProgram][3].button_text = _T("");
	button_caption[winProgram][4].button_text = _T("暂停");
	button_caption[winProgram][5].button_text = _T("终止");

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