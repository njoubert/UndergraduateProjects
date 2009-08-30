#include "main.h"

using namespace std;


void testTokenizer(int argc, char *argv[]) {
	Tokenizer myTokenizer(argv[1]);
	cout << "Parsed " << myTokenizer.tokensLeft() << " tokens." << endl;
	cout << "hit ENTER for each token." << endl << endl;
	int i = 0;
	while (myTokenizer.hasMoreTokens()) {
		cin.get();
		cout << i << "] |" << myTokenizer.getNextToken() << "|" << endl;;
		i++;
	}
}


int main(int argc, char *argv[]) {
	if (argc != 2) {
		cout << "You need to supply a config file." << endl;
		return 0;
	}
	//testTokenizer(argc, argv);
	ConfigParser confParser(argv[1]);
	confParser.parseTokens();
}
