/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Surgical Planning - Parent App

[Main class]
The program execution starts from the _tmain method.

For more details, please refer to the documentation.

Developed by Yasmin Halwani		(yasmin.halwani@outlook.com)
Supervised by Dr. Osama Halabi	(ohalabi@qu.edu.qa)
Computer Science and Engineering Department
Qatar University
2014
****************************************************************************/

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{

	// starts the first program (preoperative stage) ----------------- 

	STARTUPINFO StartInfo1; // name structure
	PROCESS_INFORMATION ProcInfo1; // name structure

	memset(&ProcInfo1, 0, sizeof(ProcInfo1)); // Set up memory block
	memset(&StartInfo1, 0 , sizeof(StartInfo1)); // Set up memory block
	StartInfo1.cb = sizeof(StartInfo1); // Set structure size
	StartInfo1.wShowWindow = SW_HIDE; // Hide window
	TCHAR tcsCommandLine[] = _T("first.exe");
	int res = CreateProcess(NULL, tcsCommandLine, NULL, NULL, 
		NULL, NULL, NULL, NULL, &StartInfo1, &ProcInfo1); // starts MyApp

	// Wait until child process exits.
    DWORD value1 = WaitForSingleObject( ProcInfo1.hProcess, INFINITE );
	// Close process and thread handles. 
	CloseHandle( ProcInfo1.hProcess );
	CloseHandle( ProcInfo1.hThread );

	// -----------------------------------------------------------------

	// if the preoperative stage software exited properly, start the next
	// stage (operation stage) -----------------------------------------
	if(value1 == WAIT_OBJECT_0)
	{
		STARTUPINFO StartInfo2; // name structure
		PROCESS_INFORMATION ProcInfo2; // name structure

		memset(&ProcInfo2, 0, sizeof(ProcInfo2)); // Set up memory block
		memset(&StartInfo2, 0 , sizeof(StartInfo2)); // Set up memory block
		StartInfo2.cb = sizeof(StartInfo2); // Set structure size
		StartInfo2.wShowWindow = SW_HIDE; // Hide window

		TCHAR tcsCommandLine[] = _T("second.exe");
		int res = CreateProcess(NULL, tcsCommandLine, NULL, NULL, 
			NULL, NULL, NULL, NULL, &StartInfo2, &ProcInfo2); // starts MyApp

		// Wait until child process exits.
		DWORD value2 = WaitForSingleObject( ProcInfo2.hProcess, INFINITE );

		// Close process and thread handles. 
		CloseHandle( ProcInfo2.hProcess );
		CloseHandle( ProcInfo2.hThread );
	}

	// -----------------------------------------------------------------

	while(1); // keeps the application running till closed by the user

	return 0;
}

