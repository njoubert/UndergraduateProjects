#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;


//LINUX LIBRARIES
#ifdef _POSIX

#endif

//OSX LIBRARIES
#ifdef OSX

#else

#endif

//WINDOWS ONLY LIBRARIES
#ifdef _WIN32

#else

#endif

/*
 * Tokenizer is ONLY responsible for returning tokens (' ' and '\t' deliminated strings) and ignoring comments.
 * Newlines are PRESERVED as a token, to give you the ability to parse by newlines.
 * Anything inside quotes are perfectly preserved.
 */
class Tokenizer {
public:
	Tokenizer(string filename) {
		  myfile.open(filename.c_str(), ios::in);
		  currentTokenIndex = 0;
		  tokenize();
	}
	~Tokenizer() {
		  myfile.close();
	}
	bool hasMoreTokens() {
		return (currentTokenIndex < tokens.size());
	}
	int tokensLeft() {
		return (tokens.size() - currentTokenIndex);
	}
	string getNextToken() {
		if (currentTokenIndex == tokens.size()) {
			cout << "Attempted to read more tokens than existed." << endl;
			exit(1);
		}
		currentTokenIndex++;
		return tokens[currentTokenIndex - 1];
	}
	string peekNextToken() {
		if (currentTokenIndex == tokens.size()) {
			cout << "Attempted to read more tokens than existed." << endl;
			exit(1);
		}
		return tokens[currentTokenIndex];
	}

private:
	//break up into tokens
	void tokenize() {
		//LOOP:
		while (myfile.good()) {
		//skip whitespace
			skipWhitespace();
			while (myfile.peek() == '#') {
				skipLine();
				skipWhitespace();
			}
			if (myfile.good())
				tokens.push_back(readUntilWhitespace());
		}
	}

	void skipWhitespace() {
		while (myfile.peek() == ' ' ||
				myfile.peek()  == '\t') {
			myfile.get();
		}
	}
	void skipLine() {
		while (myfile.peek() != '\n' && myfile.good()) {
			myfile.get();
		}
	}
	string readUntilWhitespace() {
		char buff[500];
		int i=0;
		if (myfile.peek() == '\n') {

			myfile.get();
			return string("\n");

		} else if (myfile.peek() == '\"') {

			myfile.get();
			while (myfile.peek() != '\"' && myfile.good()) {
				buff[i] = (char) myfile.get();
				i++;
			}
			myfile.get();
			buff[i] = 0;
			return string(buff);

		} else {

			while (myfile.peek() != ' ' && myfile.peek() != '\t' && myfile.peek() != '\n' && myfile.good()) {
				buff[i] = (char) myfile.get();
				i++;
			}
			buff[i] = 0;
			return string(buff);

		}
	}

	ifstream myfile;
	vector<string> tokens;
	unsigned int currentTokenIndex;
};

class ConfigParser {
public:
	ConfigParser(string filename): myTokenizer(filename) {
		parseTokens();
	}

	void parseTokens() {
		while (myTokenizer.hasMoreTokens()) {
			string nextToken = myTokenizer.peekNextToken();
			if (nextToken[0] == '\n') {

				myTokenizer.getNextToken(); //skip here.

			} else if (nextToken[0] == '[') {

				cout << "-- starting group." << endl;

				//start of a group
				string group = myTokenizer.getNextToken();
				string groupStart = myTokenizer.getNextToken();
				if (groupStart != "{") {
					cout << "EXPECTED THE START OF A GROUP, BUT INSTEAD GOT: " << groupStart << endl;
					exit(1);
				}
				parseTokens();

			} else if (nextToken[0] == '}') {
				cout << "-- ending group." << endl;
				//end of a group
				myTokenizer.getNextToken();
				return;
			} else {
				cout << "-- starting assignment." << endl;

				//assignment. Keep reading until we find '='
				stringstream name;
				while (myTokenizer.peekNextToken() != "=") {
					name << myTokenizer.getNextToken();
				}
				myTokenizer.getNextToken(); //pull the =
				//keep reading until we find the '\n'
				stringstream value;
				while (myTokenizer.peekNextToken() != "\n") {
					value << myTokenizer.getNextToken();
				}
				myTokenizer.getNextToken(); //pull the \n
				cout << "ASSIGNMENT: " << name.str() << " := " << value.str() << endl;
			}
		}
	}

private:
	Tokenizer myTokenizer;
};

