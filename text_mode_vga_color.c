/*
 * text_mode_vga_color.c
 * Minimal driver for text mode VGA support
 * This is for Week 2, with color support
 *
 *  Created on: Oct 25, 2021
 *      Author: zuofu
 */

#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alt_types.h>
#include "text_mode_vga_color.h"
#include "palette_test.h"
#include "usb_kb/GenericMacros.h"
#include "usb_kb/GenericTypeDefs.h"
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <unistd.h>


extern BYTE keyboard_input();

int grid[29][38];
int rowCount[30];
int score;
int timer;
int highScore;


void textVGAColorClr()
{
	for (int i = 0; i<(ROWS*COLUMNS) * 2; i++)
	{
		vga_ctrl->VRAM[i] = 0x00;
	}
}

void textVGADrawColorText(char* str, int x, int y, alt_u8 background, alt_u8 foreground)
{
	int i = 0;
	while (str[i]!=0)
	{
		vga_ctrl->VRAM[(y*COLUMNS + x + i) * 2] = foreground << 4 | background;
		vga_ctrl->VRAM[(y*COLUMNS + x + i) * 2 + 1] = str[i];
		i++;
	}
}

void setColorPalette (alt_u8 color, alt_u8 red, alt_u8 green, alt_u8 blue)
{
	//fill in this function to set the color palette starting at offset 0x0000 2000 (from base)
	int colorRow = color/2;
	int colorCol = color%2;
	if(colorCol != 0){
		vga_ctrl -> COLOR[colorRow] &= ~(0xFFF << 13);
		vga_ctrl -> COLOR[colorRow] |= (blue << 13 | green << 17 | red << 21);
	}
	else{
		vga_ctrl -> COLOR[colorRow] &= ~(0xFFF << 1);
		vga_ctrl -> COLOR[colorRow] |= (blue << 1 | green << 5 | red << 9);
	}
}


void textVGAColorScreenSaver()
{
	//This is the function you call for your week 2 demo
	char color_string[80];
    int fg, bg, x, y;
	textVGAColorClr();
	//initialize palette
	for (int i = 0; i < 16; i++)
	{
		setColorPalette (i, colors[i].red, colors[i].green, colors[i].blue);
	}
	while (1)
	{
		fg = rand() % 16;
		bg = rand() % 16;
		while (fg == bg)
		{
			fg = rand() % 16;
			bg = rand() % 16;
		}
		sprintf(color_string, "Drawing %s text with %s background", colors[fg].name, colors[bg].name);
		x = rand() % (80-strlen(color_string));
		y = rand() % 30;
		textVGADrawColorText (color_string, x, y, bg, fg);
		usleep (100000);
	}
}

void create_background(){
	int col = 0;
	int row = 0;
	for(int i = 0; i < 2400; i++){
		col = i % 80;
		row = i / 80;
		if(col == 20){
//			vga_ctrl -> VRAM[i * 2] = 0x0F2F; //top right
			vga_ctrl->VRAM[i * 2] = 0xDF;
			vga_ctrl->VRAM[i * 2 + 1] = ']';
			printf("top right");
		}
		else if(col == 19){
//			vga_ctrl -> VRAM[i * 2] = 0x012F; //top right
			vga_ctrl->VRAM[i * 2] = 0xDF;
			vga_ctrl->VRAM[i * 2 + 1] = '[';
			printf("top right");
		}
		else if(col == 59){
//			vga_ctrl -> VRAM[i * 2] = 0x007F; //top right
			vga_ctrl->VRAM[i * 2] = 0xDF;
			vga_ctrl->VRAM[i * 2 + 1] = '[';
			printf("top right");
		}
		else if(col == 60){
//			vga_ctrl -> VRAM[i * 2] = 0x017F; //top right
			vga_ctrl->VRAM[i * 2] = 0xDF;
			vga_ctrl->VRAM[i * 2 + 1] = ']';
			printf("top right");
		}
		else if(row == 0 && (col > 20 && col < 59) && col % 2 == 0){
//			vga_ctrl -> VRAM[i * 2] = 0x007F; //top right
			vga_ctrl->VRAM[i * 2] = 0xDF;
			vga_ctrl->VRAM[i * 2 + 1] = ']';
			printf("top right");
		}
		else if(row == 0 && (col > 20 && col < 59) && col % 2 == 1){
//			vga_ctrl -> VRAM[i * 2] = 0x007F; //top right
			vga_ctrl->VRAM[i * 2] = 0xDF;
			vga_ctrl->VRAM[i * 2 + 1] = '[';
			printf("top right");
		}
		else if (col > 20 && col < 59 && row > 0 && row < 30){
			if(col % 2 == 0){
				vga_ctrl->VRAM[i * 2] = 0x10;
				vga_ctrl->VRAM[i * 2 + 1] = ']';
			}
			if(col % 2 == 1){
				vga_ctrl->VRAM[i * 2] = 0x10;
				vga_ctrl->VRAM[i * 2 + 1] = '[';
			}
		}
		else{
			vga_ctrl->VRAM[i * 2] = 0x00;
		}
//		count1++;
//		count2++;
//		count3++;
//		count4++;
	}

}

void dropSquare() {
	BYTE keycode;
    int count = 0;
    int randomStart = rand()%36+21;
    if(randomStart % 2 == 0){
    	if(randomStart == 56){
    		randomStart--;
    	}else{
    		randomStart++;
    	}
    }
    int time = timer;
    int randomEnd = randomStart + 4;
//    int oldStart = randomStart;
//    int oldEnd = randomEnd;
    checkEndGame(randomStart, randomEnd, 2);
    for (int i = 2; i < 30; i++) {
        if (count == 0) {
            //vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0x27F;
//            vga_ctrl->VRAM[(i * 80 + 6) * 2] = 0xFF;
//            vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0xFF;

        	for (int rowToAdd = 0; rowToAdd < 2; rowToAdd++) {
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[((i - rowToAdd) * 80 + j) * 2] = 0xDE;
					vga_ctrl->VRAM[((i - rowToAdd) * 80 + j) * 2 + 1] = check_col(j);
				}
        	}


        } else {
        	for(int j = randomStart; j < randomEnd; j++){
        		if(grid[i][j] == 1){
        			for(int k = randomStart; k < randomEnd; k++){
        				grid[i-1][k] = 1;
        				grid[i-2][k] = 1;
        				rowCount[i-1]++;
        				rowCount[i-2]++;
        			}
        			return;
        		}
        	}
//        	printf("%d", keycode);
//            vga_ctrl->VRAM[(currentIndex) * 2] = 0x10;
//            vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0x27F;
//        	vga_ctrl->VRAM[(oldIndex1) * 2] = 0x00;
//        	vga_ctrl->VRAM[(oldIndex2) * 2] = 0x00;
//
//            vga_ctrl->VRAM[(i * 80 + 6) * 2] = 0xFF;
//            vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0xFF;

//        	for (int j = oldStart; j < oldEnd; j++) {
////        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x00;
//            		vga_ctrl->VRAM[((i - 2) * 80 + j) * 2] = 0x10;
//            		vga_ctrl->VRAM[((i - 2) * 80 + j) * 2 + 1] = check_col(j);
//
//        	}
        	i--;
        	for (int j = randomStart; j < randomEnd; j++) {
//        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x00;
					vga_ctrl->VRAM[((i-1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i-1) * 80 + j) * 2 + 1] = check_col(j);

			}
        	for (int j = randomStart; j < randomEnd; j++) {
//        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x00;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);

			}
        	i++;

//        	for (int rowToAdd = 0; rowToAdd < 2; rowToAdd++) {
//				for (int j = oldStart; j < oldEnd; j++) {
//					vga_ctrl->VRAM[((i + rowToAdd) * 80 + j) * 2] = 0x10;
//					vga_ctrl->VRAM[((i + rowToAdd) * 80 + j) * 2 + 1] = check_col(j);
//				}
//			}
        	keycode = keyboard_input();
//        	keycode = 0x00;
        	time = timer;
        	if(keycode == 0x51){
        		time = timer / 1.5;
			}
			else if(keycode == 0x4F){
//				i++;
				if(randomEnd != 59 && grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1){
					randomStart += 2;
					randomEnd += 2;
					i--;
				}
			}
			else if(keycode == 0x50){
//				i++;
				if(randomStart != 21 && grid[i - 1][randomStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1){
					randomStart -= 2;
					randomEnd -= 2;
					i--;
				}
			}else if(keycode == 0x2C){
				time = 0;
			}
//        	oldStart = randomStart;
//        	oldEnd = randomEnd;

        	for (int rowToAdd = 0; rowToAdd < 2; rowToAdd++) {
				for (int j =randomStart; j < randomEnd; j++) {
//					vga_ctrl->VRAM[((i + rowToAdd) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - rowToAdd) * 80 + j) * 2] = 0xDE;
					vga_ctrl->VRAM[((i - rowToAdd) * 80 + j) * 2 + 1] = check_col(j);
				}
        	}

        }
        count++;
        usleep (time);
    }
    for(int i = randomStart; i < randomEnd; i++){
    	grid[28][i] = 1;
    	grid[29][i] = 1;
    	rowCount[29]++;
    	rowCount[28]++;
    }

}

