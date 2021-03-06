// 数独.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include<ctime>
using namespace std;
class Sudoku {
public:
	Sudoku(int N);//构造函数,
	//void setanswer(int i,int j);
	//void output();
	void letterrandom();//随机生成某个字母该填哪个数
	void setchangemodel();//生成行列变化的种子
	void rowchange(int row1,int row2);//行变换实现
	void colchange(int col1,int col2);//列变换实现
	void model2answer();//生成终局
	void giveanswer();//多次调用model2answer生成要求数量的终局
	void output();//将所有结果输出到文件
	void answer2output();//将当前局结果写入输出数组里
	void outputready();//初始化输出数组
	//void readsudoku(string file);
	bool readSudoku(fstream &read_file);//读入求解的题目
	void solvesudoku(string file_name);
	bool ifSolveS(int i, int j);//解答题目
	
private:
	int number_now;//当前已生成的终局个数
	int number_total;//总共要生成多少个终局
	int list[9][9];//当前局的数独局面
	char model[9][9]= { {'i','g','h','c','a','b','f','d','e'},{ 'c','a','b','f','d','e','i','g','h' },
	{'f','d','e','i','g','h','c','a','b'},{'g','h','i','a','b','c','d','e','f'},{'a','b','c','d','e','f','g','h','i'},
	{ 'd','e','f','g','h','i','a','b','c'},{'h','i','g','b','c','a','e','f','d'},{'b','c','a','e','f','d','h','i','g'},
	{ 'e','f','d','h','i','g','b','c','a' }
	};//导入字母模板
	int changemodel[9][9];//行列变换的种子
	//int lastanswer;//真正终局
	int random[8];//模板中字母对应的数字
	char *outputarray;//用于把结果一次性输出到文件里
	int outputarrayptr;//记录输出数组中有多少位已经填入
	bool num_row[10][10];
	bool num_col[10][10];
	bool num_box[10][10][10];
	//int* outputanswer;
};
int main(int argc, char* argv[])
{
	clock_t start_time = 0, end_time = 0, output_time = 0;
	start_time = clock();
	
	
	if (strcmp(argv[1], "-c")==0)
	{
		Sudoku mysudoku(atoi(argv[2]));
		if (atoi(argv[2]) >= 0 && atoi(argv[2]) <= 1000000) {
	        Sudoku mysudoku(1000);
			mysudoku.outputready();
			mysudoku.giveanswer();
			end_time = clock();
			mysudoku.output();
			output_time = clock();
			cout << (end_time - start_time)/CLOCKS_PER_SEC << "  " << (output_time - start_time)/CLOCKS_PER_SEC << endl;
	}
		else {
			cout << "input error " << endl;
		}
	}
	else
	{
		if (strcmp(argv[1], "-s")==0)
		{
			Sudoku mysudoku(1000000);
			mysudoku.outputready();
			mysudoku.solvesudoku("E:\\sudoku.txt");
			end_time = clock();
			mysudoku.output();
			cout << (end_time - start_time) / CLOCKS_PER_SEC << "  " << (output_time - start_time) / CLOCKS_PER_SEC << endl;
		}
		else
		{
			cout << "input error" << endl;
		}
	}
}

/* 构造函数，初始化对象的变量*/
Sudoku::Sudoku(int N) 
{
	number_now = 0;
	number_total = N;//记录所需生成的终局数量
	memset(this->list, 0, sizeof(this->list));//初始化当前局面
	memset(changemodel, 0, sizeof(changemodel));
	outputarrayptr = 0;
	/*for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << model[i][j] << " ";

		}
		cout << endl;
	}*/
}
/* 用模板生成主要包含两部分，第一步是为字母赋随机数值，共有8！=40320种
     第二步是在赋值好的模板上进行行列变换，由于题目限制左上角数固定，每种赋值有2!*3!*3!*2!*3!*3!=5184种变化 */
