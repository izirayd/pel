#pragma once

#include "string_function.hpp"
#include "platform.hpp"

#include <stdio.h>
#include <string>

#ifdef PLATFORM_LINUX
#define MAX_PATH 256
#endif

#define FORMAT_TEXT 0
#define FORMAT_BIN  1

enum class table_encoding_t
{
	ORI_UTF8 = 0,
	ORI_UTF16LE,
	ORI_UNICODE,
	ORI_UNKNOW
};

enum class file_status_t
{
	READ    = 0,    // Открывает для чтения. Если файл не существует или его невозможно найти, вызов fopen завершается ошибкой.
	WRITE   = 1,    // Открывает пустой файл для записи. Если указанный файл существует, его содержимое удаляется.
	NWRITE  = 2,    // Открывается для записи в конце файла (добавление) без удаления маркера в конце файла (EOF) перед записью новых данных в файл. Создает файл, если он не существует.
	NREWR   = 3,    // Открывает для чтения и записи. Файл должен существовать.
	CLREWR  = 4,    // Открывает пустой файл для чтения и записи.Если файл существует, его содержимое удаляется.
	NCRREWR = 5     // Открывается для чтения и добавления. Операция добавления включает удаления маркера EOF перед записью новых данных в файл. Маркер EOF не восстанавливается по окончании записи. Создает файл, если он не существует.
};

#define OR  ||
#define AND &&

enum
{
	ORI_SUCCESS = 1             // Удачно
};

class file_t
{

public:
	file_t() {
		FileDescriptor = NULL;
	}

	uint32_t GetSizeFile() {
		fseek(GetDesc(), 0, SEEK_END);
		Size =  ftell(GetDesc());
		fseek(GetDesc(), 0, SEEK_SET);
		return Size;
	}

	bool FullWriteFile(char *Buffer, uint32_t WriteSize, uint32_t SizeBuffer)
	{
		bool Result = true;
		fseek(GetDesc(), 0, SEEK_SET);
		fwrite(Buffer, WriteSize, SizeBuffer, GetDesc());
		return Result;
	}

	bool FullReadFile(char *Buffer, uint32_t ReadSize)
	{
		bool Result =  false;
		uint32_t SizeFile = GetSizeFile();
		fseek(GetDesc(), 0, SEEK_SET);

		Result = fread(Buffer, ReadSize, SizeFile, GetDesc());
		
		return Result;
	}

	bool OpenFile(const char *NameFile, file_status_t StatusOpen = file_status_t::READ) {

		bool Result = false;

		if (NameFile == NULL)     return Result = "NameFile == NULL in OpenFile file.hpp";
		if (NameFile[0] == 0x00)  return Result = "NameFile is emty in OpenFile file.hpp";

		wchar_t WName[256];
		cast_char(WName, NameFile, strlen(NameFile));
	
		wfilename = WName;
		cfilename = NameFile;

		if (StatusOpen == file_status_t::READ)
			if ((FileDescriptor = fopen(NameFile, "rb")) == NULL) return Result = false;

		if (StatusOpen == file_status_t::WRITE)
			if ((FileDescriptor = fopen(NameFile, "wb")) == NULL) return Result = false;

		if (StatusOpen == file_status_t::NWRITE)
			if ((FileDescriptor = fopen(NameFile, "ab")) == NULL) return Result = false;

		if (StatusOpen == file_status_t::NREWR)
			if ((FileDescriptor = fopen(NameFile, "rb+")) == NULL) return Result = false;

		if (StatusOpen == file_status_t::CLREWR)
			if ((FileDescriptor = fopen(NameFile, "wb+")) == NULL) return Result = false;

		if (StatusOpen == file_status_t::NCRREWR)
			if ((FileDescriptor = fopen(NameFile, "ab+")) == NULL) return Result = false;

		Size = GetSizeFile();

		return Result;
	}

	bool OpenFile(const wchar_t *NameFile, file_status_t StatusOpen = file_status_t::READ, table_encoding_t TypeEncoding = table_encoding_t::ORI_UNKNOW, int32_t TypeFormat = FORMAT_BIN) {
		
		bool Result = false;

		if (NameFile == NULL)     return Result = false;
		if (NameFile[0] == 0x00)  return Result = false;


		char CName[256] = { 0 };
		cast_char(CName, NameFile, wcslen(NameFile));

		cfilename = CName;
		wfilename = NameFile;

#ifdef PLATFORM_LINUX

		char AsciiNameFile[256] = { 0 };
		cast_char(AsciiNameFile, NameFile, wcslen(NameFile));
		return OpenFile(AsciiNameFile, StatusOpen);

#endif

#ifdef PLATFORM_WINDOWS

		wchar_t FlagFile[32] = { 0 };

		if (TypeFormat == FORMAT_BIN)
		{

			if (StatusOpen == file_status_t::READ)
				cpy(FlagFile, L"rb");

			if (StatusOpen == file_status_t::WRITE)
				cpy(FlagFile, L"wb");

			if (StatusOpen == file_status_t::NWRITE)
				cpy(FlagFile, L"ab");

			if (StatusOpen == file_status_t::NREWR)
				cpy(FlagFile, L"rb+");

			if (StatusOpen == file_status_t::CLREWR)
				cpy(FlagFile, L"wb+");

			if (StatusOpen == file_status_t::NCRREWR)
				cpy(FlagFile, L"ab+");

		}

		if (TypeFormat == FORMAT_TEXT)
		{
			if (StatusOpen == file_status_t::READ)
				cpy(FlagFile, L"rt");

			if (StatusOpen == file_status_t::WRITE)
				cpy(FlagFile, L"wt");

			if (StatusOpen == file_status_t::NWRITE)
				cpy(FlagFile, L"at");

			if (StatusOpen == file_status_t::NREWR)
				cpy(FlagFile, L"rt+");

			if (StatusOpen == file_status_t::CLREWR)
				cpy(FlagFile, L"wt+");

			if (StatusOpen == file_status_t::NCRREWR)
				cpy(FlagFile, L"at+");

		}

		if (TypeEncoding == table_encoding_t::ORI_UNICODE)
			cat(FlagFile, L",ccs=UNICODE");

		if (TypeEncoding == table_encoding_t::ORI_UTF8)
			cat(FlagFile, L",ccs=UTF-8");


		if (TypeEncoding == table_encoding_t::ORI_UTF16LE)
			cat(FlagFile, L",UTF-16LE");

		if ((FileDescriptor = _wfopen(NameFile, FlagFile)) == NULL)
			  return Result = "No open in OpenFile file.hpp";;

#endif

		Size = GetSizeFile();
		return Result;
	}

	inline operator bool() { 
		if (GetDesc() != NULL) return true; 
		 else return false;
	}

	void CloseFile() {
		if (FileDescriptor != NULL)
			fclose(FileDescriptor);
	}

	inline FILE *GetDesc() { return FileDescriptor; }

	inline void StartPostion() {
		fseek(GetDesc(), 0L, SEEK_END);
	}

	std::wstring wfilename;
	std::string  cfilename;

	uint32_t Size = 0;

private:
	FILE *FileDescriptor;

};