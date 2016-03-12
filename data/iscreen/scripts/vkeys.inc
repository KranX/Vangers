/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON	  -1
#define VK_RBUTTON	  -2
#define VK_MOVE 	  -3

#define VK_ESCAPE	  0x29
#define VK_SPACE	  0x2C
#define VK_RETURN	  0x28
#define VK_TAB		  0x2B

#define VK_LEFT 	  0x50
#define VK_UP		  0x52
#define VK_RIGHT	  0x4F
#define VK_DOWN 	  0x51

#define VK_1		0x1E
#define VK_2		0x1F
#define VK_3		0x20

#define VK_F1		  0x3A
#define VK_F2		  0x3B
#define VK_F3		  0x3C

#define VK_PRIOR	  0x9D
#define VK_NEXT 	  0x22

/* not need */
#define VK_CANCEL	  0x03
#define VK_MBUTTON	  0x04	  /* NOT contiguous with L & RBUTTON */

#define VK_BACK 	  0x08


#define VK_CLEAR	  0x0C


#define VK_SHIFT	  0x130
#define VK_CONTROL	  0x11
#define VK_MENU 	  0x12
#define VK_PAUSE	  0x13
#define VK_CAPITAL	  0x14






#define VK_END		  0x23
#define VK_HOME 	  0x24



#define VK_SELECT	  0x29
#define VK_PRINT	  0x2A
#define VK_EXECUTE	  0x2B
#define VK_SNAPSHOT	  0x2C
#define VK_INSERT	  0x2D
#define VK_DELETE	  0x7F
#define VK_HELP 	  0x2F

/* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
#define VK_0		0x30



#define VK_4		0x34
#define VK_5		0x35
#define VK_6		0x36
#define VK_7		0x37
#define VK_8		0x38
#define VK_9		0x39

/* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */

#define VK_LWIN 	  0x5B
#define VK_RWIN 	  0x5C
#define VK_APPS 	  0x5D

#define VK_NUMPAD0	  0x60
#define VK_NUMPAD1	  0x61
#define VK_NUMPAD2	  0x62
#define VK_NUMPAD3	  0x63
#define VK_NUMPAD4	  0x64
#define VK_NUMPAD5	  0x65
#define VK_NUMPAD6	  0x66
#define VK_NUMPAD7	  0x67
#define VK_NUMPAD8	  0x68
#define VK_NUMPAD9	  0x69
#define VK_MULTIPLY	  0x6A
#define VK_ADD		  0x6B
#define VK_SEPARATOR	  0x6C
#define VK_SUBTRACT	  0x6D
#define VK_DECIMAL	  0x6E
#define VK_DIVIDE	  0x6F

#define VK_F4		  0x73
#define VK_F5		  0x74
#define VK_F6		  0x75
#define VK_F7		  0x76
#define VK_F8		  0x77
#define VK_F9		  0x78
#define VK_F10		  0x79
#define VK_F11		  0x7A
#define VK_F12		  0x7B
#define VK_F13		  0x7C
#define VK_F14		  0x7D
#define VK_F15		  0x7E
#define VK_F16		  0x7F
#define VK_F17		  0x80
#define VK_F18		  0x81
#define VK_F19		  0x82
#define VK_F20		  0x83
#define VK_F21		  0x84
#define VK_F22		  0x85
#define VK_F23		  0x86
#define VK_F24		  0x87

#define VK_NUMLOCK	  0x90
#define VK_SCROLL	  0x91

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT	  0x130
#define VK_RSHIFT	  0x12F
#define VK_LCONTROL	  0x132
#define VK_RCONTROL	  0x131
#define VK_LMENU	  0xA4
#define VK_RMENU	  0xA5

#define VK_ATTN 	  0xF6
#define VK_CRSEL	  0xF7
#define VK_EXSEL	  0xF8
#define VK_EREOF	  0xF9
#define VK_PLAY 	  0xFA
#define VK_ZOOM 	  0xFB
#define VK_NONAME	  0xFC
#define VK_PA1		  0xFD
#define VK_OEM_CLEAR	  0xFE

// Some additional virtual keycodes...
#define VK_OEM_COMMA	0xBC
#define VK_OEM_PERIOD	0xBE
#define VK_OEM_SCROLL	0x91
#define VK_OEM_MINUS	0xBD
#define VK_OEM_5	0xDC
#define VK_OEM_PLUS	0xBB
#define VK_OEM_ALT	0xF2

#define VK_SLASH	0x2F
#define VK_BKSLASH	0x5C
#define VK_TILDE	0x60
#define VK_LBR		0x5B
#define VK_RBR		0x5D

#define 	VK_A	0x61
#define 	VK_B	0x62
#define 	VK_C	0x63
#define 	VK_D	0x64
#define 	VK_E	0x65
#define 	VK_F	0x66
#define 	VK_G	0x67
#define 	VK_H	0x68
#define 	VK_I	0x69
#define 	VK_J	0x6A
#define 	VK_K	0x6B
#define 	VK_L	0x6C
#define 	VK_M	0x6D
#define 	VK_N	0x6E
#define 	VK_O	0x6F
#define 	VK_P	0x70
#define 	VK_Q	0x71
#define 	VK_R	0x72
#define 	VK_S	0x73
#define 	VK_T	0x74
#define 	VK_U	0x75
#define 	VK_V	0x76
#define 	VK_W	0x77
#define 	VK_X	0x78
#define 	VK_Y	0x79
#define 	VK_Z	0x7A

