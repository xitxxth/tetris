#include "tetris.h"
#include <stdlib.h>
#include <stdio.h>
//s	
static struct sigaction act, oact;
int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank();	break;
		case MENU_RECOMMEND: recommendedPlay();	break;
		case MENU_EXIT: exit=1; break;
		default: break;

	}
	}
	writeRankFile();
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0; // field reset

	nextBlock[0]=rand()%7; // 0 1 2 3 4 5 6, curr block
	nextBlock[1]=rand()%7; // 0 1 2 3 4 5 6, next block
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1; //realize at the roof
	blockX=WIDTH/2-2; //realize at the center of the roof
	recommend(NULL);//fixed
	score=0;//score reset
	gameOver=0;//game0ver==1 -> over
	timed_out=0;//used at blockdown()

	//Draw
	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);//instead drwaBlock
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);//moved for new block_2

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);//moved for new block_2
}

int GetCommand(){
	int command; // week1
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1; //output, program quit == QUIT or 'q', cf) tetris.h
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX))) //drawflag==1 -> can
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag){
		DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);//DrawChange: erase, DBWF: draw shadow + blocks
	}
	return ret;	
}

int QuitCommand(int command){
	int ret=1; //output, program quit == QUIT or 'q', cf) tetris.h
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	default:
		break;
	}
	return ret;	
}

void DrawField(){//0=='.', 1=='tile', 2=='shadow'
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);//moved for new block_2
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(10+i,WIDTH+13);//moved for new block_2
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){//new block exists
				attron(A_REVERSE);
				printw(" ");//colored
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
	
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////
//block[nexblock[0]]
int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(block[currentBlock][blockRotate][i][j]==1){ //check if block exists(value==1)
				if(i+blockY>=HEIGHT || j+blockX>=WIDTH || j+blockX<0)//does it break the rule?
				return 0;
				if(f[i+blockY][j+blockX]==1)//the target place is already filled with block
				return 0;
			}
		}
	}
	return 1;//can move
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int prevY = blockY, prevX = blockX, prevRot = blockRotate; //they represent the previous block's position
	int i, j;
	switch(command){
	case KEY_UP://arrow_up
	prevRot = (blockRotate+3)%4;
		break;

	case KEY_DOWN://arrow_down
		prevY = blockY-1;//previous block's position
		break;

	case KEY_RIGHT://arrow_right
		prevX = blockX-1;//previous block's position
		break;

	case KEY_LEFT://arrow_left
		prevX = blockX+1;//previous block's position
		break;

	default:
		break;
	}
	int pshadY= prevY, pshadX = prevX, pshadRot = prevRot; //previous shadow block's position
	//it has same value as prev pos, but we declare new variance for convenience
	while(CheckToMove(field, currentBlock, pshadRot, pshadY, pshadX)){
		pshadY++;
	}
	pshadY--;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
		if(block[currentBlock][pshadRot][i][j]==1 && (pshadY+i)<HEIGHT && (pshadX+j)<WIDTH){//does it break the rule? 
            move(i+pshadY+1, j+pshadX+2);//move the cursor
            printw("\b.");//erase prev shadow, fill it with dot
		}	
		}
	}
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
		if(block[currentBlock][prevRot][i][j]==1 && (prevY+i)<HEIGHT && (prevX+j)<WIDTH){//does it break the rule? 
            move(i+prevY+1, j+prevX+2);//move the cursor
            printw("\b.");//erase prev block, fill it with dot
		}
		}
	}
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
	return;
}

void BlockDown(int sig){//if get sig
	// user code
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)){//can drop it?
        DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, ++blockY, blockX);//drop it
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);//DB -> DBWF
	}
	else{
		int i;
		//gameover
		if(blockY==-1){ //!(check to move) && blockY==-1 -> game0ver 
			gameOver=1;
		}
		//add block
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX); //turn block to field
		//check deleting line
		score += DeleteLine(field);//count score
		//print score
		PrintScore(score);//print score
		//bring new block
		nextBlock[0] = nextBlock[1];//bring next block
		//make new block
		nextBlock[1] = nextBlock[2];//bring next block
        nextBlock[2] = rand()%7;//make new block_2
		blockY = -1; 
		blockX = (WIDTH/2)-2;
		blockRotate = 0;//make new block
		recommend(NULL);
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
		DrawNextBlock(nextBlock);//draw nextblock[1], [2]
		//initialize current block location(drop end), Drawfield()
		DrawField();//draw field
	}
	timed_out=0;//for next call(alarm call)
return;//강의자료 p26-27의 플로우차트를 참고한다.
}