/*  letterrandom函数用于生成模板中字母对应的数字*/
void Sudoku::letterrandom() {
	int flag = 1;
	srand((int)time(NULL));//以时间做种子，确保生成数不重复
	for (int i = 0; i < 8; i++) {
		random[i] = rand() % 9 + 1;//将除了7以外的数字赋予不同的字母
		for (int j = 0; j < i; j++) {
			if (random[i] == random[j] || random[i] == 7) {//初始位置确定是7
				i--;//若有重复，则当前值重新赋值
				flag = 0;
				break;
			}
		}
		if (0 == flag) { flag = 1; continue; }
		
	}
}
/*setchangemodel() 用以为字母赋随机数值  */
void Sudoku::setchangemodel() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if(model[i][j]!='i')
			changemodel[i][j] = random[model[i][j] - 'a'];
			if (model[i][j]== 'i') {
				changemodel[i][j] = 7;
			}
			//cout << changemodel[i][j] << " ";
		}
		//cout << endl;
	}
}
/*  rowchange()和colchange用于完成最终结果的调整，就是将数字7调回最左上角的格子里*/
void Sudoku::rowchange(int row1,int row2) {
	int changemid = 0;
	for (int i = 0; i < 9; i++) {
		changemid= list[row1][i];
		list[row1][i] = list[row2][i];
		list[row2][i] = changemid;
	}
}
void Sudoku::colchange(int col1, int col2) {
	int changemid = 0;
	for (int i = 0; i < 9; i++) {
		changemid = list[i][col1];
		list[i][col1] = list[i][col2];
		list[i][col2] = changemid;
	}
}
/*model2answer用于将赋值好的模板进行行列交换生成最终结果*/
void Sudoku::model2answer() {
	//int now = number_now,total=number_total;
	int colmodel[9][9];//用于储存进行了列变换而未进行行变换的中间结果
	int rand_row_col[6][3];//存储行列变换的信号，也即如何变换
	memset(colmodel, 0, sizeof(colmodel));//初始化数组
	memset(rand_row_col, 0, sizeof(rand_row_col));
	srand(number_now);//以当前时间作为种子，确保每次生成的序列不同
	/*  下面这一个循环体用于生成行列变换的信号 */
	for(int i=0;i<6;i++){
		for (int j = 0; j < 3; j++) {
			rand_row_col[i][j] =rand()%3;//因为只能在三行或三列中进行交换，故信号量要模3
			for (int k = 0; k < j; k++) {
				if (rand_row_col[i][j] == rand_row_col[i][k]) {
					j--;
					break;
				}
			}
		}
	}
	/* 行列变换仅能在三格内变换，如果只通过行列交换的方式过于繁琐，因而选择新建数组，再直接写入的方式  */

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			colmodel[i][j] = changemodel[i][rand_row_col[j / 3][j % 3]+j-j%3];
		}
	}
	/*for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << colmodel[i][j] << " ";
		}
		cout << endl;
	}*/
	/*在行变换的基础上进行列变换*/
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			list[i][j] = colmodel[rand_row_col[i/3+3][i%3]+i-i%3][j];
		}
	}
	/*for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << list[i][j] << " ";
		}
		cout << endl;
	}*/
	//cout << endl;
	/* 由于题目要求，左上角必须为7，但在行列变换时可能已经破坏了这个条件
	为了满足该条件，通过合法的行列变换将最左上九宫格里的7变换到最左上角的格子即可 */
	int flag = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (list[i][j] == 7) {
				colchange(0, j);
				rowchange(0, i);
				flag = 1;
				break;
			}
		}
		if (1 == flag) {
			flag = 0;
			break;
		}
	}

	/*for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << list[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;*/
}

