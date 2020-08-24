#define OPTIM
//#define PRINTF_RETURN
//#define ONE_TEST
//#define EXECUTE
#define NO_PRINT
//#define NO_REG
#define CHANGE_REG
#define _CRT_SECURE_NO_WARNINGS
//#define ALL_TEST
#ifdef ALL_TEST
#ifdef _WIN32
#include <io.h>
#else
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#endif

#include  <time.h> 
#include <iostream>
#include <vector>
#include <string>
#include<algorithm>
#include "frontEndInput.hpp"
#include "globalOptimizer.hpp"
#include "arm.hpp"

typedef unsigned int uint;

using namespace std;

//四大任务：
//1.生成中间代码
//1.生成.s汇编代码
//2.生成可执行文件代码
//3.执行程序进行结果比对

void getFiles(string path, vector<string>& files);
vector<string> getAllCaseNameOfCatalog(void);
void generateAllTacCodeFile(void);
void generateOneTacCodeFile(const string& fileName);					
void generateAllAssemblyFile(void);								//生成汇编代码
void generateOneAssemblyFile(const string& fileName);

#ifndef _WIN32
void generateAllExecutableFile(void);							//生成可执行文件
void generateOneExecutableFile(const string& fileName);

void checkAllResultOfExecutableFile(void);						//执行可执行文件并进行结果比对
void checkOneResultOfExecutableFile(const string& fileName);
#endif


int main(int argc, char* argv[])
{
	// generateOneTacCodeFile("test.sy");
	// generateOneAssemblyFile("test.sy");
	// generateOneExecutableFile("test.s");
	// checkOneResultOfExecutableFile("test");
	#ifndef EXECUTE
	generateAllTacCodeFile();
	generateAllAssemblyFile();
	generateAllExecutableFile();
	#endif
	checkAllResultOfExecutableFile();
	return 0;
}

/****** 垃圾代码 *******/
void generateAllTacCodeFile(void)
{
	vector<string> fileNeedToTestList = getAllCaseNameOfCatalog();
	//删除上一次汇编代码文件
	for(int i = 0;i < fileNeedToTestList.size();i++)
	{
		string fileNeedToSave = fileNeedToTestList[i] + ".txt";
		if(access(fileNeedToSave.c_str(),0))continue;
		remove(fileNeedToSave.c_str());
	}
	//生成汇编文件
	for(int i = 0;i < fileNeedToTestList.size();i++)
	generateOneTacCodeFile(fileNeedToTestList[i] + ".sy");	
}

void generateAllAssemblyFile(void)
{
	vector<string> fileNeedToTestList = getAllCaseNameOfCatalog();
	//删除上一次汇编代码文件
	for(int i = 0;i < fileNeedToTestList.size();i++)
	{
		string fileNeedToSave = fileNeedToTestList[i] + ".s";
		if(access(fileNeedToSave.c_str(),0))continue;
		remove(fileNeedToSave.c_str());
	}
	//生成汇编文件
	for(int i = 0;i < fileNeedToTestList.size();i++)
	generateOneAssemblyFile(fileNeedToTestList[i] + ".sy");	
}

#ifndef _WIN32
void generateAllExecutableFile(void)
{
	vector<string> fileNeedToTestList = getAllCaseNameOfCatalog();
	//删除上一次可执行文件
	for(int i = 0;i < fileNeedToTestList.size();i++)
	{
		string fileNeedToSave = fileNeedToTestList[i];
		if(access(fileNeedToSave.c_str(),0))continue;
		remove(fileNeedToSave.c_str());
	}
	//生成可执行代码文件
	for(int i = 0;i < fileNeedToTestList.size();i++)
	generateOneExecutableFile(fileNeedToTestList[i]+".s");		
}