void RecBlockDown(int sig){//if get sig
	// user code
/*
	while(blockRotate!=recommendR){
		blockRotate++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);//drop it
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);//DB -> DBWF
		usleep(INTERVAL);
	}
	if(blockX>recommendX){
		while(blockX!=recommendX){
		blockX--;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);//drop it
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);//DB -> DBWF
		usleep(INTERVAL);
		}
	}
	else if(blockX<recommendX){
		while(blockX!=recommendX){
		blockX++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);//drop it
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);//DB -> DBWF
		usleep(INTERVAL);
		}
	}
	while(blockY!=recommendY){
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);//drop it
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);//DB -> DBWF
		usleep(INTERVAL);
	}
*/
blockX=recommendX;
blockY=recommendY;
blockRotate=recommendR;
	if(!CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)){//can drop it?
		int i;
		//gameover
		if(blockY==-1){ //!(check to move) && blockY==-1 -> game0ver 
			gameOver=1;
		}
		//add block
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX); //turn block to field
		//check deleting line
		score += DeleteLine(field);//count score
		//print score
		PrintScore(score);//print score
		//bring new block
		nextBlock[0] = nextBlock[1];//bring next block
		//make new block
		nextBlock[1] = nextBlock[2];//bring next block
        nextBlock[2] = rand()%7;//make new block_2
		blockY = -1; 
		blockX = (WIDTH/2)-2;
		blockRotate = 0;//make new block
		recommend(NULL);
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
		DrawNextBlock(nextBlock);//draw nextblock[1], [2]
		//initialize current block location(drop end), Drawfield()
		DrawField();//draw field
	}
	timed_out=0;//for next call(alarm call)
return;//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j, touched = 0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++){
			if(block[currentBlock][blockRotate][i][j]==1){//turn block
                f[blockY+i][blockX+j] = 1;//into field
                if(i + blockY + 1 == HEIGHT || f[blockY+i+1][blockX+j]==1) {touched++;}//Y_pos ==21 : the bottom of field
				//if the block turns into field on the bottom, score += 10
            }
		}
	//Block이 추가된 영역의 필드값을 바꾼다.
	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int i, j, x, y, count = 0;
	for(i=0; i<HEIGHT; i++){
		for(j=0; j<WIDTH; j++){
			if(f[i][j]==0)	break;
		}
		if(j==WIDTH){
			count++;
			for(y=i; y>0; y--){
				for(x=0; x<WIDTH; x++){
					f[y][x] = f[y-1][x];
				}
			}
		}
	}
	for(j=0; j<WIDTH; j++)	f[0][j] = 0;

	
	return count*count*100;//as a pre_condition
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int pshadY = y, pshadX = x;//shadow pos
	while(CheckToMove(field, nextBlock[0], blockRotate, pshadY, pshadX)){
		pshadY++;
	}
	pshadY--;//drop the shadow
	DrawBlock(pshadY, x, blockID, blockRotate, ' ');
	DrawBlock(pshadY, x, blockID, blockRotate, '/');//draw the shadow
	return;
	}

void createRankList(){
	// user code
	int i;
	FILE* fp = fopen("rank.txt", "r");
	if(!fp){
		printw("FILE OPEN ERROR\n");
		return;
	}
	Node_pointer prev = NULL;
	Node_pointer curr = NULL;
	Node_pointer newnode = NULL;
	fscanf(fp, "%d\n", &size_rank);
	while(1){
		newnode = (Node_pointer)malloc(sizeof(struct Node));
		newnode->link = NULL;
		if(fscanf(fp, "%s %d\n", newnode->name, &newnode->point)==EOF){
			free(newnode);
			break;
		}
		if(!HEAD){
			HEAD = newnode;
			prev = HEAD;
		}
		prev->link = newnode;
		prev = newnode;
	}
	if(fclose(fp)!=0){
		printw("FILE CLOSE ERROR\n");
	}
	return;
}

void rank(){
	clear();
	// user code
	Node_pointer curr = HEAD;
	Node_pointer prev = HEAD;
	int x=1, y=size_rank, i, ch;
	printw("input the type you want to see ranks\n");
	printw("1. ranks from X to Y\n");
	printw("2. by a specific name\n");
	printw("3. delete a specific rank\n");
	ch = wgetch(stdscr);
	echo();

	if(ch == '1'){
		printw("X ");
		scanw("%d", &x);
		printw("Y ");
		scanw("%d", &y);
		if(y>size_rank)	y=size_rank;
		printw("\n%10s | %10s\n", "name", "score");
		printw("-----------------------\n");
		if(x>y)	printw("serach failure: no rank in the list\n");
		else if(x<=y){
			for(i=0; i<x-1; i++)	curr = curr->link;
			for(i=x; i<y; i++){
				printw("%10s | %10d\n", curr->name, curr->point);
				curr = curr->link;
			}
			printw("%10s | %10d\n", curr->name, curr->point);
		}
		}

		else if(ch == '2'){
		int check=0;
		char tar_name[40];
		printw("input the name: ");
		scanw("%s", tar_name);
		printw("\n%10s | %10s\n", "name", "score");
		printw("-----------------------\n");
		while(curr){
			if(strcmp(tar_name, curr->name)==0){
				printw("%10s | %10d\n", tar_name, curr->point);
				check=1;
			}
			curr = curr->link;
		}
		if(check==0)	printw("search failure: no name in the list\n");
	}

		else if(ch == '3'){
			if(size_rank<=0){
				printw("EMPTY RANK");
				return;
			}
			int rank, i;
			printw("input the rank: ");
			scanw("%d", &rank);
			if(rank<=size_rank && rank>1){
				for(i=1; i<rank; i++){
					prev=curr;
					curr=curr->link;
				}
				prev->link = curr->link;
				free(curr);
				size_rank--;
				printw("Rank delete complete\n");
			}
			else if(rank==1){
				prev = curr;
				curr = curr->link;
				HEAD = curr;
				free(prev);
				size_rank--;
				printw("Rank delete complete\n");
			}
			else{ printw("search failure: the rank not in the list\n");	}
		}
	noecho();
    getch();
}
	


