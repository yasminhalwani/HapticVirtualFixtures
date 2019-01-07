/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Final Experiment - Parent App

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

const int V				= 1;
const int FV			= 2;
const int GV			= 3;
const int FG			= 4;
const int COM			= 5;
const int TUTORIAL		= 99;

void executeModule(int moduleNum)
{
	
	STARTUPINFO StartInfo; 
	PROCESS_INFORMATION ProcInfo; 

	memset(&ProcInfo, 0, sizeof(ProcInfo)); 
	memset(&StartInfo, 0 , sizeof(StartInfo)); 
	StartInfo.cb = sizeof(StartInfo); 
	StartInfo.wShowWindow = SW_HIDE; 
	DWORD value;

	switch(moduleNum)
	{
	case V: 
		{
			TCHAR tcsCommandLine[] = _T("V.exe");
			int res = CreateProcess(NULL, tcsCommandLine, NULL, NULL, 
			NULL, NULL, NULL, NULL, &StartInfo, &ProcInfo);
			value = WaitForSingleObject( ProcInfo.hProcess, INFINITE );
			CloseHandle( ProcInfo.hProcess );
			CloseHandle( ProcInfo.hThread );
		}
	break;

	case FV:
		{
			TCHAR tcsCommandLine[] = _T("FV.exe");
			int res = CreateProcess(NULL, tcsCommandLine, NULL, NULL, 
			NULL, NULL, NULL, NULL, &StartInfo, &ProcInfo);
			value = WaitForSingleObject( ProcInfo.hProcess, INFINITE );
			CloseHandle( ProcInfo.hProcess );
			CloseHandle( ProcInfo.hThread );
		}
		break;

	case GV:
		{
			TCHAR tcsCommandLine[] = _T("GV.exe");
			int res = CreateProcess(NULL, tcsCommandLine, NULL, NULL, 
			NULL, NULL, NULL, NULL, &StartInfo, &ProcInfo);
			value = WaitForSingleObject( ProcInfo.hProcess, INFINITE );
			CloseHandle( ProcInfo.hProcess );
			CloseHandle( ProcInfo.hThread );
		}
		break;

	case FG:
		{
			TCHAR tcsCommandLine[] = _T("FG.exe");
			int res = CreateProcess(NULL, tcsCommandLine, NULL, NULL, 
			NULL, NULL, NULL, NULL, &StartInfo, &ProcInfo);
			value = WaitForSingleObject( ProcInfo.hProcess, INFINITE ); 
			CloseHandle( ProcInfo.hProcess );
			CloseHandle( ProcInfo.hThread );
		}
		break;

	case COM:
		{
			TCHAR tcsCommandLine[] = _T("COM.exe");
			int res = CreateProcess(NULL, tcsCommandLine, NULL, NULL, 
			NULL, NULL, NULL, NULL, &StartInfo, &ProcInfo);
			value = WaitForSingleObject( ProcInfo.hProcess, INFINITE );
			CloseHandle( ProcInfo.hProcess );
			CloseHandle( ProcInfo.hThread );
		}
		break;

	case TUTORIAL: 
		{
			TCHAR tcsCommandLine[] = _T("tutorial.exe");
			int res = CreateProcess(NULL, tcsCommandLine, NULL, NULL, 
			NULL, NULL, NULL, NULL, &StartInfo, &ProcInfo);
			value = WaitForSingleObject( ProcInfo.hProcess, INFINITE );
			CloseHandle( ProcInfo.hProcess );
			CloseHandle( ProcInfo.hThread );
		}
		break;
	}
}

void runTutorialModule()
{
	executeModule(TUTORIAL);
}

void runTestingModule()
{
	int num1, num2, num3, num4, num5;
	num1 = num2 = num3 = num4 = num5 = 0;
	int moduleNum = 0;

	while(num1==0 || num2==0 || num3==0 || num4==0 || num5==0)
	{			
		srand ( time(NULL) );
		moduleNum = 1 + (rand() % 5);

		if(num1==0 && num2==0 && num3==0 && num4==0 && num5==0)
			num1 = moduleNum;
		else if(num2==0 && (moduleNum!=num1))
			num2 = moduleNum;
		else if(num3==0 && (moduleNum!=num1 && moduleNum!=num2))
			num3 = moduleNum;
		else if(num4==0 && (moduleNum!=num1 && moduleNum!=num2 && moduleNum!=num3))
			num4 = moduleNum;
		else if(num5==0 && (moduleNum!=num1 && moduleNum!=num2 && moduleNum!=num3 && moduleNum!=num4))
			num5 = moduleNum;
	}

	system("cls");
	printf("======================\n");
	printf("Module # 1\n");
	printf("======================\n");
	executeModule(num1);
	system("cls");
	printf("======================\n");
	printf("Module # 2\n");
	printf("======================\n");
	executeModule(num2);
	system("cls");
	printf("======================\n");
	printf("Module # 3\n");
	printf("======================\n");
	executeModule(num3);
	system("cls");
	printf("======================\n");
	printf("Module # 4\n");
	printf("======================\n");
	executeModule(num4);
	system("cls");
	printf("======================\n");
	printf("Module # 5\n");
	printf("======================\n");
	executeModule(num5);
	system("cls");
	printf("======================\n");
	printf("Module # 6\n");
	printf("======================\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("Robotic Surgery Haptic Simulation Experiment\n");
	printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

	char userResponse;
	cout<<"Would you like to run the tutorial module? [y/n]"<<endl;
	cin>>userResponse;
	cout<<endl;

	if(userResponse=='y')
		runTutorialModule();
	system("cls");
	cout<<"Loading.."<<endl;
	runTestingModule();	

	system("cls");
	cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout<<"Tester's Data"<<endl;
	cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;

	char gender;
	int age;
	char prevExperienceWithHaptics;

	cout<<"Do you have a previous experience with haptics? [y/n]: ";
	cin>>prevExperienceWithHaptics;
	cout<<"Your age: ";
	cin>>age;
	cout<<"Your gender [m/f]: ";
	cin>>gender;


	ofstream myfile;
	myfile.open ("OUTPUT_TESTER_DATA.txt");
	myfile << prevExperienceWithHaptics <<endl;
	myfile << age <<endl;
	myfile << gender <<endl;
	myfile.close();

	cout<<endl<<endl;
	cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout<<"Thank you for your valued contribution. Have a nice day!"<<endl;
	cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;

	while(true);

	return 0;
}



