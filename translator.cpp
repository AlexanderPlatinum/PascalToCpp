#include "translator.h"

Translator::Translator() {
	begin_count = 0;
	line = 0;
}

void Translator::HandleBegin() {
	if ( begin_count == 0 ) {
		fout << std::endl;
		fout << "int main ( int argc, char **argv ) {" << std::endl;
		fout << std::endl;
	} else {
		fout << "{" << std::endl;
	}
	begin_count++;
}

void Translator::HandleEnd() {
	begin_count--;
	RenderTabs();
	fout << "}" << std::endl;
}

void Translator::RenderTabs() {
	for ( int i = 0; i < begin_count; i++ ) {
		fout << '\t';
	}
}

void Translator::HandleEndOfProgram() {
	fout << std::endl;
	fout << "\treturn 0;" << std::endl;
	fout << "}" << std::endl;
}

void Translator::HandleWriteln( std::string str, bool new_line ) {
	fout << "cout << ";

	std::smatch match;
	std::regex my_string;

	if ( new_line ) {
		my_string = "writeln(.+);";
	}
	else {
		my_string = "write(.+);";
	}

	if (std::regex_match(str, match, my_string)) {
		std::string tmp = match[1].str();

		for (int i = 0; i < tmp.size(); i++) {
			if ((tmp[i] == '(' ) || (tmp[i] == ')')) {
				continue;
			}

			if ( tmp[i] == ',' ) {
				fout << " << ";
				continue;
			}

			if (tmp[i] == '\'') {
				fout << '"';
				continue;
			}

			fout << tmp[i];
		}
	}
	else {
		if (new_line) {
			AddError( line, WRITELN );
		}
		else {
			AddError( line, WRITE );
		}
	}

	if ( new_line ) {
		fout << " << endl;";
	}
	else {
		fout << ";";
	}

	fout << std::endl;
}

void Translator::HandleEquals( std::string str ) {
	std::regex equals(":=");
	std::string replase = "=";
	fout << std::regex_replace( str, equals, replase ) << std::endl;
}

void Translator::HandleIf( std::string str ) {
	fout << "if ( ";

	std::smatch match;
	std::regex my_string("if \((.+)\) then");

	if (std::regex_match(str, match, my_string)) {
		fout << match[1].str() << " )";
	} else {
		AddError( line, IF );
	}

	fout << std::endl;
}

void Translator::HandleFor( std::string str ) {

	fout << std::endl;
	RenderTabs();

	std::smatch match;
	std::regex my_string("for (.+):=(.+) to (.+) do");

	if (std::regex_match(str, match, my_string)) {
		std::string tmp_counter = match[1].str();
		std::string tmp_begin = match[2].str();
		std::string tmp_to_end = match[3].str();

		fout << "for ( " << tmp_counter << "=" << tmp_begin << "; " << tmp_counter << "<= " << tmp_to_end << "; " << tmp_counter << "++ )" << std::endl;
	}
	else {
		AddError( line, FOR );
	}

}

void Translator::HandleArray( std::string str ) {

	for ( int i = 0; i < str.size(); i++ ) {
		if ( str[i] == '[') {
			str[i] = ':';
		}

		if ( str[i] == ']' ) {
			str[i] = ':';
		}
	}

	std::smatch match;
	std::regex my_string("var (.+): array :1..(.+): of integer;");

	if (std::regex_match(str, match, my_string)) {
		std::string arr_name = match[1].str();
		std::string arr_lenght = match[2].str();

		int lenght = std::stoi( arr_lenght ) + 1;

		fout << "int " << arr_name << "[" << lenght << "];" << std::endl;
	}
	else {
		AddError( line, INTEGER_ARR );
	}
}

void Translator::HandleInteger( std::string str ) {
	std::smatch match;
	std::regex my_string("(.+): integer;");

	if (std::regex_match(str, match, my_string)) {
		fout << "int " << match[1] << ";" << std::endl;
	}
	else {
		AddError(line, INTEGER_VAR);
	}
}

std::string Translator::ChekingCommnent( std::string str ) {

	if ( str.size() <= 1 ) {
		return std::string("");
	}

	std::stringstream ss;
	for (int i = 0; i < str.size(); i++) {
		if ( str[i] == '{' ) {
			ss << "/*";
		}
		else if ( str[i] == '}' ) {
			ss << "*/";
		}
		else {
			ss << str[i];
		}
	}

	std::string tmp = ss.str();

	for (int i = 0; i < tmp.size() - 1; i++) {
		if ( (tmp[i] == '(') && (tmp[i + 1] == '*') ) {
			tmp[i] = '/';
		}

		if ( (tmp[i] == '*' ) && (tmp[i + 1] == ')') ) {
			tmp[i + 1] = '/';
		}
	}

	return tmp;
}

