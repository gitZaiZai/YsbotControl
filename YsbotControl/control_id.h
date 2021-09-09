#pragma once

enum ControlID {
	IDR_MAINFRAME = 128,
	IDD_YSBOTCONTROL_DIALOG = 2800,

	LEFT_DIALOG_BUTTON1 = 3001,         // ���Ի����һ����ť
	LEFT_DIALOG_BUTTON2 = 3002,         // ���Ի���ڶ�����ť
	LEFT_DIALOG_BUTTON3 = 3003,         // ���Ի����������ť
	LEFT_DIALOG_BUTTON4 = 3004,         // ���Ի�����ĸ���ť
	LEFT_DIALOG_BUTTON5 = 3005,         // ���Ի���������ť
	LEFT_DIALOG_BUTTON6 = 3006,         // ���Ի����������ť
	LEFT_DIALOG_BUTTON7 = 3007,         // ���Ի�����߸���ť
	RIGHT_DIALOG_LABEL1 = 3008,         // �Ҳ�Ի����һ����ǩ
	RIGHT_DIALOG_LABEL2 = 3009,         // �Ҳ�Ի���ڶ�����ǩ
	RIGHT_DIALOG_LABEL3 = 3010,         // �Ҳ�Ի����������ǩ
	RIGHT_DIALOG_LABEL4 = 3011,         // �Ҳ�Ի�����ĸ���ǩ
	RIGHT_DIALOG_LABEL5 = 3012,         // �Ҳ�Ի���������ǩ
	RIGHT_DIALOG_LABEL6 = 3013,         // �Ҳ�Ի����������ǩ
	RIGHT_DIALOG_LABEL7 = 3014,         // �Ҳ�Ի�����߸���ǩ
	BOTTOM_DIALOG_BUTTON1 = 3015,       // �·��Ի����һ����ť
	BOTTOM_DIALOG_BUTTON2 = 3016,       // �·��Ի���ڶ�����ť
	BOTTOM_DIALOG_BUTTON3 = 3017,       // �·��Ի����������ť
	BOTTOM_DIALOG_BUTTON4 = 3018,       // �·��Ի�����ĸ���ť
	BOTTOM_DIALOG_BUTTON5 = 3019,       // �·��Ի���������ť
	BOTTOM_DIALOG_BUTTON6 = 3020,       // �·��Ի����������ť
	BOTTOM_DIALOG_RETURN = 3021,        // �·��Ի��򷵻ذ�ť
	LOCATION_DIALOG_GRID = 3022,        // λ�ù��������еı���ؼ�
	LOCATION_DIALOG_ROBOT_NAME = 3023,  // λ�ù��������еĻ���������
	LOCATION_DIALOG_COORDINATE = 3024,  // λ�ù��������еĲ���ϵ
	LOCATION_DIALOG_TOOL = 3025,        // λ�ù��������еĹ���
	LOCATION_DIALOG_ROBOT_SPEED = 3025, // λ�ù��������еĻ������ٶ�
	LOCATION_DIALOG_MODE = 3026,        // λ�ù��������е�ģʽ
	LOCATION_DIALOG_JOG_SPEED = 3027,   // λ�ù��������еĵ㶯�ٶ�
	VARIABLE_DIALOG_TREE = 3028,        // �������������е����νṹ
	PARAMETER_DIALOG_TREE = 3029,        //�������������е����νṹ
	VARIABLE_RADIO_BUTTON_1 = 3030,
	VARIABLE_RADIO_BUTTON_2 = 3031,
	VARIABLE_RADIO_BUTTON_3 = 3033,
	VARIABLE_COMBOBOX_TYPE  = 3034,
	IDD_CUSTOM_POPUP_DLG    = 3035,
	NEW_VARIABLE_COMBOBOX_TYPE    = 3036,
	STATE_DIALOG = 3100,                // ״̬���Ի���
	IDS_ROBOT_STATE    = 3101,          // ������״̬
	IDS_OPERATION_MODE = 3102,          // ����ģʽ
 	IDS_COORDINATE_SYSTEM = 3103,       // ����ϵ״̬
 	IDS_TOOL = 3104,                    // ������
	IDS_ROBOT_SPEED = 3105,             // �������ٶ�
	IDS_LOGO = 3106,                    // ��˾��־
 	IDS_PROJECT_NAME = 3107,            // ��Ŀ�ļ�����
	IDS_PROGRAM_STATE = 3108,           // ��������״̬
	IDS_RUNNING_MODE = 3109,            // ����ģʽ
	IDS_RESERVE_1 = 3110,            // ����ģʽ
	IDS_RESERVE_2 = 3111,            // ����ģʽ
	IDS_RESERVE_3 = 3112,            // ����ģʽ
	IDS_SPACE_WATCH = 3113,             // �ռ���
	IDS_SAFE_STATE = 3114,              // ��ȫ��·״̬
	IDS_USER_GRADE = 3115,              // �û��ȼ�
	IDS_SYSTEM_TIME = 3116,             // ϵͳʱ��
	IDS_PROMPT = 3117,                  // ��Ϣ��ʾͼ��
 	IDC_PROMPT_CONFIRM = 3119,          // ��Ϣȷ��
	IDB_COORDINATE_WORLD = 3120,
	IDI_PROMPT_CONFIRM = 3121,
	IDB_AUTO_MODE = 3122,
	IDB_MANUAL_MODE = 3123,
	IDB_EXTERNAL_AUTO_MODE = 3124,
	IDB_ROBOT_STATE1 = 3125,
	IDB_ROBOT_STATE2 = 3126,
	IDB_TOOL = 3127,
	IDB_PROJECT = 3128,
	IDB_PROGRAM_RUN = 3129,
	IDB_PROGRAM_PAUSE = 3130,
	IDB_PROGRAM_STOP = 3131,
	IDB_PROGRAM_RETURN = 3132,
	IDB_RUN_CONTINUE = 3133,
	IDB_RUN_STEP = 3134,
	IDB_RUN_MOTION = 3135,
	IDB_SPACE_WATCH = 3136,
	IDB_LOOP_CONNECT = 3137,
	IDB_LOOP_DISCONNECT = 3138,
	IDB_INFO_PROMPT = 3139,
	IDB_WARNING_PROMPT = 3140,
	IDB_ERROR_PROMPT = 3141,
	IDB_COORDINATE_JOINT = 3142,
	IDB_COORDINATE_TOOLS = 3143,
    IDI_VARIABLE_GLOBAL = 3144,
    IDI_VARIABLE_PROGRAM = 3145,
    IDI_VARIABLE_PROJECT = 3146,
    IDI_VARIABLE_SYSTEM  = 3147,
	IDI_OVERLAY1         = 3148,
	 
