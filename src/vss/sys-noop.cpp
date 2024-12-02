//
// Created by caiiiycuk on 02.12.24.
//

void sys_initScripts(const char*) {
}

bool sys_readyQuant() {
	return true;
}

void sys_runtimeObjectQuant(int) {
}

void sys_tickQuant() {
}

extern "C" const char* sys_fileOpenQuant(const char* file, unsigned) {
	return file;
}
