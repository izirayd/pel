﻿/*
 Dev console version 0.1;
*/

#include <pel.hpp>

#include <code\detail\file.hpp>
#include <code\detail\file_watcher/FileWatcher.h>
#include <code\detail\path.hpp>

#define BIG_DATA_FILE_TEST

#ifdef _WIN64
	#define script_file_name L"\\..\\..\\parser_data\\parser_text.txt"
#else
	#define script_file_name L"\\..\\parser_data\\parser_text.txt"
#endif // _WIN64

#ifdef _WIN64
	#define script_file_data L"\\..\\..\\parser_data\\data.txt"
#else
	#define script_file_data L"\\..\\parser_data\\data.txt"
#endif // _WIN64

using namespace std::chrono_literals;

unsigned int BinHashH37(const char* Data, int Size)
{
	unsigned int hash = 2139062143;
	for (int i = 0; i < Size; i++)
		hash = 37 * hash + Data[i];
	return hash;
}

uint32_t file_hash(const std::wstring& file_name)
{
	file_t cpp_file;

	cpp_file.OpenFile(file_name.c_str());
	uint32_t result = 0;

	if (cpp_file)
	{
		int size_file = cpp_file.GetSizeFile();

		char* buffer_code = new char[size_file + sizeof(void*)];

		cpp_file.FullReadFile(buffer_code, 1);
		buffer_code[size_file] = 0x00;

		result = BinHashH37(buffer_code, size_file);

		cpp_file.CloseFile();
		delete[] buffer_code;
	}
	else
	{
		fmt::print("Can`t open file {}\n", cpp_file.cfilename.c_str());
	}

	return result;
}

uint32_t last_hash = 0;

void read_data_file(std::string& data, const std::wstring &file_name)
{
	file_t file;

	data.clear();

	file.OpenFile(dir_t(file_name.c_str()));

	if (file)
	{
		if (file.GetSizeFile() > 0) {
			data.resize(file.GetSizeFile());
			file.FullReadFile(data.data(), 1);
		}
		else
		{
			fmt::print("Emtpy file {}\n", file.cfilename.c_str());
			return;
		}

		file.CloseFile();
	}
	else
	{
		fmt::print("Can`t open file {}\n", file.cfilename.c_str());
		return;
	}

	if (data.empty())
	{
		fmt::print("File empty {}\n", file.cfilename.c_str());
		return;
	}
}

void read_code(std::string& result_code)
{
	file_t cpp_file;
	path_t cpp_path;

#ifdef _WIN64
	cpp_file.OpenFile(dir_t(L"\\..\\..\\parser_data\\parser_text.txt"));
#else
	cpp_file.OpenFile(dir_t(L"\\..\\parser_data\\parser_text.txt"));
#endif // _WIN64

	if (cpp_file)
	{
		int size_file = cpp_file.GetSizeFile();

		char* buffer_code = new char[size_file + sizeof(void *)];

		cpp_file.FullReadFile(buffer_code, 1);
		buffer_code[size_file] = 0x00;

		result_code = buffer_code;

		cpp_file.CloseFile();
		delete[] buffer_code;
	}
	else
	{
		fmt::print("Can`t open file {}\n", cpp_file.cfilename.c_str());
		return;
	}

	if (result_code.empty())
	{
		fmt::print("File empty {}\n", cpp_file.cfilename.c_str());
		return;
	}
}

int process(const std::string& code, const std::string &data)
{
	pel::pel_parser_t pel_parser;

	if (!code.empty()) {

		pel_parser.code = code;
	
#ifdef BIG_DATA_FILE_TEST
		if (!data.empty())
			pel_parser.push_text(data);
#endif

		if (!pel_parser.compile()) {

		}

		pel_parser.run();
		pel_parser.get_ast();
	}
	else
	{
		fmt::print("No code, file is empty!\n");
	}

	pel_parser.clear();
	pel_parser.clear_pe();

	return 0;
}

void process_code()
{
	std::string code, data;

	read_data_file(code, script_file_name);
	read_data_file(data, script_file_data);

	process(code, data);
}

class code_file : public FW::FileWatchListener
{
public:
	code_file() {}

	void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename, FW::Action action)
	{
		if (filename == L"parser_text.txt")
		{
			if (FW::Action::Modified) {

				char CName[256] = { 0 };
				cast_char(CName, filename.c_str(), wcslen(filename.c_str()));

				std::string cfilename = CName;

				std::wstring full_name;

				full_name += dir;
				full_name += filename;

				uint32_t hash = file_hash(full_name);

				if (hash != 0 && hash != last_hash)
				{
					last_hash = hash;

					// TODO: only windows?!
					std::system("cls");

					process_code();
				}
			}
		}
	}
};

void main_thread()
{
	process_code();

	path_t cpp_path;

	FW::FileWatcher fw;

#ifdef _WIN64
	fw.addWatch(std::wstring(dir_t(L"\\..\\..\\parser_data\\")), new code_file());
#else
	fw.addWatch(std::wstring(dir_t(L"\\..\\parser_data\\")), new code_file());
#endif // _WIN64

	while (true)
	{
		fw.update();
		std::this_thread::sleep_for(100ms);
	}

	fmt::print("End thread\n");
}


void start(int count_th)
{
	for (size_t i = 0; i < count_th; i++)
	{
		std::thread th(main_thread);
		th.detach();
	}
}


class output_handle_t {
public:

	void init() {

#ifdef PLATFORM_WINDOWS
		// for support color from FMT
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hOut == INVALID_HANDLE_VALUE)
		{
			fmt::print("Error init STD_OUTPUT_HANDLE %u\n", GetLastError());
			return;
		}

		DWORD dwMode = 0;

		if (!GetConsoleMode(hOut, &dwMode))
		{
			fmt::print("Error init GetConsoleMode %u\n", GetLastError());
			return;
		}

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

		if (!SetConsoleMode(hOut, dwMode))
		{
			fmt::print("Error init SetConsoleMode %u\n", GetLastError());
			return;
		}

#endif // PLATFORM_WINDOWS
	}

};

int main()
{
	std::system("title PEL DEV: Parser Engine Lang Dev console");

	output_handle_t output_handle;
	output_handle.init();

	fmt::print(fmt::fg(fmt::color::coral), "Dev version with render code pel, with tree code v{0}.{1}\n\n",  PEL_VERSION_MAJOR, PEL_VERSION_MINOR);

	file_t file;
	bool result = false;

#ifdef _WIN64
	result = file.OpenFile(dir_t(L"\\..\\..\\parser_data\\parser_text.txt"));
#else
	result = file.OpenFile(dir_t(L"\\..\\parser_data\\parser_text.txt"));
#endif // _WIN64

	if (result) {
	
		file.CloseFile();

		fmt::print("PEL uses a system of instant response to file changes. You can change the file in real time and the PEL will reload it without restarting the pel application-interpreter.\n\n");
		
		fmt::print("File: {}\n\n\n", file.cfilename);

		start(1);
	}
	else
	{
		fmt::print("Fatal error, can`t open file with pel code {} [no file!]\n", file.cfilename);
	}

	// pause
	for (;;) { std::this_thread::sleep_for(100ms); }
}