void Translator::HandleRead( std::string str, bool is_new_line ) {
	fout << "cin >> ";

	std::smatch match;
	std::regex my_string;

	if ( is_new_line ) {
		my_string = "readln(.+);";
	}
	else {
		my_string = "read(.+);";
	}

	if (std::regex_match(str, match, my_string)) {
		std::string tmp = match[1].str();

		for (int i = 0; i < tmp.size(); i++) {
			if ((tmp[i] == '(') || (tmp[i] == ')')) {
				continue;
			}

			if (tmp[i] == ',') {
				fout << " >> ";
				continue;
			}

			fout << tmp[i];
		}
	}
	else {
		if ( is_new_line ) {
			AddError(line, READLN);
		}
		else {
			AddError(line, READ);
		}
	}

	fout << ";";
	fout << std::endl;
}

void Translator::ParseString( std::string str ) {

	std::regex begin( "begin" );
	std::regex end("end;");
	std::regex endDot( "end." );
	std::regex writeln( "writeln" );
	std::regex write("write");
	std::regex equals( ":=" );
	std::regex _if("if");
	std::regex _for("for");
	std::regex _array("array");
	std::regex integer("integer");
	std::regex program("program");
	std::regex read("read");
	std::regex readln("readln");

	str = ChekingCommnent( str );

	bool triger = false;

	//end;
	if (std::regex_search(str, end)) {
		HandleEnd();
		triger = true;
		return;
	}

	//end.
	if (std::regex_search(str, endDot)) {
		HandleEndOfProgram();
		triger = true;
		return;
	}

	RenderTabs();

	// begin
	if ( std::regex_search(str, begin) ) {
		HandleBegin();
		triger = true;
	}

	// writeln
	if (std::regex_search(str, writeln)) {
		HandleWriteln(str, true);
		triger = true;
		return;
	}

	// write
	if (std::regex_search(str, write)) {
		HandleWriteln(str, false);
		triger = true;
		return;
	}

	// if
	if (std::regex_search(str, _if)) {
		HandleIf(str);
		triger = true;
	}

	// for
	if (std::regex_search(str, _for)) {
		HandleFor(str);
		triger = true;
		return;
	}

	// array
	if (std::regex_search(str, _array)) {
		HandleArray(str);
		triger = true;
		return;
	}

	// integer
	if (std::regex_search(str, integer)) {
		HandleInteger(str);
		triger = true;
		return;
	}

	// :=
	if (std::regex_search(str, equals)) {
		HandleEquals(str);
		triger = true;
	}

	// program
	if (std::regex_search(str, program)) {
		fout << "/* " << str << " */" << std::endl;
		triger = true;
	}

	// readln 
	if (std::regex_search(str, readln)) {
		HandleRead( str, true );
		triger = true;
		return;
	}

	// read
	if (std::regex_search(str, read)) {
		HandleRead( str, false );
		triger = true;
		return;
	}

	if (!triger) {
		fout << str << std::endl;
	}
}

void Translator::PrepareFile() {
	fout << "#include <iostream>" << std::endl;
	fout << std::endl;
	fout << "using namespace std;" << std::endl;
	fout << std::endl;
}

void Translator::LoadErrors() {
	std::ifstream in_data("errors.data");

	std::regex tpl("(.+):(.+)");

	while ( !in_data.eof() ) {
		std::smatch match;
		std::string tmp;
		std::getline(in_data, tmp);

		if (std::regex_match(tmp, match, tpl)) {
			std::string str_pos = match[1].str();
			std::string str_txt = match[2].str();

			int pos = std::stoi( str_pos );

			error_data[pos] = str_txt;
		}
	}
}

void Translator::AddError( int line, int type ) {
	std::stringstream ss;

	ss << "Error: " << error_data[type] << " on line: " << line;

	errors.push_back( ss.str() );
}

void Translator::PrintErrors() {
	std::cout << "Program include " << errors.size() << " errors" << std::endl;

	if (errors.size() == 0) {
		std::cout << "Translate done! ..." << std::endl;
	}

	for ( int i = 0; i < errors.size(); i++ ) {
		std::cout << errors[i] << std::endl;
	}
}

void Translator::Initialize( std::string input_file, std::string output_file ) {
	fin.open( input_file );
	fout.open( output_file );

	LoadErrors();
}

void Translator::Run() {

	PrepareFile();

	while ( !fin.eof() ) {
		std::string tmp;
		std::getline( fin, tmp );
		
		line++;
		
		ParseString( tmp );
	}

	PrintErrors();

	fin.close();
	fout.close();
}