/* 最终通过传入参数来生成要求数量的终局 */
void Sudoku::giveanswer() {
	/* 这里由于是随机生成的，而测试数据共1e6，所以为了保证终局尽量不同，每组字母赋值仅选用了2000个变换（共5184个）
	只需进行500次字母赋值即可*/
	for (int i = 0; i < number_total / 2000+1; i++) {
		int flag = 0;
		letterrandom();

		setchangemodel();//每两千个终局换一个数字模板
		for (int j = 0; j < 2000; j++) {
			if (1 == flag) {
				flag = 0;
				break;
			}
			if (number_now >= number_total) {
				flag = 1;
				break;
			}
			model2answer();
			number_now++;
			answer2output();
		}
	}
}
/*outputready用于初始化输出数组*/
void Sudoku::outputready() {
	int total = this->number_total;
	cout << total << endl;
	this->outputarray = new char[163*total+5];
	memset(outputarray, 'b', 163*total+5);
	/*for (int i = 0; i < 163*total; i++) {
		cout << outputarray[i] << " ";
	}*/
}
/*answer2output将生成的终局或解答写入输出数组里，最后一起输出。选择一起输出的原因是这样做只访问一次外存，速度更快*/
void Sudoku::answer2output() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			this->outputarray[outputarrayptr] = '0'+list[i][j];
			/* 鉴于挨个数组输出太浪费时间，所以选择一起输出。但换行符和空格并不好处理，所以选择把数字全部转换为字符
			这样就可以将空格和数字写入同一个数组里，输出就方便了*/
			
			outputarrayptr++;
			if (j != 8)//读到一行的最后一格，则不加空格，换成换行符
			{
				outputarray[outputarrayptr] = ' ';
				outputarrayptr++;
			}
			else {
				outputarray[outputarrayptr] = '\n';
				outputarrayptr++;
			}
		}
		//outputarray[outputarrayptr++] = '\n';
	}
	outputarray[outputarrayptr] = '\n';//一个答案输出完了，换行符隔开。对最后一个答案在之后单独处理。
	outputarrayptr++;
}
/* output()将输出数组里的内容输出到文本中*/
void Sudoku::output() {
	fstream file("E:\\sudoku.txt", ios::out);
	outputarray[outputarrayptr - 1] = '\0';
	outputarray[outputarrayptr - 2] = '\0';//数组后两个符号都是换行符，把它们都删去，就满足题目要求了
	file << this->outputarray;
	/* 下面的for循环用来进行控制台输出，测试方便*/
	for (int i = 0; i < outputarrayptr; i++) {
		cout << outputarray[i];
	}
	//cout << outputarrayptr << endl;
	delete[] this->outputarray;//释放内存
	this->outputarray = NULL;
	file.close();
}
/* 将文本中的题目读入到解答数组里*/
bool Sudoku::readSudoku(fstream &read_file)
{
	string str_line;
	int num = 0;
	memset(num_row, true, sizeof(num_row));
	memset(num_col, true, sizeof(num_col));
	memset(num_box, true, sizeof(num_box));
	/*初始化可填入数组，这三个数组分别判定某个数可否在行、列、九宫格里填入*/
	for (int i = 0; i < 9; i++)
	{
		if (!getline(read_file, str_line))
			return false;
		for (int k = 0, j = 0; k <= 16; k += 2, j++)//每一行里字符间隔了一个空格
		{
			num = str_line[k] - '0';//将字符转化成数字填入
			this->list[i][j] = num;
			this->num_row[num][i] = false;
			this->num_col[num][j] = false;
			this->num_box[num][i][j] = false;
			/* 该位置已经给出答案，故修改该数字的可填入数组*/
		}
	}
	getline(read_file, str_line);
	return true;
}

void Sudoku::solvesudoku(string file_name)
{
	fstream read_file(file_name, ios::in);
	if (!read_file.is_open())
	{
		cout << "Error: 打开文件错误！" << endl;
		exit(-1);
	}

	while (readSudoku(read_file))
	{
		if (ifSolveS(1, 1))
		{
			answer2output();//递归函数调用成功，表明有正确解，将解写入输出数组里
		}
		else
			cout << "无解！" << endl;
	}

	read_file.close();
}
/* ifsolves()用来解答题目*/
bool Sudoku::ifSolveS(int i, int j)
{
	if (i == 9 && j == 10)//数组填好了，有结
		return true;

	if (i != 9 && j == 10)//某一行填完了，开始填下一行
	{
		i++;
		j = 1;
	}

	if (this->list[i][j] != 0)//判定当前格子是否需要填入
	{
		return ifSolveS(i, j + 1);
	}

	int row = ((i - 1) / 3) * 3 + 1;
	int col = ((j - 1) / 3) * 3 + 1;

	for (int num = 0; num < 9; num++)
	{
		if (this->num_row[num][i] == true
			&& this->num_col[num][j] == true
			&& this->num_box[num][row][col] == true)
			/*该数字同时满足行、列、九宫格条件，填入*/
		{
			this->list[i][j] = num;
			this->num_row[num][i] = false;
			this->num_col[num][j] = false;
			this->num_box[num][row][col] = false;
			/* 数字填入之后修改可填入数组*/
			bool out_come = ifSolveS(i, j + 1);//填下一个格子

			if (out_come)
				return true;
			/*  填入失败，回溯*/
			this->num_row[num][i] = true;
			this->num_col[num][j] = true;
			this->num_box[num][row][col] = true;
		}
	}

	this->list[i][j] = 0;
	return false;
}