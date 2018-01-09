//
// Created by nikita on 11/30/17.
//

#ifndef VANGERS_COMMADLINEOPTIONS_H
#define VANGERS_COMMADLINEOPTIONS_H


class CommandLineOptions {
public:
	void parseArgs(int argc, char **argv);

	static void printUsage();

	int getScreenWidth() const;

	int getScreenHeight() const;

	CommandLineOptions();

private:
	int screenWidth;
	int screenHeight;

};


#endif //VANGERS_COMMADLINEOPTIONS_H