	IDD_DLGSCRIPTCOMMANDEDIT         = 3149,
	IDE_DLGSCRIPTCOMMANDEDIT_EDIT    = 3150,
	IDE_DLGSCRIPTCOMMANDEDIT_BTLEFT  = 3151,
	IDE_DLGSCRIPTCOMMANDEDIT_BTRIGHT = 3152,
	IDE_DLGSCRIPTCOMMANDEDIT_RADIO1  = 3152,
	IDE_DLGSCRIPTCOMMANDEDIT_RADIO2  = 3153,

	IDC_BOX_STATE   = 3154,
	IDC_BOX_MAIN    = 3155,
	IDC_BOX_RIGHT   = 3156,
	IDC_BOX_BOTTONS = 3157,
	IDC_BOX_MENUDLG  = 3158,
	IDC_BOX_SECONDMENU  = 3159,

	IDC_PROGRAM_EDIT = 3200,            // �������֮�༭��
    IDL_VARIABLE_KIND = 3201,
    IDL_VARIABLE_TYPE = 3202,

	IDB_SETTINGDLG_MechGroup = 3203,
	IDC_VERT_SCROLLBAR       = 3204,
	IDC_IO_REGION            = 3205,
	IDC_MAIN_MENU_BTN        = 3206,          
	IDC_FIRSTMENU_BTN1        = 3207,          
	IDC_FIRSTMENU_BTN2        = 3208,          
	IDC_FIRSTMENU_BTN3        = 3209,          
	IDC_FIRSTMENU_BTN4        = 3210,          
	IDC_FIRSTMENU_BTN5        = 3211,          
	IDC_FIRSTMENU_BTN6        = 3212,          
	IDC_FIRSTMENU_BTN7        = 3213, 
	IDC_FIRSTMENU_BTN8        = 3214, 