void dropRectangle() {
    int count = 0;
    BYTE keycode;
    int randomStart = rand()%30+21;
    int time = timer;
    if(randomStart % 2 == 0){
    	if(randomStart == 50){
    		randomStart--;
    	}else{
    		randomStart++;
    	}
    }

    int randomEnd = randomStart + 8;
    checkEndGame(randomStart, randomEnd, 1);
//    int startTwo, endTwo, startThree, endThree, startFour, endFour;
    int shapeLayout = 0; //0 = sideways, 1 = vertical
    for (int i = 1; i < 30; i++) {
        if (count == 0) {
            //vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0x27F;
//            vga_ctrl->VRAM[(i * 80 + 6) * 2] = 0xFF;
//            vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0xFF;


			for (int j = randomStart; j < randomEnd; j++) {
				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xB3;
				vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);

			}



        } else {

        	if(shapeLayout == 0){
				for(int j = randomStart; j < randomEnd; j++){
					if(grid[i][j] == 1){
						for(int k = randomStart; k < randomEnd; k++){
							grid[i-1][k] = 1;
							rowCount[i-1]++;
						}
						return;
					}
				}
			}else if(shapeLayout == 1){
				for(int j = randomStart; j < randomEnd; j++){
					if(grid[i][j] == 1){
						for(int k = randomStart; k < randomEnd; k++){
							grid[i-1][k] = 1;
							grid[i-2][k] = 1;
							grid[i-3][k] = 1;
							grid[i-4][k] = 1;

							rowCount[i-1]++;
							rowCount[i-2]++;
							rowCount[i-3]++;
							rowCount[i-4]++;
						}
						return;
					}
				}
			}

        	if(shapeLayout){
				for(int row = 0; row < 4; row++){
					for (int j = randomStart; j < randomEnd; j++) {
		//				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row - 1) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row - 1) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}else{
				for (int j = randomStart; j < randomEnd; j++) {
	//				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
			}

//        	for (int j = randomStart; j < randomEnd; j++) {
////        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x00;
//            		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
//            		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
//        	}

        	keycode = keyboard_input();
        	time = timer;
			if(keycode == 0x51){
				time = timer / 1.5;
			}
			else if(keycode == 0x4F){
//				if(shapeLayout == 1){
//					if(grid[i-1][randomEnd + 1] != 1  && grid[i-2][randomEnd + 1] != 1
//							&& grid[i-3][randomEnd + 1] != 1   && grid[i-4][randomEnd + 1] != 1   ){
//						if(randomEnd != 59){
//							randomStart += 2;
//							randomEnd += 2;
//							i--;
//						}
//					}
//				}
//				else
				if(randomEnd != 59){
//					randomStart += 2;
//					randomEnd += 2;
//					i--;
					if(shapeLayout == 1){
						if(grid[i-1][randomEnd + 1] != 1  && grid[i-2][randomEnd + 1] != 1
								&& grid[i-3][randomEnd + 1] != 1   && grid[i-4][randomEnd + 1] != 1   ){
							if(randomEnd != 59){
								randomStart += 2;
								randomEnd += 2;
								i--;
							}
						}
					}else if(grid[i-1][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						i--;
					}
				}
			}else if(keycode == 0x50){
				if(randomStart != 21){
//					randomStart -= 2;
//					randomEnd -= 2;
//					i--;
					if(shapeLayout == 1){
						if(grid[i-1][randomStart - 1] != 1  && grid[i-2][randomStart - 1] != 1
								&& grid[i-3][randomStart - 1] != 1   && grid[i-4][randomStart - 1] != 1   ){
							if(randomEnd != 59){
								randomStart -= 2;
								randomEnd -= 2;
								i--;
							}
						}
					}else if(grid[i-1][randomStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						i--;
					}
				}
			}else if(keycode == 0x1D && i > 3 && i < 29 ){ // rotate left
				i--;
				if(shapeLayout && randomEnd < 57 && randomStart > 23){
					shapeLayout = 0;
					randomStart -= 2;
					randomEnd += 4;
					i--;
				}else if(shapeLayout == 0){
					shapeLayout = 1;
					randomStart += 2;
					randomEnd -= 4;
					i++;
				}else{
					i++;
				}
			}else if(keycode == 0x1B && i > 2 && i < 29){ // rotate left
				i--;
				if(shapeLayout  && randomEnd < 57 && randomStart > 23){
					shapeLayout = 0;
					randomStart -= 2;
					randomEnd += 4;
					i--;
				}else if(shapeLayout == 0){
					shapeLayout = 1;
					randomStart += 2;
					randomEnd -= 4;
					i++;
				}else{
					i++;
				}
			}else if(keycode == 0x2C){
				time = 0;
			}


			if(shapeLayout){
				for(int row = 0; row < 4; row++){
					for (int j = randomStart; j < randomEnd; j++) {
		//				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0xB3;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}else{
				for (int j = randomStart; j < randomEnd; j++) {
	//				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xB3;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}

//			for (int j = randomStart; j < randomEnd; j++) {
////				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
//				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xB3;
//				vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
//			}


        }
        count++;
        usleep (time);
    }
    if(shapeLayout == 0){
    	for(int i = randomStart; i < randomEnd; i++){
    		grid[29][i] = 1;
    		rowCount[29]++;
    	}
    }else if(shapeLayout == 1){
    	for(int i = randomStart; i < randomEnd; i++){
    		grid[28][i] = 1;
    		grid[27][i] = 1;
    		grid[26][i] = 1;
    		grid[29][i] = 1;

    		rowCount[29]++;
    		rowCount[28]++;
    		rowCount[27]++;
    		rowCount[26]++;
    	}
    }

}

void dropJBlock() {
    int count = 0;
    BYTE keycode;
    int randomStart = rand()%32+21;
    if(randomStart % 2 == 0){
    	if(randomStart == 52){
    		randomStart--;
    	}else{
    		randomStart++;
    	}
    }
    int randomFirstEnd = randomStart + 2;
    int randomEnd = randomStart + 6;
    int time = timer;
    int secondWall = 0;
    int shapeLayout = 0; //0 == default, 1 == one left rotate, 2 == 2 rotate, 3 == 3 rotate
    checkEndGame(randomStart, randomEnd, 2);
    for (int i = 2; i < 30; i++) {
        if (count == 0) {
            //vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0x27F;
//            vga_ctrl->VRAM[(i * 80 + 6) * 2] = 0xFF;
//            vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0xFF;


        	for (int j = randomStart; j < randomFirstEnd; j++) {
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x56;
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
        	}

        	for (int j = randomStart; j < randomEnd; j++) {
				vga_ctrl->VRAM[(i * 80 + j) * 2] = 0x56;
				vga_ctrl->VRAM[(i * 80 + j) * 2 + 1] = check_col(j);
			}



        } else {
        	if(shapeLayout == 0){
				for (int j = randomStart; j < randomEnd; j++) {
					if(grid[i][j] == 1){
						for (int j = randomStart; j < randomFirstEnd; j++) {
							grid[i - 2][j] = 1;
							rowCount[i-2]++;
						}
						for (int j = randomStart; j < randomEnd; j++) {
							grid[i -1][j] = 1;
							rowCount[i-1]++;
						}
						return;
					}
				}
			}else if(shapeLayout == 1){
				for (int j = randomStart; j < randomEnd; j++){
					if(grid[i][j] == 1){
						for (int j = randomStart; j < randomEnd; j++) {
							grid[i- 1][j] = 1;
							rowCount[i-1]++;
						}
						for(int row = 1; row < 3; row++){
							for (int j = secondWall; j < randomEnd; j++) {
								grid[i - 1 - row][j] = 1;
								rowCount[i-1-row]++;
							}
						}
						return;
					}
			}
			}else if(shapeLayout == 2){
				for (int j = secondWall; j < randomEnd; j++){
					if(grid[i][j] == 1){
						for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 2][j] = 1;
							rowCount[i-2]++;
						}
						for (int j = secondWall; j < randomEnd; j++) {
							grid[i-1][j] = 1;
							rowCount[i-1]++;
						}
						return;
					}
				}
				for(int j = randomStart; j < secondWall; j++){
					if(grid[i - 1][j] == 1){
						for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 2][j] = 1;
							rowCount[i - 2]++;
						}
						for (int j = secondWall; j < randomEnd; j++) {
							grid[i-1][j] = 1;
							rowCount[i - 1]++;
						}
						return;
					}
				}

			}else if(shapeLayout == 3){
				for (int j = randomStart; j < randomFirstEnd; j++){
					if(grid[i][j] == 1){
						for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 3][j] = 1;
							rowCount[i - 3]++;
						}
						for(int row = 0; row < 2; row++){
							for (int j = randomStart; j < randomFirstEnd; j++) {
								grid[i - row - 1][j] = 1;
								rowCount[i - row - 1]++;
							}
						}
						return;
					}
				}
				for (int j = randomFirstEnd; j < randomEnd; j++){
					if(grid[i - 2][j] == 1){
						for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 3][j] = 1;
							rowCount[i - 3]++;
						}
						for(int row = 0; row < 2; row++){
							for (int j = randomStart; j < randomFirstEnd; j++) {
								grid[i - row - 1][j] = 1;
								rowCount[i - row - 1]++;
							}
						}


						return;
					}
				}

			}
//
        	i--;
        	if(shapeLayout == 0){
				for (int j = randomStart; j < randomFirstEnd; j++) {
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[(i * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[(i * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[(i * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[(i * 80 + j) * 2 + 1] = check_col(j);
				}
				for(int row = 1; row < 3; row++){
					for (int j = secondWall; j < randomEnd; j++) {
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}else if(shapeLayout == 2){
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
				for (int j = secondWall; j < randomEnd; j++) {
					vga_ctrl->VRAM[(i * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[(i * 80 + j) * 2 + 1] = check_col(j);
				}

			}else if(shapeLayout == 3){
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[((i - 2) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 2) * 80 + j) * 2 + 1] = check_col(j);
				}
				for(int row = 0; row < 2; row++){
					for (int j = randomStart; j < randomFirstEnd; j++) {
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}
        	i++;



        	keycode = keyboard_input();
        	time = timer;
			if(keycode == 0x51){
				time = time / 1.5;
			}
			else if(keycode == 0x4F){ //left
				if(randomEnd != 59){
					if(shapeLayout == 0 && grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						secondWall += 2;
						i--;
					}else if(shapeLayout == 1
							&& grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1 && grid[i - 3][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						secondWall += 2;
						i--;
					}
					else if(shapeLayout == 2
							&& grid[i - 2][randomEnd + 1] != 1 && grid[i - 1][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						secondWall += 2;
						i--;
					}
					else if(shapeLayout == 3
							&& grid[i - 1][randomFirstEnd + 1] != 1 && grid[i - 2][randomFirstEnd + 1] != 1 && grid[i - 3][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						secondWall += 2;
						i--;
					}
				}
			}
			else if(keycode == 0x50){ //right
				if(randomStart != 21){
					if(shapeLayout == 0
							 && grid[i - 1][randomStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						secondWall -= 2;
						i--;
					}else if(shapeLayout == 1
							&& grid[i - 1][randomStart - 1] != 1 && grid[i - 2][secondWall - 1] != 1 && grid[i - 3][secondWall - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						secondWall -= 2;
						i--;
					}
					else if(shapeLayout == 2
							&& grid[i - 2][randomStart - 1] != 1 && grid[i - 1][secondWall - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						secondWall -= 2;
						i--;
					}
					else if(shapeLayout == 3
							&& grid[i - 1][randomStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1 && grid[i - 3][randomStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						secondWall -= 2;
						i--;
					}
				}

			}else if(keycode == 0x1D && i >= 3 && i < 29){ // rotate left
				i--;
				if(shapeLayout == 0 && i > 2){
					shapeLayout = 1;
					secondWall = randomStart + 2;
					randomEnd -= 2;
					i++;
				}else if(shapeLayout == 1 && randomEnd <= 57 && randomStart >= 23
						&& grid[i - 1][randomEnd + 1] != 1 && grid[i][randomEnd + 1] != 1){
					shapeLayout = 2;
					secondWall += 2;
					randomEnd += 2;
				}else if(shapeLayout == 2 && i < 29){
					shapeLayout = 3;
					randomStart += 2;
					randomFirstEnd = secondWall;
				}else if(shapeLayout == 3  && randomEnd <= 57 && randomStart >= 23
						&& grid[i][randomStart - 1] != 1 && grid[i - 1][randomStart - 1] != 1 && grid[i][randomFirstEnd + 1] != 1){
					shapeLayout = 0;
					i--;
					randomStart -= 2;
					randomFirstEnd -= 2;
				}else{
					i++;
				}
			}else if(keycode == 0x1B && i >= 3 && i < 29){ // rotate left
				i--;
				if(shapeLayout == 0 && i > 2 && grid[i - 1][randomEnd] != 1){
					shapeLayout = 3;
					randomStart += 2;
					randomFirstEnd += 2;
					i++;
				}else if(shapeLayout == 1 && randomEnd <= 57 && randomStart >= 23
						&& grid[i - 1][randomEnd + 1] != 1 && grid[i - 1][secondWall - 1] != 1 && grid[i - 2][secondWall - 1] != 1){
					shapeLayout = 0;
					randomFirstEnd = secondWall;
					randomEnd += 2;
					i--;
				}else if(shapeLayout == 2 && i < 29){
					shapeLayout = 1;
					secondWall -= 2;
					randomEnd -= 2;
				}else if(shapeLayout == 3  && randomEnd <= 57 && randomStart >= 23
						&& grid[i][randomFirstEnd + 1] != 1 && grid[i - 1][randomStart - 1] != 1){
					shapeLayout = 2;
					randomStart -= 2;
					secondWall = randomFirstEnd;
				}else{
					i++;
				}
			}else if(keycode == 0x2C){
				time = 0;
			}


			if(shapeLayout == 0){
				for (int j = randomStart; j < randomFirstEnd; j++) {
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x56;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[(i * 80 + j) * 2] = 0x56;
					vga_ctrl->VRAM[(i * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[(i * 80 + j) * 2] = 0x56;
					vga_ctrl->VRAM[(i * 80 + j) * 2 + 1] = check_col(j);
				}
				for(int row = 1; row < 3; row++){
					for (int j = secondWall; j < randomEnd; j++) {
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x56;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}else if(shapeLayout == 2){
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x56;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
				for (int j = secondWall; j < randomEnd; j++) {
					vga_ctrl->VRAM[(i * 80 + j) * 2] = 0x56;
					vga_ctrl->VRAM[(i * 80 + j) * 2 + 1] = check_col(j);
				}

			}else if(shapeLayout == 3){
				for (int j = randomStart; j < randomEnd; j++) {
					vga_ctrl->VRAM[((i - 2) * 80 + j) * 2] = 0x56;
					vga_ctrl->VRAM[((i - 2) * 80 + j) * 2 + 1] = check_col(j);
				}
				for(int row = 0; row < 2; row++){
					for (int j = randomStart; j < randomFirstEnd; j++) {
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x56;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}



        }
        count++;
        usleep (time);
    }
    if(shapeLayout == 0){
		for (int j = randomStart; j < randomFirstEnd; j++) {
			grid[28][j] = 1;
			rowCount[28]++;
		}
		for (int j = randomStart; j < randomEnd; j++) {
			grid[29][j] = 1;
			rowCount[29]++;
		}
	}else if(shapeLayout == 1){
		for (int j = randomStart; j < randomEnd; j++) {
			grid[29][j] = 1;
			rowCount[29]++;
		}
		for(int row = 1; row < 3; row++){
			for (int j = secondWall; j < randomEnd; j++) {
				grid[29 - row][j] = 1;
				rowCount[29 - row]++;
			}
		}
	}else if(shapeLayout == 2){
		for (int j = randomStart; j < randomEnd; j++) {
			grid[28][j] = 1;
			rowCount[28]++;
		}
		for (int j = secondWall; j < randomEnd; j++) {
			grid[29][j] = 1;
			rowCount[29]++;
		}

	}else if(shapeLayout == 3){
		for (int j = randomStart; j < randomEnd; j++) {
			grid[27][j] = 1;
			rowCount[27]++;
		}
		for(int row = 0; row < 2; row++){
			for (int j = randomStart; j < randomFirstEnd; j++) {
				grid[29 - row][j] = 1;
				rowCount[29 - row]++;
			}
		}
	}
}

void dropLBlock() {
    int count = 0;
    BYTE keycode;
    int randomStart = rand()%30+21;
    if(randomStart % 2 == 0){
    	if(randomStart == 52){
    		randomStart--;
    	}else{
    		randomStart++;
    	}
    }
    int randomFirstStart = randomStart + 4;
    int randomEnd = randomStart + 6;
    int time = timer;
    int secondEnd = 0;
    int shapeLayout = 0; // 0 = default, 1 = 1 left,  2 = 2 left....
    checkEndGame(randomStart, randomEnd, 2);
    for (int i = 2; i < 30; i++) {
        if (count == 0) {



        	for (int j = randomFirstStart; j < randomEnd; j++) {
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0xC4;
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
        	}

        	for (int j = randomStart; j < randomEnd; j++) {
				vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xC4;
				vga_ctrl->VRAM[(i * 80 + j) * 2 + 1] = check_col(j);
			}



        } else {

        	if(shapeLayout == 0){
				for (int j = randomStart; j < randomEnd; j++) {
			      if(grid[i][j] == 1){
			    	  for (int j = randomFirstStart; j < randomEnd; j++) {
			        		grid[i - 2][j] = 1;
			        		rowCount[i - 2]++;
						}
						for (int j = randomStart; j < randomEnd; j++) {
			//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
							grid[i - 1][j] = 1;
							rowCount[i - 1]++;
						}
						return;
			      }
				}
			}else if(shapeLayout == 1){
				for (int j = randomStart; j < randomEnd; j++) {
			      if(grid[i - 2][j] == 1){
			    	  for(int row = 0; row < 2; row++){
							for (int j = randomFirstStart; j < randomEnd; j++) {
				//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
								grid[i - 1 - row][j] = 1;
								rowCount[i - 1 -row]++;
							}
						}
						for (int j = randomStart; j < randomEnd; j++) {
			//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
							grid[i - 3][j] = 1;
							rowCount[i - 3]++;
						}
						return;
			      }
			      for (int j = randomFirstStart; j < randomEnd; j++) {
				//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
			    	  if(grid[i][j] == 1){
			    		  for(int row = 0; row < 2; row++){
								for (int j = randomFirstStart; j < randomEnd; j++) {
					//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
									grid[i - 1 - row][j] = 1;
									rowCount[i - 1 - row]++;
								}
							}
							for (int j = randomStart; j < randomEnd; j++) {
				//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
								grid[i - 3][j] = 1;
								rowCount[i - 3]++;
							}
							return;
			    	  }

			      }
				}
			}else if(shapeLayout == 2){
				for (int j = randomStart; j < randomEnd; j++) {	//potential bug
			      if(grid[i - 1][j] == 1){
			    	  for (int j = randomStart; j < randomEnd; j++) {
			        		grid[i - 2][j] = 1;
			        		rowCount[i - 2]++;
						}
						for (int j = randomStart; j < secondEnd; j++) {
			        		grid[i - 1][j] = 1;
			        		rowCount[i - 1]++;
						}
						return;
			      }
				}
				for (int j = randomStart; j < secondEnd; j++) {
			      if(grid[i][j] == 1){
			    	  for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 2][j] = 1;
							rowCount[i - 2]++;
						}
						for (int j = randomStart; j < secondEnd; j++) {
							grid[i - 1][j] = 1;
							rowCount[i - 1]++;
						}
						return;
			      }
				}
			}else if(shapeLayout == 3){

				for (int j = randomStart; j < randomEnd; j++) {
			      if(grid[i][j] == 1){
			    	  for(int row = 1; row < 3; row++){
							for (int j = randomStart; j < secondEnd; j++) {
				        		grid[i - 1 - row][j] = 1;
				        		rowCount[i - 1 -row]++;
							}
						}
						for (int j = randomStart; j < randomEnd; j++) {
			        		grid[i - 1][j] = 0;
			        		rowCount[i - 1]++;
						}
						return;
			      }
				}
			}

        	i--;
        	if(shapeLayout == 0){
				for (int j = randomFirstStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for(int row = 0; row < 2; row++){
					for (int j = randomFirstStart; j < randomEnd; j++) {
		//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 2) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 2) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 2){
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
				for (int j = randomStart; j < secondEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 3){
				for(int row = 1; row < 3; row++){
					for (int j = randomStart; j < secondEnd; j++) {
		//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}





//        	for (int j = randomStart; j < randomEnd; j++) {
////        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x00;
//            		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
//            		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
//        	}
//
//        	for (int j = randomStart; j < randomEnd; j++) {
////        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x00;
//            		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
//            		vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
//
//        	}
        	i++;

        	keycode = keyboard_input();
        	time = timer;
			if(keycode == 0x51){
				time = timer / 1.5;
			}
			else if(keycode == 0x4F && grid[i-1][randomStart - 1] != 1){
				if(randomEnd != 59){
					if(shapeLayout == 0
							&& grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstStart += 2;
						secondEnd += 2;
						i--;
					}else if(shapeLayout == 1
							&& grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1 && grid[i - 3][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstStart += 2;
						secondEnd += 2;
						i--;
					}else if(shapeLayout == 2
							&& grid[i - 1][secondEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstStart += 2;
						secondEnd += 2;
						i--;
					}else if(shapeLayout == 3
							&& grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][secondEnd + 1] != 1 && grid[i - 3][secondEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstStart += 2;
						secondEnd += 2;
						i--;
					}

				}
			}
			else if(keycode == 0x50 && grid[i-1][randomEnd + 1] != 1){
				if(randomStart != 21){
					if(shapeLayout == 0
							&& grid[i - 1][randomStart - 1] != 1 && grid[i - 2][randomFirstStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstStart -= 2;
						secondEnd -= 2;
						i--;
					}else if(shapeLayout == 1
							&& grid[i - 1][randomFirstStart - 1] != 1 && grid[i - 2][randomFirstStart - 1] != 1 && grid[i - 3][randomStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstStart -= 2;
						secondEnd -= 2;
						i--;
					}else if(shapeLayout == 2
							&& grid[i - 1][randomStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstStart -= 2;
						secondEnd -= 2;
						i--;
					}else if(shapeLayout == 3
							&& grid[i - 1][randomStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1 && grid[i - 3][randomStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstStart -= 2;
						secondEnd -= 2;
						i--;
					}

				}
			}else if(keycode == 0x1D){
				i--;
				if(shapeLayout == 0  && i > 2 && i < 29
						&& grid[i - 2][randomFirstStart - 1] != 1 && grid[i-2][randomStart] != 1){
					shapeLayout = 1;
					randomFirstStart -= 2;
					randomEnd -= 2;
					i++;
				}else if(shapeLayout == 1 && randomStart >= 23 && randomEnd <= 57
						&& grid[i][randomFirstStart - 1] != 1 && grid[i - 1][randomEnd + 1] != 1){
					shapeLayout = 2;
					randomEnd += 2;
					secondEnd = randomFirstStart;
				}else if(shapeLayout == 2  && i > 2 && i < 29
						&& grid[i - 2][randomStart + 2] != 1){
					shapeLayout = 3;
					randomStart += 2;
					secondEnd += 2;
				}else if(shapeLayout == 3  && randomStart >= 23 && randomEnd <= 57
						&& grid[i - 1][randomStart - 1] != 1 && grid[i - 1][randomEnd] != 1 && grid[i - 2][randomEnd] != 1){
					shapeLayout = 0;
					randomStart -= 2;
					randomFirstStart = secondEnd;
					i--;
				}else{
					i++;
				}
			}else if(keycode == 0x1B){
				i--;
				if(shapeLayout == 0 && i > 2 && i < 29
						&& grid[i - 2][secondEnd + 1] != 1){
					shapeLayout = 3;
					randomStart += 2;
					secondEnd = randomFirstStart;
					i++;
				}else if(shapeLayout == 1  && randomStart >= 23 && randomEnd <= 57
						&& grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1){
					shapeLayout = 0;
					randomFirstStart += 2;
					randomEnd += 2;
					i--;
				}else if(shapeLayout == 2 && i > 2 && i < 29
						&& grid[i - 2][randomStart] != 1 && grid[i - 2][randomStart + 2] != 1){
					shapeLayout = 1;
					randomFirstStart = secondEnd;
					randomEnd -= 2;
				}else if(shapeLayout == 3 && randomStart >= 23 && randomEnd <= 57
						&& grid[i][randomStart - 1] != 1 && grid[i - 1][randomStart - 1] != 1 && grid[i - 1][randomEnd] != 1){
					shapeLayout = 2;
					randomStart -= 2;
					secondEnd -= 2;
				}else{
					i++;
				}
			}else if(keycode == 0x2C){
				time = 0;
			}

        	if(shapeLayout == 0){
        		for (int j = randomFirstStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0xC4;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xC4;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
        	}else if(shapeLayout == 1){
        		for(int row = 0; row < 2; row++){
        			for (int j = randomFirstStart; j < randomEnd; j++) {
		//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0xC4;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
        		}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 2) * 80 + j) * 2] = 0xC4;
					vga_ctrl->VRAM[((i - 2) * 80 + j) * 2 + 1] = check_col(j);
				}
        	}else if(shapeLayout == 2){
        		for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0xC4;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
        		for (int j = randomStart; j < secondEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xC4;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
        	}else if(shapeLayout == 3){
        		for(int row = 1; row < 3; row++){
					for (int j = randomStart; j < secondEnd; j++) {
		//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0xC4;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xC4;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
        	}








        }
        count++;
        usleep (time);
    }

    if(shapeLayout == 0){
		for (int j = randomFirstStart; j < randomEnd; j++) {
	//
			grid[28][j] = 1;
			rowCount[28]++;
		}
		for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
			grid[29][j] = 1;
			rowCount[29]++;
		}
	}else if(shapeLayout == 1){
		for(int row = 0; row < 2; row++){
			for (int j = randomFirstStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
				grid[29 - row][j] = 1;
				rowCount[29 - row]++;
			}
		}
		for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
			grid[27][j] = 1;
			rowCount[27]++;
		}
	}else if(shapeLayout == 2){
		for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
			grid[28][j] = 1;
			rowCount[28]++;
		}
		for (int j = randomStart; j < secondEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
			grid[29][j] = 1;
			rowCount[29]++;
		}
	}else if(shapeLayout == 3){
		for(int row = 1; row < 3; row++){
			for (int j = randomStart; j < secondEnd; j++) {
	//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
				grid[29 - row][j] = 1;
				rowCount[29 - row]++;
			}
		}
		for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
			grid[29][j] = 1;
			rowCount[29]++;
		}
	}
}

void dropTBlock() {
    int count = 0;
    BYTE keycode;
    int randomStart = rand()%30+21;
    if(randomStart % 2 == 0){
    	if(randomStart == 52){
    		randomStart--;
    	}else{
    		randomStart++;
    	}
    }
    int randomFirstStart = randomStart + 2;
	int randomFirstEnd = randomStart + 4;
	int randomEnd = randomStart + 6;
	int time = timer;
	int shapeLayout = 0;
	checkEndGame(randomStart, randomEnd, 2);
    for (int i = 2; i < 30; i++) {
        if (count == 0) {
            //vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0x27F;
//            vga_ctrl->VRAM[(i * 80 + 6) * 2] = 0xFF;
//            vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0xFF;


        	for (int j = randomFirstStart; j < randomFirstEnd; j++) {
//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x76;
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = check_col(j);

        	}

        	for (int j = randomStart; j < randomEnd; j++) {
				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x76;
				vga_ctrl->VRAM[((i) * 80 + j) * 2] = check_col(j);
			}



        } else {

        	if(shapeLayout == 0){
				for (int j = randomStart; j < randomEnd; j++) {
					if(grid[i][j] == 1){
						for (int j = randomFirstStart; j < randomFirstEnd; j++) {
			        		grid[i - 2][j] = 1;
			        		rowCount[i - 2]++;

						}

						for (int j = randomStart; j < randomEnd; j++) {
			        		grid[i - 1][j] = 1;
			        		rowCount[i - 1]++;
						}
						return;
					}

				}
			}else if(shapeLayout == 1){
				for (int j = randomFirstStart; j < randomEnd; j++) {
					if(grid[i][j] == 1){
						for(int row = 0; row  < 3; row++){
							for (int j = randomFirstStart; j < randomEnd; j++) {
				        		grid[i - 1 - row][j] = 1;
				        		rowCount[i - 1 - row]++;
							}
						}
						for (int j = randomStart; j < randomFirstStart; j++) {
			        		grid[i - 2][j] = 1;
			        		rowCount[i - 2]++;
						}
						return;
					}
				}
				for (int j = randomStart; j < randomFirstStart; j++) {
					if(grid[i - 1][j] == 1){
						for(int row = 0; row  < 3; row++){
							for (int j = randomFirstStart; j < randomEnd; j++) {
								grid[i - 1 - row][j] = 1;
								rowCount[i - 1 - row]++;
							}
						}
						for (int j = randomStart; j < randomFirstStart; j++) {
							grid[i - 2][j] = 1;
							rowCount[i - 2]++;
						}
						return;
					}
				}

			}else if(shapeLayout == 2){
				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
					if(grid[i][j] == 1){
						for (int j = randomFirstStart; j < randomFirstEnd; j++) {
			        		grid[i - 1][j] = 1;
			        		rowCount[i - 1]++;
						}
						for (int j = randomStart; j < randomEnd; j++) {
			        		grid[i - 2][j] = 1;
			        		rowCount[i - 2]++;
						}
						return;
					}
				}
				for (int j = randomStart; j < randomEnd; j++) {
					if(grid[i-1][j] == 1){
						for (int j = randomFirstStart; j < randomFirstEnd; j++) {
							grid[i - 1][j] = 1;
							rowCount[i - 1]++;
						}
						for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 2][j] = 1;
							rowCount[i - 2]++;
						}
						return;
					}
				}
			}else if(shapeLayout == 3){
				for (int j = randomStart; j < randomFirstEnd; j++) {
					if(grid[i][j] == 1){
						for(int row = 0; row  < 3; row++){
							for (int j = randomStart; j < randomFirstEnd; j++) {
								grid[i - 1 - row][j] = 1;
								rowCount[i - 1 - row]++;
							}
						}
						for (int j = randomFirstEnd; j < randomEnd; j++) {
							grid[i - 2][j] = 1;
							rowCount[i - 2]++;
						}
						return;
					}
				}
				for (int j = randomFirstEnd; j < randomEnd; j++) {
					if(grid[i - 1][j] == 1){
						for(int row = 0; row  < 3; row++){
							for (int j = randomStart; j < randomFirstEnd; j++) {
				        		grid[i - 1 - row][j] = 1;
				        		rowCount[i - 1 - row]++;
							}
						}
						for (int j = randomFirstEnd; j < randomEnd; j++) {
			        		grid[i - 2][j] = 1;
			        		rowCount[i - 2]++;
						}
						return;
					}
				}
			}


        	i--;
        	if(shapeLayout == 0){
        		for (int j = randomFirstStart; j < randomFirstEnd; j++) {
	//        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x00;
						vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);

				}

				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x00;
						vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for(int row = 0; row  < 3; row++){
					for (int j = randomFirstStart; j < randomEnd; j++) {
		//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
				for (int j = randomStart; j < randomFirstStart; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 2){
				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
	//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 3){
				for(int row = 0; row  < 3; row++){
					for (int j = randomStart; j < randomFirstEnd; j++) {
		//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
				for (int j = randomFirstEnd; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}
			}
        	i++;

        	keycode = keyboard_input();
        	time = timer;
			if(keycode == 0x51){
				time = timer / 1.5;
			}
			else if(keycode == 0x4F){
				if(randomEnd != 59){
					if(shapeLayout == 0
							&& grid[i-1][randomEnd + 1] != 1 && grid[i - 2][randomFirstEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						randomFirstStart += 2;
						i--;
					}else if(shapeLayout == 1
							&& grid[i-1][randomFirstEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1 && grid[i-3][randomFirstEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						randomFirstStart += 2;
						i--;
					}else if(shapeLayout == 2
							&& grid[i-2][randomEnd + 1] != 1 && grid[i - 1][randomFirstEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						randomFirstStart += 2;
						i--;
					}else if(shapeLayout == 3
							&& grid[i-1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1 && grid[i-3][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						randomFirstStart += 2;
						i--;
					}


				}
			}
			else if(keycode == 0x50){
				if(randomStart != 21){
					if(shapeLayout == 0
							&& grid[i-1][randomStart - 1] != 1 && grid[i - 2][randomFirstStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						randomFirstStart -= 2;
						i--;
					}else if(shapeLayout == 1
							&& grid[i-1][randomFirstStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1 && grid[i-3][randomFirstStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						randomFirstStart -= 2;
						i--;
					}else if(shapeLayout == 2
							&& grid[i-2][randomStart - 1] != 1 && grid[i - 1][randomFirstStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						randomFirstStart -= 2;
						i--;
					}else if(shapeLayout == 3
							&& grid[i-1][randomStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1 && grid[i-3][randomStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						randomFirstStart -= 2;
						i--;
					}
				}
			}else if(keycode == 0x1D){
				i--;
				if(shapeLayout == 0){
					i++;
					shapeLayout = 1;
					randomEnd -= 2;
				}else if(shapeLayout == 1 && randomStart >= 23 && randomEnd <= 57
						&& grid[i - 1][randomEnd + 1] != 1){
					shapeLayout = 2;
					randomEnd += 2;
				}else if(shapeLayout == 2 && grid[i - 2][randomStart + 2] != 1){
					shapeLayout = 3;
					randomStart += 2;
				}else if(shapeLayout == 3 && randomStart >= 23 && randomEnd <= 57
						&& grid[i - 1][randomStart - 1] != 1){
					shapeLayout = 0;
					randomStart -= 2;
					i--;
				}else{
					i++;
				}
			}else if(keycode == 0x1B && i > 2 && i < 29){
				i--;
				if(shapeLayout == 0){
					i++;
					shapeLayout = 3;
					randomStart += 2;
				}else if(shapeLayout == 1 && randomStart >= 23 && randomEnd <= 57
						&& grid[i - 1][randomEnd + 1] != 1){
					shapeLayout = 0;
					randomEnd += 2;
					i--;
				}else if(shapeLayout == 2 && grid[i - 2][randomStart + 2] != 1){
					shapeLayout = 1;
					randomEnd -= 2;
				}else if(shapeLayout == 3 && randomStart >= 23 && randomEnd <= 57
						&& grid[i - 1][randomStart - 1] != 1){
					shapeLayout = 2;
					randomStart -= 2;
				}else{
					i++;
				}
			}else if(keycode == 0x2C){
				time = 0;
			}

			if(shapeLayout == 0){
				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
	//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x76;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = check_col(j);
				}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x76;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for(int row = 0; row  < 3; row++){
					for (int j = randomFirstStart; j < randomEnd; j++) {
		//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x76;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = check_col(j);
					}
				}
				for (int j = randomStart; j < randomFirstStart; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x76;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = check_col(j);
				}
			}else if(shapeLayout == 2){
				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
	//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x76;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = check_col(j);
				}
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x76;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = check_col(j);
				}
			}else if(shapeLayout == 3){
				for(int row = 0; row  < 3; row++){
					for (int j = randomStart; j < randomFirstEnd; j++) {
		//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x76;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = check_col(j);
					}
				}
				for (int j = randomFirstEnd; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x76;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = check_col(j);
				}
			}




        }
        count++;
        usleep (time);
    }
    if(shapeLayout == 0){
		for (int j = randomFirstStart; j < randomFirstEnd; j++) {
        		grid[28][j] = 1;
        		rowCount[28]++;

		}

		for (int j = randomStart; j < randomEnd; j++) {
			grid[29][j] = 1;
			rowCount[29]++;
		}
	}else if(shapeLayout == 1){
		for(int row = 0; row  < 3; row++){
			for (int j = randomFirstStart; j < randomEnd; j++) {
        		grid[29 - row][j] = 1;
        		rowCount[29 -row]++;
			}
		}
		for (int j = randomStart; j < randomFirstStart; j++) {
        	grid[28][j] = 1;
        	rowCount[28]++;
		}
	}else if(shapeLayout == 2){
		for (int j = randomFirstStart; j < randomFirstEnd; j++) {
        	grid[29][j] = 1;
        	rowCount[29]++;
		}
		for (int j = randomStart; j < randomEnd; j++) {
        	grid[28][j] = 1;
        	rowCount[28]++;
		}
	}else if(shapeLayout == 3){
		for(int row = 0; row  < 3; row++){
			for (int j = randomStart; j < randomFirstEnd; j++) {
        		grid[29 - row][j] = 1;
        		rowCount[29 - row]++;
			}
		}
		for (int j = randomFirstEnd; j < randomEnd; j++) {
//        		vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
			grid[28][j] = 1;
			rowCount[28]++;
		}
	}
}

void dropSBlock() {
    int count = 0;
    BYTE keycode;
    int randomStart = rand()%32+21;
    if(randomStart % 2 == 0){
    	if(randomStart == 54){
    		randomStart--;
    	}else{
    		randomStart++;
    	}
    }
    int randomFirstStart = randomStart + 2;
	int randomFirstEnd = randomStart + 6;
	int randomEnd = randomStart + 4;
	int time = timer;
	int shapeLayout = 0;
	checkEndGame(randomStart, randomFirstEnd, 2);
    for (int i = 2; i < 30; i++) {
        if (count == 0) {

        	for (int j = randomFirstStart; j < randomFirstEnd; j++) {
//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0xED;
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
        	}

			for (int j = randomStart; j < randomEnd; j++) {
//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xED;
				vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
			}



        } else {

        	if(shapeLayout == 0){
				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
					if(grid[i - 1][j] == 1){
						for (int j = randomFirstStart; j < randomFirstEnd; j++) {
			        		grid[i - 2][j] = 1;
			        		rowCount[i - 2]++;
						}


						for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 1][j] = 1;
							rowCount[i - 1]++;
						}
						return;
					}
				}


				for (int j = randomStart; j < randomEnd; j++) {
					if(grid[i][j] == 1){
						for (int j = randomFirstStart; j < randomFirstEnd; j++) {
							grid[i - 2][j] = 1;
							rowCount[i - 2]++;
						}


						for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 1][j] = 1;
							rowCount[i - 1]++;
						}
						return;
					}
				}
			}else if(shapeLayout == 1){
				for (int j = randomStart; j < randomFirstEnd; j++) {
					if(grid[i - 1][j] == 1){
						for(int row = 0; row < 2; row++){
							for (int j = randomStart; j < randomFirstEnd; j++) {
				        		grid[i - 2 - row][j] = 1;
				        		rowCount[i - 2 - row]++;
							}


							for (int j = randomFirstStart; j < randomEnd; j++) {
								grid[i - 1 - row][j] = 1;
								rowCount[i - 1 - row]++;
							}
						}
						return;
					}
				}


				for (int j = randomFirstStart; j < randomEnd; j++) {
					if(grid[i][j] == 1){
						for(int row = 0; row < 2; row++){
							for (int j = randomStart; j < randomFirstEnd; j++) {
								grid[i - 2 - row][j] = 1;
								rowCount[i - 2 - row]++;
							}


							for (int j = randomFirstStart; j < randomEnd; j++) {
								grid[i - 1 - row][j] = 1;
								rowCount[i - 1 - row]++;
							}
						}
						return;
					}
				}
			}

        	i--;
			if(shapeLayout == 0){
				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
	//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}


				for (int j = randomStart; j < randomEnd; j++) {
	//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for(int row = 0; row < 2; row++){
					for (int j = randomStart; j < randomFirstEnd; j++) {
		//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - 1 - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - 1 - row) * 80 + j) * 2 + 1] = check_col(j);
					}


					for (int j = randomFirstStart; j < randomEnd; j++) {
		//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}

        	i++;

        	keycode = keyboard_input();
        	time = timer;
			if(keycode == 0x51){
				time = timer / 1.5;
			}
			else if(keycode == 0x4F){
				if((randomFirstEnd != 59 && shapeLayout == 0) || (randomEnd != 59 && shapeLayout == 1)){
					if(shapeLayout == 0 && grid[i - 2][randomFirstEnd + 1] != 1 && grid[i - 1][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						randomFirstStart += 2;
						i--;
					}else if(shapeLayout == 1
							&& grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1 && grid[i - 3][randomFirstEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						randomFirstStart += 2;
						i--;
					}
				}
			}else if(keycode == 0x50){
				if(randomStart != 21){
						if(shapeLayout == 0 && grid[i - 2][randomFirstStart - 1] != 1 && grid[i - 1][randomStart - 1] != 1){
							randomStart -= 2;
							randomEnd -= 2;
							randomFirstEnd -= 2;
							randomFirstStart -= 2;
							i--;
						}else if(shapeLayout == 1
							&& grid[i - 1][randomFirstStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1 && grid[i - 3][randomStart - 1] != 1){
						randomStart -= 2;
						randomEnd -= 2;
						randomFirstEnd -= 2;
						randomFirstStart -= 2;
						i--;
					}
				}
			}else if((keycode == 0x1D || keycode == 0x1B) && i > 2 && i < 29){
				i--;
				if(shapeLayout == 0 && i > 2 && grid[i - 1][randomStart] != 1 && grid[i - 2][randomStart] != 1){
					shapeLayout = 1;
					randomFirstEnd -= 4;
				}else if(shapeLayout == 1 && grid[i - 1][randomEnd + 1] != 1
						&& randomStart > 23 && randomEnd < 57){
					shapeLayout = 0;
					randomFirstEnd += 4;
				}else{
					i++;
				}
			}else if(keycode == 0x2C){
				time = 0;
			}

			if(shapeLayout == 0){
				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
	//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0xED;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}


				for (int j = randomStart; j < randomEnd; j++) {
	//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xED;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for(int row = 0; row < 2; row++){
					for (int j = randomStart; j < randomFirstEnd; j++) {
		//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - 1 - row) * 80 + j) * 2] = 0xED;
						vga_ctrl->VRAM[((i - 1 - row) * 80 + j) * 2 + 1] = check_col(j);
					}


					for (int j = randomFirstStart; j < randomEnd; j++) {
		//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0xED;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}


        }
        count++;
        usleep (time);
    }
    if(shapeLayout == 0){
		for (int j = randomFirstStart; j < randomFirstEnd; j++) {
        	grid[28][j] = 1;
        	rowCount[28]++;
		}


		for (int j = randomStart; j < randomEnd; j++) {
			grid[29][j] = 1;
			rowCount[29]++;
		}
	}else if(shapeLayout == 1){
		for(int row = 0; row < 2; row++){
			for (int j = randomStart; j < randomFirstEnd; j++) {
        		grid[28 - row][j] = 1;
        		rowCount[28 - row]++;
			}


			for (int j = randomFirstStart; j < randomEnd; j++) {
				grid[29 - row][j] = 1;
				rowCount[29 - row]++;
			}
		}
	}

}

void dropZBlock() {
    int count = 0;
    BYTE keycode;
    int randomStart = rand()%32+21;
    if(randomStart % 2 == 0){
		if(randomStart == 54){
			randomStart--;
		}else{
			randomStart++;
		}
	}
    int randomFirstStart = randomStart + 2;
	int randomFirstEnd = randomStart + 6;
	int randomEnd = randomStart + 4;
	int time = timer;
	int shapeLayout = 0;
	checkEndGame(randomStart, randomFirstEnd, 2);
    for (int i = 2; i < 30; i++) {
        if (count == 0) {
            //vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0x27F;
//            vga_ctrl->VRAM[(i * 80 + 6) * 2] = 0xFF;
//            vga_ctrl->VRAM[(i * 80 + 5) * 2] = 0xFF;

        	for (int j = randomStart; j < randomEnd; j++) {
//        		vga_ctrl->VRAM[(i * 80 + j) * 2] = 0xFF;
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0xA9;
        		vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
        	}

			for (int j = randomFirstStart; j < randomFirstEnd; j++) {
//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
				vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xA9;
				vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
			}



        } else {

        	if(shapeLayout == 0){
				for (int j = randomStart; j < randomEnd; j++) {
					if(grid[i -1][j] == 1){
						for (int j = randomStart; j < randomEnd; j++) {
			        		grid[i - 2][j] = 1;
			        		rowCount[i - 2]++;
						}


						for (int j = randomFirstStart; j < randomFirstEnd; j++) {
							grid[i - 1][j] = 1;
							rowCount[i - 1]++;
						}
						return;
					}
				}


				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
					if(grid[i][j] == 1){
						for (int j = randomStart; j < randomEnd; j++) {
							grid[i - 2][j] = 1;
							rowCount[i - 2]++;
						}


						for (int j = randomFirstStart; j < randomFirstEnd; j++) {
							grid[i - 1][j] = 1;
							rowCount[i - 1]++;
						}
						return;
					}
				}
			}else if(shapeLayout == 1){
				for (int j = randomStart; j < randomFirstEnd; j++) {
					if(grid[i -1][j] == 1){
						for(int row = 0; row < 2; row++){
							for (int j = randomStart; j < randomFirstEnd; j++) {
				        		grid[i - 2 - row][j] = 1;
				        		rowCount[i - 2 - row]++;
							}


							for (int j = randomFirstStart; j < randomEnd; j++) {
								grid[i - 1 - row][j] = 1;
								rowCount[i - 1 - row]++;
							}
						}
						return;

					}
				}


				for (int j = randomFirstStart; j < randomEnd; j++) {
					if(grid[i][j] == 1){
						for(int row = 0; row < 2; row++){
							for (int j = randomStart; j < randomFirstEnd; j++) {
								grid[i - 2 - row][j] = 1;
								rowCount[i - 2 - row]++;
							}


							for (int j = randomFirstStart; j < randomEnd; j++) {
								grid[i - 1 - row][j] = 1;
								rowCount[i - 1 -row]++;
							}
						}
						return;
					}
				}
			}

        	i--;


        	if(shapeLayout == 0){
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}


				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
	//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0x10;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for(int row = 0; row < 2; row++){
					for (int j = randomStart; j < randomFirstEnd; j++) {
		//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - 1 - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - 1 - row) * 80 + j) * 2 + 1] = check_col(j);
					}


					for (int j = randomFirstStart; j < randomEnd; j++) {
		//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0x10;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}

        	i++;

        	keycode = keyboard_input();
        	time = timer;
			if(keycode == 0x51){
				time = timer / 1.5;
			}
			else if(keycode == 0x4F){
				if(randomEnd != 57){
					if(shapeLayout == 0 && grid[i - 1][randomFirstEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						randomFirstStart += 2;
						i--;
					}else if(shapeLayout == 1
							&& grid[i - 3][randomFirstEnd + 1] != 1 && grid[i - 2][randomFirstEnd + 1] != 1 && grid[i - 1][randomEnd + 1] != 1){
						randomStart += 2;
						randomEnd += 2;
						randomFirstEnd += 2;
						randomFirstStart += 2;
						i--;
					}

				}
			}
			else if(keycode == 0x50){
				if(shapeLayout == 0 && grid[i - 1][randomFirstStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1 && randomStart > 21){
					randomStart -= 2;
					randomEnd -= 2;
					randomFirstEnd -= 2;
					randomFirstStart -= 2;
					i--;
				}else if(shapeLayout == 1
						&& grid[i - 3][randomStart - 1] != 1 && grid[i - 2][randomStart - 1] != 1 && grid[i - 1][randomFirstStart - 1] != 1 && randomStart > 23){
					randomStart -= 2;
					randomEnd -= 2;
					randomFirstEnd -= 2;
					randomFirstStart -= 2;
					i--;
				}
			}else if((keycode == 0x1D || keycode == 0x1B)){
				i--;
				if(shapeLayout == 0  && i > 2
						&& grid[i - 1][randomEnd + 1] != 1 && grid[i - 2][randomEnd + 1] != 1){
					shapeLayout = 1;
					randomStart += 4;
				}else if(shapeLayout == 1
						&& grid[i - 1][randomFirstStart - 1] != 1 && randomStart > 23){
					shapeLayout = 0;
					randomStart -= 4;
				}else{
					i++;
				}
			}else if(keycode == 0x2C){
				time = 0;
			}

			if(shapeLayout == 0){
				for (int j = randomStart; j < randomEnd; j++) {
	//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2] = 0xA9;
					vga_ctrl->VRAM[((i - 1) * 80 + j) * 2 + 1] = check_col(j);
				}


				for (int j = randomFirstStart; j < randomFirstEnd; j++) {
	//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
					vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xA9;
					vga_ctrl->VRAM[((i) * 80 + j) * 2 + 1] = check_col(j);
				}
			}else if(shapeLayout == 1){
				for(int row = 0; row < 2; row++){
					for (int j = randomStart; j < randomFirstEnd; j++) {
		//        		vga_ctrl->VRAM[((i) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - 1 - row) * 80 + j) * 2] = 0xA9;
						vga_ctrl->VRAM[((i - 1 - row) * 80 + j) * 2 + 1] = check_col(j);
					}


					for (int j = randomFirstStart; j < randomEnd; j++) {
		//				vga_ctrl->VRAM[((i + 1) * 80 + j) * 2] = 0xFF;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2] = 0xA9;
						vga_ctrl->VRAM[((i - row) * 80 + j) * 2 + 1] = check_col(j);
					}
				}
			}




        }
        count++;
        usleep(time);
    }
    if(shapeLayout == 0){
		for (int j = randomStart; j < randomEnd; j++) {
        	grid[28][j] = 1;
        	rowCount[28]++;
		}


		for (int j = randomFirstStart; j < randomFirstEnd; j++) {
			grid[29][j] = 1;
			rowCount[29]++;
		}
	}else if(shapeLayout == 1){
		for(int row = 0; row < 2; row++){
			for (int j = randomStart; j < randomFirstEnd; j++) {
				grid[28 - row][j] = 1;
				rowCount[28 - row]++;
			}


			for (int j = randomFirstStart; j < randomEnd; j++) {
				grid[29 - row][j] = 1;
				rowCount[29 - row]++;
			}
		}
	}

}

void ContinueGame() {
	startGame();
	create_background();
	score = 0;
	int length = floor(log10(abs(score))) + 1;
	printf("%d" ,length);
	int count = 0;
	int waitCount = 10;

	if (highScore != 0) {
		updateHighScore();
	}

	textVGADrawColorText("Score",65, 10, 0xA9, 0x00);
	textVGADrawColorText("0",65,11, 0xA9, 0x00);


	textVGADrawColorText("Right Arrow = ", 1, 5, 0xA9, 0x00);
	textVGADrawColorText("move right", 1, 6, 0xA9, 0x00);
	textVGADrawColorText("Left Arrow = left", 1, 8, 0xA9, 0x00);
	textVGADrawColorText("move left", 1, 9, 0xA9, 0x00);
	textVGADrawColorText("Down Arrow = ",1, 11, 0xA9, 0x00);
	textVGADrawColorText("drop faster",1, 12, 0xA9, 0x00);
	textVGADrawColorText("Z = rotate left",1, 14, 0xA9, 0x00);
	textVGADrawColorText("X = rotate right",1, 15, 0xA9, 0x00);
	textVGADrawColorText("Have Fun!",1, 16, 0xA9, 0x00);

	srand(time(0));
	while (1) {
		if(count == waitCount){
			waitCount += count;
			count = 0;
			timer = timer/1.5;
		}
		int randomNumber = rand()%7;
//		int randomNumber = 5;
		switch (randomNumber) {
			case 0:
				dropSquare();
				break;
			case 1:
				dropRectangle();
				break;
			case 2:
				dropJBlock();
				break;
			case 3:
				dropLBlock();
				break;
			case 4:
				dropTBlock();
				break;
			case 5:
				dropZBlock();
				break;
			case 6:
				dropSBlock();
				break;
			default:
				break;
		}
		count++;
		check_rows();
	}
}

char check_col(int col){
	if(col % 2 == 0){
		return ']';
	}else{
		return '[';
	}
}

void check_rows(){
	rowCount[0] = 0;
	for(int i = 29; i > 0; i--){
		printf("%d ", rowCount[i]);
		if(rowCount[i] >= 38){
			clearRow(i);
			shiftGrid(i);
			score += 5;
			updateScore();
			i++;
		}
	}
}

void clearRow(int row){
	rowCount[row] = rowCount[row - 1];
	for(int j = 21; j < 59; j++){
		if(grid[row - 1][j] == 1){
			vga_ctrl->VRAM[((row * 80) + j) * 2] = 0xA9;
			vga_ctrl->VRAM[((row * 80) + j) * 2 + 1] = check_col(j);
		}else{
			vga_ctrl->VRAM[((row * 80) + j) * 2] = 0x10;
			vga_ctrl->VRAM[((row * 80) + j) * 2 + 1] = check_col(j);
		}
		grid[row][j] = grid[row - 1][j];
	}
}

void shiftGrid(int row){
	for(int i = row; i > 0; i--){
		rowCount[i] = rowCount[i - 1];
		for(int j = 21; j < 59; j++){
			if(grid[i - 1][j] == 1){
				vga_ctrl->VRAM[((i * 80) + j) * 2] = 0xA9;
				vga_ctrl->VRAM[((i * 80) + j) * 2 + 1] = check_col(j);
			}else{
				vga_ctrl->VRAM[((i * 80) + j) * 2] = 0x10;
				vga_ctrl->VRAM[((i * 80) + j) * 2 + 1] = check_col(j);
			}

			grid[i][j] = grid[i - 1][j];
		}
	}
}

void updateScore(){

	int length = floor(log10(abs(score))) + 1;
	int tempScore = score;
	char scoreToPrint[length + 1];
	int index = length;
	scoreToPrint[length] = '\0';
	index-=1;

	while (tempScore > 0) {
		int digit = tempScore % 10;
		char c = digit + '0';
		scoreToPrint[index] = c;
		index-=1;
		tempScore /= 10;
	}

	scoreToPrint[index] = '\0';


	textVGADrawColorText("Score",65, 10, 0xA9, 0x00);
	textVGADrawColorText(scoreToPrint,65,11, 0xA9, 0x00);
}

int checkEndGame(int start, int end, int row){
	if (rowCount[0] >= 1) {
		endScreen();
		return 0;
	}
	for(int j = row; j > 0; j--){
		for(int i = start; i < end; i++){
			if(grid[j][i] == 1){
				endScreen();
			}
		}
	}
	return 0;
}

int endScreen(){
	BYTE keyCode;
	textVGADrawColorText("Final Score",65, 10, 0xA9, 0x00);
	if(highScore < score){
		highScore = score;
		updateHighScore();
	}
	while(1){
		textVGADrawColorText("Press R to",65, 12, 0xA9, 0x00);
		textVGADrawColorText("Restart",65, 13, 0xA9, 0x00);
		keyCode = keyboard_input();
		if (keyCode == 0x15) {
			break;
		}
	}
	for (int i = 0; i < 33; i++) {
		rowCount[i] = 0;
	}
	for (int i = 0; i < 31; i++) {
		for (int j = 0; j < 40; j++) {
			grid[i][j] = 0;
		}
	}

	timer = 75000;
	ContinueGame();
}

void startGame() {
	BYTE keyCode;
	int col = 0;
	int row = 0;
	for(int i = 0; i < 2400; i++){
		col = i % 80;
		row = i / 80;
		if(col == 20){
//			vga_ctrl -> VRAM[i * 2] = 0x0F2F; //top right
			vga_ctrl->VRAM[i * 2] = 0x00;
			vga_ctrl->VRAM[i * 2 + 1] = 0x00;
			printf("top right");
		}
		else if(col == 19){
//			vga_ctrl -> VRAM[i * 2] = 0x012F; //top right
			vga_ctrl->VRAM[i * 2] = 0x00;
			vga_ctrl->VRAM[i * 2 + 1] = 0x00;
			printf("top right");
		}
		else if(col == 59){
//			vga_ctrl -> VRAM[i * 2] = 0x007F; //top right
			vga_ctrl->VRAM[i * 2] = 0x00;
			vga_ctrl->VRAM[i * 2 + 1] = 0x00;
			printf("top right");
		}
		else if(col == 60){
//			vga_ctrl -> VRAM[i * 2] = 0x017F; //top right
			vga_ctrl->VRAM[i * 2] = 0x00;
			vga_ctrl->VRAM[i * 2 + 1] = 0x00;
			printf("top right");
		}
		else if(row == 0 && (col > 20 && col < 59) && col % 2 == 0){
//			vga_ctrl -> VRAM[i * 2] = 0x007F; //top right
			vga_ctrl->VRAM[i * 2] = 0x00;
			vga_ctrl->VRAM[i * 2 + 1] = 0x00;
			printf("top right");
		}
		else if(row == 0 && (col > 20 && col < 59) && col % 2 == 1){
//			vga_ctrl -> VRAM[i * 2] = 0x007F; //top right
			vga_ctrl->VRAM[i * 2] = 0x00;
			vga_ctrl->VRAM[i * 2 + 1] = 0x00;
			printf("top right");
		}
		else if (col > 20 && col < 59 && row > 0 && row < 30){
			if(col % 2 == 0){
				vga_ctrl->VRAM[i * 2] = 0x00;
				vga_ctrl->VRAM[i * 2 + 1] = 0x00;
			}
			if(col % 2 == 1){
				vga_ctrl->VRAM[i * 2] = 0x00;
				vga_ctrl->VRAM[i * 2 + 1] = 0x00;
			}
		}
		else{
			vga_ctrl->VRAM[i * 2] = 0x00;
		}
//		count1++;
//		count2++;
//		count3++;
//		count4++;
	}




	textVGADrawColorText("Welcome To Tetris!",28, 5, 0xA9, 0x00);
	textVGADrawColorText("Press Space To Continue!",25, 6, 0xA9, 0x00);

	textVGADrawColorText("Choose Difficulty",28, 10, 0xA9, 0x00);
	textVGADrawColorText(" 1 ",33, 11, 0xA9, 0xC4);
	textVGADrawColorText(" 2 ",35, 11, 0xA9, 0x00);
	textVGADrawColorText(" 3 ",37, 11, 0xA9, 0x00);

	timer = 100000;
	while (1) {
		keyCode = keyboard_input();
		if(keyCode == 0x1E) {
			textVGADrawColorText(" 1 ",33, 11, 0xA9, 0xC4);
			textVGADrawColorText(" 2 ",35, 11, 0xA9, 0x00);
			textVGADrawColorText(" 3 ",37, 11, 0xA9, 0x00);
			timer = 75000;
		}
		if(keyCode == 0x1F) {
			textVGADrawColorText(" 1 ",33, 11, 0xA9, 0x00);
			textVGADrawColorText(" 2 ",35, 11, 0xA9, 0xC4);
			textVGADrawColorText(" 3 ",37, 11, 0xA9, 0x00);
			timer = 50000;
		}
		if(keyCode == 0x20) {
			textVGADrawColorText(" 1 ",33, 11, 0xA9, 0x00);
			textVGADrawColorText(" 2 ",35, 11, 0xA9, 0x00);
			textVGADrawColorText(" 3 ",37, 11, 0xA9, 0xC4);
			timer = 20000;
		}
		if(keyCode == 0x2C) {
			break;
		}
	}
}

void updateHighScore(){

	int length = floor(log10(abs(highScore))) + 1;
	int tempScore = highScore;
	char scoreToPrint[length + 1];
	int index = length;
	scoreToPrint[length] = '\0';
	index-=1;

	while (tempScore > 0) {
		int digit = tempScore % 10;
		char c = digit + '0';
		scoreToPrint[index] = c;
		index-=1;
		tempScore /= 10;
	}

	scoreToPrint[index] = '\0';


	textVGADrawColorText("High Score",65, 20, 0xA9, 0x00);
	textVGADrawColorText(scoreToPrint,65,21, 0xA9, 0x00);
}