void checkAllResultOfExecutableFile(void)
{
	vector<string> fileNeedToTestList = getAllCaseNameOfCatalog();
	//删除所有输出结果
	for(int i = 0;i < fileNeedToTestList.size();i++)
	{
		string fileNeedToSave = fileNeedToTestList[i];
		fileNeedToSave.append(".result");
		if(access(fileNeedToSave.c_str(),0))continue;
		remove(fileNeedToSave.c_str());
	}
	//检查所有结果
	for(int i = 0;i < fileNeedToTestList.size();i++)
	{
		if(access(fileNeedToTestList[i].c_str(),0))
		{
			cout << "the " << fileNeedToTestList[i] << "is not exist!" << endl;
			continue;
		}
		checkOneResultOfExecutableFile(fileNeedToTestList[i]);
	}
			
}
#endif

/****** 重要代码 *******/
void generateOneTacCodeFile(const string& fileName)
{
	string fileNeedToTest = fileName;
	string fileNeedToSave = fileName;
	fileNeedToSave.erase(fileName.size()-3,3);
	fileNeedToSave.append(".txt");	

	//.txt中间代码生成
	freopen(fileNeedToTest.c_str(), "r", stdin);
    freopen(fileNeedToSave.c_str(), "w", stdout);
	line = 1;
	lex_char = 0;
	token[0] = 0;
	tos = 0;
	symb_list = NULL;
	enode_list = NULL;
	global_symbol_table = NULL, local_symbol_table = NULL;
	field_head = NULL, field_tail = NULL;
	next_temp = 0, next_label = 0;
	local_scope = 0;
	tac_head = NULL;
	stack_head = NULL, stack_tail = NULL;
	temp_count = 0;
	label_count = 0;
	count_for_rename = 0;
	count_for_string = 0;
	lex.val = 0;
	lex.str = NULL;
	band.buf = 0;
	band.flu = 0;
	lex_and_parser();

	//中间代码格式整理
	make_tac_format();
	//制作一个空头
	tac* emptyHead = new tac();
	tac* tacHead = get_tac_head();
	emptyHead->next = tacHead;
	tacHead->prev = emptyHead;
	cout << endl <<  "传进来的中间代码：" << endl;
	print_tac();
    printf("\n\n@@@@@@@@@@@@@@@@@@@@@@\n\n");

    GlobalOptimizer* globalOptimizer = new GlobalOptimizer();
    globalOptimizer->writeCodeIntoFunctionBlock(emptyHead);
	globalOptimizer->makeSsaTranslate();
	globalOptimizer->runOptimizer();
	cout << endl <<  "生成的代码：" << endl;
	tac_print(globalOptimizer->transSsaTacIntoTac()->next);
	delete globalOptimizer;
	fflush(stdout);

#ifdef _WIN32
	freopen("CON","w",stdout);
	freopen("CON","r",stdin);
#else
	freopen("/dev/tty", "r", stdin);
	freopen("/dev/tty", "w", stdout);
#endif
	cout << "succeed to generate " << fileNeedToSave << endl;
	fflush(stdout);
}

void generateOneAssemblyFile(const string& fileName)
{
	string fileNeedToTest = fileName;
	string fileNeedToSave = fileName;
	fileNeedToSave.erase(fileName.size()-3,3);
	fileNeedToSave.append(".s");
	
	//.s汇编文件代码生成
	freopen(fileNeedToTest.c_str(), "r", stdin);
    freopen(fileNeedToSave.c_str(), "w", stdout);
	line = 1;
	lex_char = 0;
	token[0] = 0;
	tos = 0;
	symb_list = NULL;
	enode_list = NULL;
	global_symbol_table = NULL, local_symbol_table = NULL;
	field_head = NULL, field_tail = NULL;
	next_temp = 0, next_label = 0;
	local_scope = 0;
	tac_head = NULL;
	stack_head = NULL, stack_tail = NULL;
	temp_count = 0;
	label_count = 0;
	count_for_rename = 0;
	count_for_string = 0;
	lex.val = 0;
	lex.str = NULL;
	band.buf = 0;
	band.flu = 0;
	lex_and_parser();

	//中间代码格式整理
	make_tac_format();
	//制作一个空头
	tac* emptyHead = new tac();
	tac* tacHead = get_tac_head();
	emptyHead->next = tacHead;
	tacHead->prev = emptyHead;
	
	// print_tac();
    // printf("\n\n@@@@@@@@@@@@@@@@@@@@@@\n\n");
    GlobalOptimizer* globalOptimizer = new GlobalOptimizer();
    globalOptimizer->writeCodeIntoFunctionBlock(emptyHead);
	globalOptimizer->makeSsaTranslate();
	globalOptimizer->runOptimizer();
	cg(globalOptimizer->transSsaTacIntoTac());
	//cg(emptyHead);
	delete globalOptimizer;

#ifdef _WIN32
	freopen("CON","w",stdout);
	freopen("CON","r",stdin);
#else
	freopen("/dev/tty", "r", stdin);
	freopen("/dev/tty", "w", stdout);
#endif
	cout << "succeed to generate " << fileNeedToSave << endl;
}

