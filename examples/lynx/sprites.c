#include <lynx.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_W (160)
#define SCREEN_H (102)
#define LETTER_W (4)
#define LETTER_H (7)

#define SPR_W (8)
#define SPR_H (8)

const unsigned char sprite[] = {
    0x03,0xB8,0xC0,0x03,0xB9,0xE0,0x03,0xBB,0xF0,0x03,0x3C,0x00,0x03,0x3C,0x00,0x03,
    0xBB,0xF0,0x03,0xB9,0xE0,0x03,0xB8,0xC0,0x00,
};

typedef struct SPRITE {
	SCB_REHV_PAL scb;
	signed char hv;
	signed char vv;
} SPRITE;

// setup a custom cartridge header
SET_CART_INFO(
    BLOCKSIZE_256K,
    "Lynx Sprite Sample             ",
    "LLVM-MOS       ",
    1, 0);

unsigned short lastJoy = 0;
SPRITE *head = NULL;
SPRITE *end = NULL;
unsigned char refresh = 0;
unsigned char sprctl0 = 0;
unsigned char stats = 1;
unsigned char count = 1;

SPRITE* init_sprite()
{
	SPRITE *spr = (SPRITE*)malloc(sizeof(SPRITE));
    memset(spr, 0, sizeof(SPRITE));
	spr->scb.sprctl0 = BPP_1 | (sprctl0 % 8);
	spr->scb.sprctl1 = PACKED | REHV;
	spr->scb.sprcoll = 0;
	spr->scb.data = (unsigned char*)sprite;
	spr->scb.hpos = rand() % 150;
	spr->scb.vpos = rand() % 92;
	spr->scb.hsize = 0x0100;
	spr->scb.vsize = 0x0100;
	spr->scb.penpal[0] = (rand()%14)+1;
	spr->hv = 1;
	spr->vv = 1;

	if(end)
	{
		end->scb.next = (char*)spr;
		end = spr;
	}

	return spr;
}

void delete_sprite()
{
	SPRITE *spr = head;
	while( ((SPRITE*)spr->scb.next)->scb.next != 0)
		spr = (SPRITE*)spr->scb.next;

	free(spr->scb.next);
	spr->scb.next = 0;
	end = spr;
}

void change_sprites()
{
	SPRITE *spr = head;
	while(true)
	{
		spr->scb.sprctl0 = BPP_1 | (sprctl0 % 8);
		if(spr->scb.next == 0)
			break;

		spr = (SPRITE*)spr->scb.next;
	}
}

void handle_input(unsigned short joy)
{
	if(JOY_UP(joy) && !JOY_UP(lastJoy))
	{
		++count;
		init_sprite();
	}

	if(JOY_DOWN(joy) && !JOY_DOWN(lastJoy) && count > 1)
	{
		--count;
		delete_sprite();
	}

	if(JOY_RIGHT(joy) && !JOY_RIGHT(lastJoy))
	{
		unsigned char i;
		for(i = 0; i < 5; i++)
		{
			++count;
			init_sprite();
		}
	}
	else if(JOY_LEFT(joy) && !JOY_LEFT(lastJoy) && count > 5)
	{
		unsigned char i;
		for(i = 0; i < 5; i++)
		{
			--count;
			delete_sprite();
		}
	}

	if(JOY_BTN_A(joy) && !JOY_BTN_A(lastJoy))
	{
		++sprctl0;
		change_sprites();
	}

	if(JOY_BTN_B(joy) && !JOY_BTN_B(lastJoy))
	{
		++refresh;
		switch(refresh%3)
		{
			case 0:
				lynx_video_setframerate(50);
				break;
			case 1:
				lynx_video_setframerate(60);
				break;
			case 2:
				lynx_video_setframerate(75);
				break;
		}
	}

	if(JOY_BTN_FLIP(joy) && !JOY_BTN_FLIP(lastJoy))
    {
		lynx_video_flip();
    }

	if(JOY_BTN_OPT1(joy) && !JOY_BTN_OPT1(lastJoy))
    {
        stats = !stats;
    }

	lastJoy = joy;
}

int main()
{
	lynx_video_init();
	lynx_video_setframerate(60);
	asm("cli");
	while (lynx_video_busy()) { }

	head = init_sprite();
	end = head;

	lynx_video_setcolor(COLOR_BLUE);

	while(true)
	{
		unsigned short joy = lynx_joy_read();

		handle_input(joy);

		if(!lynx_video_busy())
		{
			SPRITE *ptr = head;

			lynx_video_clear();

			while(true)
			{
				if(ptr->scb.hpos == SCREEN_W - SPR_W)
					ptr->hv = -1;
				else if(ptr->scb.hpos == 0)
					ptr->hv = 1;

				if(ptr->scb.vpos == SCREEN_H - SPR_H)
					ptr->vv = -1;
				else if(ptr->scb.vpos == 0)
					ptr->vv = 1;

				ptr->scb.hpos += ptr->hv;
				ptr->scb.vpos += ptr->vv;

				if(ptr->scb.next == 0)
					break;

				ptr = (SPRITE*)ptr->scb.next;
			}

			lynx_video_sprite(head);
			if(stats)
			{
				switch(refresh%3)
				{
					case 0:
						lynx_video_outtextxy(130,0,"50");
						break;
					case 1:
						lynx_video_outtextxy(130,0,"60");
						break;
					case 2:
						lynx_video_outtextxy(130,0,"75");
						break;
				}

				switch(sprctl0%8)
				{
					case 0:
						lynx_video_outtextxy(0,0,"BACKGROUND");
						break;
					case 1:
						lynx_video_outtextxy(0,0,"BACKNONCOLL");
						break;
					case 2:
						lynx_video_outtextxy(0,0,"BSHADOW");
						break;
					case 3:
						lynx_video_outtextxy(0,0,"BOUNDARY");
						break;
					case 4:
						lynx_video_outtextxy(0,0,"NORMAL");
						break;
					case 5:
						lynx_video_outtextxy(0,0,"NONCOLL");
						break;
					case 6:
						lynx_video_outtextxy(0,0,"XOR");
						break;
					case 7:
						lynx_video_outtextxy(0,0,"SHADOW");
						break;
				}
			}

			lynx_video_updatedisplay();
		}
	}
}
