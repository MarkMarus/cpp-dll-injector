#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <libloaderapi.h>

using namespace std;
// Получение айди процесса в который мы впихиваем длл через айди процесса ( в моем случае - cs:go )
// Getting the id of process we want to inject dll in ( in my case it's cs:go )
void get_proc_id(const char* window_title, DWORD& process_id)
{
	GetWindowThreadProcessId(FindWindow(NULL, window_title), &process_id);
}

// меню ошибки
void error(const char* error_title, const char* error_message)
{
	MessageBox(0, error_message, error_title, 0);
	exit(-1);
}
// булевая функция которая проверяет файл на наличие
// check if the file exists at all
bool file_exists(string file_name)
{
	struct stat buffer;
	return (stat(file_name.c_str(), &buffer) == 0);
}


int main()
{
	// переменные (длл нейм - длл файл, виндоу тайтл - процесс инжекта)
	// variables (dll name - dll file, windows title - injection process)
	DWORD proc_id = NULL;
	char dll_path[MAX_PATH];
	string dllInput;
	cout << "Enter name of the DLL (path to dll if the dll isn't in this folder): ";
	cin >> dllInput;
	const char* dll_name = dllInput.c_str();
	const char* window_title = "Безымянный - Paint";


	// я заебался писать комменты - дальше сами разбирайтесь :)
	if (!file_exists(dll_name))
	{
		error("file_exist", "File doesn't exist");
	}

	if (!GetFullPathName(dll_name, MAX_PATH, dll_path, nullptr))
	{
		error("GetFullPathName", "Failed to get full path");
	}

	get_proc_id(window_title, proc_id);
	if (proc_id == NULL)
	{
		error("get_proc_id", "failed to get process id");
	}

	HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, proc_id);
	if (!h_process)
	{
		error("OpenProccess", "Failed to open proccess");
	}

	void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!allocated_memory)
	{
		error("VirtualAllocEx", "Failed to allocate memory");
	}

	if (!WriteProcessMemory(h_process, allocated_memory, dll_path, MAX_PATH, nullptr))
	{
		error("WriteProcessMemory", "failed to write proccess memory");
	}

	HANDLE h_thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
	if (!h_thread)
	{
		error("CreateRemoteThread", "Failed to create remote thread");
	}

	CloseHandle(h_process);
	VirtualFreeEx(h_process, allocated_memory, 0, MEM_RELEASE);
	MessageBox(0, "Success", "Successfully injected DLL to game!", 0);
}