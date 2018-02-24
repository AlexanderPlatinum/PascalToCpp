#pragma once

#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <regex>

enum {
	WRITELN = 1,
	WRITE,
	READ,
	READLN,
	INTEGER_ARR,
	INTEGER_VAR,
	EQUALS,
	IF,
	FOR
};

class Translator
{
private:

	std::ifstream fin;
	std::ofstream fout;
	
	int begin_count;
	int line;

	std::map<int, std::string> error_data;
	std::vector<std::string> errors;

	void HandleBegin();
	void HandleEnd();
	void HandleEndOfProgram();
	void HandleWriteln( std::string str, bool new_line);
	void HandleEquals( std::string str );
	void HandleIf( std::string str );
	void HandleFor( std::string str );
	void HandleArray( std::string str );
	void HandleInteger( std::string str );
	void HandleRead( std::string str, bool is_new_line );

	std::string ChekingCommnent( std::string str );
	void RenderTabs();

	void ParseString( std::string str );
	void PrepareFile();

	void LoadErrors();
	void AddError( int line, int type );
	void PrintErrors();
	
public:
	Translator();
	
	void Initialize( std::string input_file, std::string output_file );
	void Run();

};