 	WM_MESSAGEBOARD      = 3300,
	WM_BUTTON_PRESS      = 3301, 
    WM_MESSAGE_CONFIRM   = 3302,
	WM_POPUPMENU_COMMAND = 3303,
	WM_ACTIVATE_WINDOW   = 3304,
	WM_KEYBOARD_ENTER    = 3305, 
	WM_UPDATE_COMMBOX    = 3306, 
	WM_RESIZE_GRID       = 3307, 	 
	WM_REBOOT_SYSTEM     = 3308, 	 
	WM_SAVE_PROJECT      = 3309, 
	WM_PENDANT_KEYPRESS  = 3310,
	WM_2ND_CLICKED       = 3311,
	WM_GET_HOMEPOS       = 3312,

	WM_ACQUIRE_KEY_INPUT       = 3313,
	WM_RELEASE_KEY_INPUT       = 3314,
	WM_CUSTOMCOMBOX_SELCHANGED = 3315,
	WM_TEACHDLG_MOVETYPE       = 3316,
	WM_KEYBOARD_CANCLE         = 3317, 
	WM_UPDATE_BUTTONTEXT       = 3318,
	WM_MENU_CLICK              = 3319, 
	WM_EXIT_SYSTEM             = 3320,
	WM_LEFT_KEY_PRESS          = 3321,
	WM_SET_PCKEY_DATA          = 3322,

	IDC_SECONDMENU_BTN1        = 3330,          
	IDC_SECONDMENU_BTN2        = 3331,          
	IDC_SECONDMENU_BTN3        = 3332,          
	IDC_SECONDMENU_BTN4        = 3333,          
	IDC_SECONDMENU_BTN5        = 3334,          
	IDC_SECONDMENU_BTN6        = 3335,          
	IDC_SECONDMENU_BTN7        = 3336,          
	IDC_SECONDMENU_BTN8        = 3337,          
	IDC_SECONDMENU_BTN9        = 3338,          
	IDC_SECONDMENU_BTN10       = 3339,          
	IDC_SECONDMENU_BTN11       = 3340, 
	IDC_SECONDMENU_BTN12       = 3341, 
	IDC_POWER_OFF_BTN          = 3342, 
	IDC_POWER_ON_BTN           = 3343, 

	IDC_TEACH_JOG_LEFT1        = 3345, 
	IDC_TEACH_JOG_LEFT2        = 3346, 
	IDC_TEACH_JOG_LEFT3        = 3347, 
	IDC_TEACH_JOG_LEFT4        = 3348, 
	IDC_TEACH_JOG_LEFT5        = 3349, 
	IDC_TEACH_JOG_LEFT6        = 3350, 
	IDC_TEACH_JOG_RIGHT1        = 3351, 
	IDC_TEACH_JOG_RIGHT2        = 3352, 
	IDC_TEACH_JOG_RIGHT3        = 3353, 
	IDC_TEACH_JOG_RIGHT4        = 3354, 
	IDC_TEACH_JOG_RIGHT5        = 3355, 
	IDC_TEACH_JOG_RIGHT6        = 3356, 

    WM_EDIT_MESSAGE      = 6900,

	HOTKEY_SHIFT_0   = 4000,
	HOTKEY_CTRL_0    = 4050,
	HOTKEY_ALT_0     = 4100,
 	HOTKEY_ALTCTRL_0 = 4150,
 
};