void writeRankFile(){
	int i;
	Node_pointer curr = HEAD;
	Node_pointer prev = HEAD;
	FILE* fp = fopen("rank.txt", "w");
	if(!fp){
		printw("FILE OPEN ERROR\n");
		return;
	}
	fprintf(fp, "%d\n", size_rank);
	while(curr){
		prev = curr;
		fprintf(fp, "%s %d\n", curr->name, curr->point);
		curr = curr->link;
		if(prev!=NULL)	free(prev);
	}
	if(fclose(fp)!=0){
		printw("FILE CLOSE ERROR\n");
	}
	return;
	// user code
}

void newRank(int score){
	// user code
	echo();
	Node_pointer newnode = NULL;
	newnode = (Node_pointer)malloc(sizeof(struct Node));
	newnode->point = score;
	newnode->link = NULL;
	clear();
	printw("your name: ");
	scanw("%s", newnode->name);
	if(!HEAD)	HEAD = newnode;
	Node_pointer curr = HEAD;
	Node_pointer prev = NULL;
	//x+1p   x   x-1 c
	while((newnode->point < curr->point) && curr){
		prev = curr;
		curr = curr->link;
	}
	if(HEAD->point < newnode->point){
		newnode -> link = HEAD;
		HEAD = newnode;
	}
	else{
	if(prev)	prev->link = newnode;
	if(curr!=newnode)	newnode -> link = curr;
	}
	size_rank++;
	noecho();
	return;
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	if(CheckToMove(field, blockID, recommendR, recommendY, recommendX)==1){
	DrawBlock(recommendY, recommendX, blockID, recommendR, ' ');
	DrawBlock(recommendY, recommendX, blockID, recommendR, 'R');
	}
	return;
}

int  recommend(Leaf_pointer prev){
int max=0, tmp;
int acc_score;
int rotate, x, y, i, j;
char originField[HEIGHT][WIDTH];
Leaf_pointer curr = (Leaf_pointer)malloc(sizeof(Leaf));
if(prev==NULL){
curr->level = 0;
acc_score = 0;
for(i=0; i<HEIGHT; i++)
	for(j=0; j<WIDTH; j++)
		originField[i][j] = field[i][j];
}
else{
curr->level = prev->level + 1;
acc_score = prev->accumulatedScore;
for(i=0; i<HEIGHT; i++)
	for(j=0; j<WIDTH; j++)
		originField[i][j] = prev->recField[i][j];
}
curr->curBlockID = nextBlock[curr->level];

	if(curr->curBlockID==4)	rotate=3;
	else if(curr->curBlockID==0 || curr->curBlockID==5 || curr->curBlockID==6)	rotate=2;
	else	rotate=0;

for(rotate=0; rotate<4; rotate++){
	for(x=-1; x<WIDTH; x++){

for(i=0; i<HEIGHT; i++)
	for(j=0; j<WIDTH; j++)
		curr->recField[i][j] = originField[i][j];
		y=0;
		while(CheckToMove(originField, curr->curBlockID, rotate, ++y, x)==1);	y--;
		if(CheckToMove(originField, curr->curBlockID, rotate, y, x)==0)	continue;
		curr->accumulatedScore = acc_score + AddBlockToField(curr->recField, curr->curBlockID, rotate, y, x);
		curr->accumulatedScore += DeleteLine(curr->recField);
		if(curr->level<VISIBLE_BLOCKS-1)	curr->accumulatedScore+=recommend(curr);
		if(max < curr->accumulatedScore){//problem
			max = curr->accumulatedScore;
			if(curr->level==0){
				recommendY = y;
				recommendX = x;
				recommendR = rotate;
			}
		}
		
	}
}	
	free(curr);
	return max;
}
//lv 0 complete
//1. 전역변수 2. leaf point -> leaf
void recommendedPlay(){
		int command;
	clear();
	act.sa_handler = RecBlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(QuitCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);

}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(y, x, blockID, blockRotate, ' ');//draw the block
	DrawShadow(y, x, blockID, blockRotate);//draw the shadow
	DrawRecommend(y, x, blockID, blockRotate);
	return;
}