#ifndef _WIN32
void generateOneExecutableFile(const string& fileName)
{
	//生成可执行文件 gcc test.s sylib.h libsysy.a -o test -march=armv7-a -static
	string fileNeedToTest = fileName;
	string fileNeedToSave = fileName;
	fileNeedToSave.erase(fileName.size()-2,2);
	
	//生成可执行指令
	string commandToExc = "gcc-7 -g " + fileNeedToSave + ".s" + " sylib.h "
	+ "libsysy.a -o " + fileNeedToSave + " -march=armv7-a -static";
	int result = system(commandToExc.c_str());
	if(result == 0)
	{
		cout << "succeed to generate " << fileNeedToSave << endl;
		return;
	}
	cout << "fail to generate " << fileNeedToSave << endl;
	//exit(-1);
}

void checkOneResultOfExecutableFile(const string& fileName)
{
	//命令：./test < input.txt > output.txt
	//执行可执行文件并检查答案是否正确
	string fileNeedToTest = fileName;
	
	string commandToExc = "./" + fileNeedToTest;
	//检查是否有输入文件
	if(!access((fileNeedToTest + ".in").c_str(),0))
		commandToExc += " < " + fileNeedToTest + ".in";
	//将打印结果和返回值写入result文档
	commandToExc += " > " + fileNeedToTest + ".result";
	//commandToExc += ";echo $? >> " + fileNeedToTest + ".result";
	int result = system(commandToExc.c_str());
	// if(result != 0)
	// {
	// 	cout << fileNeedToTest + " is run error!" << endl;
	// 	exit(-1);
	// }
	//进行结果比对
	string diffCommand = "diff -B " + fileNeedToTest + ".result "
		+ fileNeedToTest + ".out";// + " > report.txt";
	result = system(diffCommand.c_str());
	if(result != 0)
	{
		cout << "the result of " + fileNeedToTest + " is wrong!" << endl;
		//exit(-1);
	}
	else 
	{
		cout << "the result of " + fileNeedToTest + " is true!" << endl;
	}
}
#endif



vector<string> getAllCaseNameOfCatalog(void)
{
	//首先获取文件名列表
#ifdef ONE_TEST
	string prefPath("one_test");//性能测试
	vector<string> pFilesPath;
	vector<string> filesPath;
	getFiles(prefPath,pFilesPath);
	for(uint i = 0;i < pFilesPath.size();i++)
		filesPath.push_back(pFilesPath[i]);	
#else	
	string funcPath("functional_test");//功能测试
	string prefPath("performance_test");//性能测试
	vector<string> fFilesPath;
	vector<string> pFilesPath;
	vector<string> filesPath;
	getFiles(funcPath,fFilesPath);
	getFiles(prefPath,pFilesPath);
	for(uint i = 0;i < fFilesPath.size();i++)
		filesPath.push_back(fFilesPath[i]);
	for(uint i = 0;i < pFilesPath.size();i++)
		filesPath.push_back(pFilesPath[i]);

#endif

	vector<string> fileNameOfCase;
	fileNameOfCase.clear();
	for(uint i = 0;i < filesPath.size();i++)
	{
		uint filePathLen = filesPath[i].size();
		const char* cFilesPath = filesPath[i].c_str();
		if(filePathLen < 3)continue;
		if(cFilesPath[filePathLen-1] != 'y')continue;
		if(cFilesPath[filePathLen-2] != 's')continue;
		if(cFilesPath[filePathLen-3] != '.')continue;
		filesPath[i].erase(filesPath[i].size()-3,3);
		fileNameOfCase.push_back(filesPath[i]);
	}
	return fileNameOfCase;
}

