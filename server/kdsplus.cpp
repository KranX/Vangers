
#include "kdsplus.h"
#include <chrono>
#include <thread>

#define VERSION  "1.9"
/* --------------------------- PROTOTYPE SECTION --------------------------- */
void ComlineAnalyze(int argc,char** argv);
void terminal();

/* --------------------------- DEFINITION SECTION -------------------------- */
struct KDsPlus : XRuntimeObject
{
	virtual int Quant(void);
};

int frame = 0;
int broadcast_port = 1992;
int default_server_port = 2197;
int time_to_live = 0;
int ErrHExcept = 1;
int GlobalExit = 0;
int terminal_log = 0;
char* result_name = 0;
int StatLogging = 0;
int leave_empty_games = 0;
Server* server;

int xtInitApplication(void) {
#ifdef _WIN32
	SetConsoleTitle("Vangers Server ");
#else
	std::cout << "\033]0;" << "Vangers Server " << "\007";
#endif
	std::cout << "Multiplayer VANGERS Server by K-D LAB\nRelease Version " VERSION " (c) 1998 All Rights Reserved\n";
	std::cout << "Compilation: DATE: " <<  __DATE__ << " TIME: " << __TIME__ << "\n\n";

	//SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	ComlineAnalyze(__argc,__argv);
//	ErrH.SetFlags(XERR_CTRLBRK);

	if(!XSocketInit())
		ErrH.Abort("WinSock initialization failed, check Your TCP/IP settings");

//	if(terminal_log) {
//		terminal();
//		return -1;
//	}

	server = new Server(default_server_port, broadcast_port, time_to_live);

#ifdef _DEBUG
	if(result_name) {
		(server -> create_game()) -> load_result(server,result_name);
		ErrH.Abort("Result loaded");
	}
#endif

	KDsPlus* p = new KDsPlus;
	
	xtCreateRuntimeObjectTable();
	xtRegisterRuntimeObject(p);

	return 1;
}

void xtDoneApplication(void) {
	delete server;
}

void Syncro() {
	const int dt_total = 1000/64;
	static int t_prev = 0;
	int dt = dt_total - (clock() - t_prev);
	if(dt > 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(dt));
	t_prev = clock();
}

int KDsPlus::Quant(void) {
	if(GlobalExit || XKey.Pressed(VK_ESCAPE)) return XT_TERMINATE_ID;
	frame++;
	Syncro();
	server -> quant();
	DBGCHECK
	return 0;
}

void ComlineAnalyze(int argc,char** argv) {
	int i,j;
	for(i = 1;i < argc;i++)
		if(argv[i][0] == '/'){
			j = 0;
			while(argv[i][j] == '/' || argv[i][j] == '-'){
				switch(argv[i][j + 1]){
					case 'p':
						default_server_port = atoi(argv[i] + (j + 2));
						break;
					case 'b':
						if(argv[i][j + 2] == '-')
							broadcast_port = 0;
						else
							broadcast_port = atoi(argv[i] + (j + 2));
						break;
					case 't':
						time_to_live = atoi(argv[i] + (j + 2));
						std::cout << "Time to Live: " << time_to_live << " second\n";
						break;
					case 'i':
						FreeConsole();
						break;
					case 's':
						std::cout << "Logging ON\n";
						StatLogging = 1;
						break;
					case 'l':
						std::cout << "Empty games will not be removed\n";
						leave_empty_games = 1;
						break;
					case 'h':
					case '?':
						std::cout << "Switches: \n  /pxxxx  -  main TCP/IP port \n  /bxxxx  -  UDP broadcast port \n  /b-     -  supress UDP broadcast \n  /i      -  invisible mode \n  /txxx   -  time to live without clients, seconds\n  /s      -  creates log file (VangersServer.log)\n  /l      -  leave empty games\n\n";
						break;
					}
				j+=2;
				}
			}
		else
			result_name = argv[i];
}

/*
void terminal()
{
	XCon < "MP TERMINAL:\n\n";
	XSocket sock;
	//if(!sock.open_by_socks5("etype0.enet.ru",1973,"toothelator"))
	if(!sock.open_by_socks5("mike.nevalink.ru",1973,"toothelator"))
	//if(!sock.open("localhost",1973))
	//if(!sock.open("mike.nevalink.ru",1973))
		return;
	OutputEventBuffer out(100);
	XBuffer in(2000);
	out.begin_event(META_OPEN_LOG);
	out.end_event();
	out.send(sock);
	int counter = 0;
	while(sock()){
		int len = sock.receive(in,in.length());
		in[len] = 0;
		XCon < in < " " <= (counter++ % 10) < " " < "\b\b\b";
		in.init();
		Syncro();
		}
}

*/
