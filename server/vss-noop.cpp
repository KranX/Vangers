//
// Created by caiiiycuk on 22.06.22.
//
void sys_initScripts(const char*) {}
bool sys_readyQuant() { return true; }
void sys_runtimeObjectQuant(int) {}
extern "C" const char* sys_fileOpenQuant(const char* file, unsigned flags)  {
	return file;
}