void getFiles( string path, vector<string>& files)
{
	#ifdef _WIN32
    long   hFile   =   0;
    struct _finddata_t fileinfo;
    string p;
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
    {
        do
        {
            if(!(fileinfo.attrib &  _A_SUBDIR))
                files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
        }while(_findnext(hFile, &fileinfo)  == 0);
        _findclose(hFile);
    }
	//sort(files.begin(),files.end());
	#else
	DIR *dir;
	struct dirent *ptr;

	if ((dir = opendir(path.c_str())) == NULL)
	{
		perror("Open dir error...");
		exit(1);
	}
	while ((ptr = readdir(dir)) != NULL)
	{
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
		continue;
		else if (ptr->d_type == 8)
		files.push_back(path + "//" + ptr->d_name);
		else if (ptr->d_type == 10)
		continue;
	}
	closedir(dir);
	sort(files.begin(),files.end());
	#endif
}






#else

#ifdef _WIN32
#include <io.h>
#else
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#endif

#include  <time.h> 
#include <iostream>
#include <vector>
#include <string>
#include<algorithm>
#include "frontEndInput.hpp"
#include "globalOptimizer.hpp"
#include "arm.hpp"

typedef unsigned int uint;

using namespace std;

//四大任务：
//1.生成中间代码
//1.生成.s汇编代码
//2.生成可执行文件代码
//3.执行程序进行结果比对

int main(int argc, char* argv[])
{//./main -o com.s -S ./00_arr_defn2.sy 
//./main -S -o com.s ./00_arr_defn2.sy 
	if (argc != 5 && argc != 6)
	{
		printf("error args\n");
		exit(0);
	}
	if (freopen(argv[4], "r", stdin) == NULL)
	{
		printf("error: open %s failed\n", argv[4]);
		exit(0);
	}
	if (freopen(argv[3], "w", stdout) == NULL)
	{
		printf("error: open %s failed\n", argv[3]);
		exit(0);
	}
	line = 1;
	lex_char = 0;
	token[0] = 0;
	tos = 0;
	symb_list = NULL;
	enode_list = NULL;
	global_symbol_table = NULL, local_symbol_table = NULL;
	field_head = NULL, field_tail = NULL;
	next_temp = 0, next_label = 0;
	local_scope = 0;
	tac_head = NULL;
	stack_head = NULL, stack_tail = NULL;
	temp_count = 0;
	label_count = 0;
	count_for_rename = 0;
	count_for_string = 0;
	lex.val = 0;
	lex.str = NULL;
	band.buf = 0;
	band.flu = 0;

	lex_and_parser();

	//中间代码格式整理

	make_tac_format();
	//封装全局变量
	// ReplaceGlobalByLocal replaceGlobal(tac_head);
	// replaceGlobal.makeGlobalToLocal();
	// make_tac_format();
	//制作一个空头
	tac* emptyHead = new tac();
	tac* tacHead = get_tac_head();
	emptyHead->next = tacHead;
	tacHead->prev = emptyHead;
	if (strcmp("-tac", argv[2]) == 0)
	{
		print_tac();
	}
	GlobalOptimizer* globalOptimizer = new GlobalOptimizer();
	globalOptimizer->writeCodeIntoFunctionBlock(emptyHead);
	globalOptimizer->makeSsaTranslate();
	globalOptimizer->runOptimizer();
	TAC* ptr_tac=globalOptimizer->transSsaTacIntoTac();

	//cg(emptyHead);
	cg(ptr_tac);
		#ifndef NO_PRINT
		cout<<"hello"<<endl;
		tac_print(ptr_tac->next);
		#endif
	delete globalOptimizer;
	return 0;
}

#endif

