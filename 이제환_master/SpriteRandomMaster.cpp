#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdio.h>

#include "ddutil.h"

#include <dsound.h>
#include "dsutil.h"


#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))
#define _GetKeyPush( vkey )  LOBYTE(GetAsyncKeyState( vkey ))

HWND MainHwnd;

LPDIRECTDRAW         DirectOBJ;
LPDIRECTDRAWSURFACE  RealScreen;
LPDIRECTDRAWSURFACE  BackScreen;
LPDIRECTDRAWSURFACE  Player;
LPDIRECTDRAWSURFACE  BackGround;
LPDIRECTDRAWSURFACE  BackGround2;
LPDIRECTDRAWSURFACE  BackGround3;
LPDIRECTDRAWSURFACE  BackGround4;
LPDIRECTDRAWSURFACE  BackGround5;
LPDIRECTDRAWSURFACE  BackGround6;
LPDIRECTDRAWSURFACE  BackGround7;
LPDIRECTDRAWSURFACE  BackGround8;


LPDIRECTDRAWSURFACE  Shoot;
LPDIRECTDRAWSURFACE  Number;
LPDIRECTDRAWSURFACE  Complete;

LPDIRECTDRAWSURFACE  Pop;

LPDIRECTDRAWCLIPPER	ClipScreen;

bool BIG = 0;
RECT SpriteRect[10], dstRect[10];	//0: player, 1:beam, 2:life, 3:moster, 4:mon_die 5:player_die, 6:score, 7:heart, 8:item, 9:big_p
int gFullScreen=0, Click=0, beam =0;
int gWidth=640*2, gHeight=480;
int MouseX=100, MouseY=gHeight/2;
int GAME_MODE = 0;		//0:start, 1:play, 2:end, 3:ending
int rand_pos[1000];
int speed[1000];
int speed_y[1000];
int heart[100];
int coin[100];
int sp[1000];

int life = 5;
int score = 0;
int p_size = 0;
int third = 0;
////////////////////

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

HSNDOBJ Sound[10];


BOOL _InitDirectSound( void )
{
    if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
    {
        if (SoundOBJ->SetCooperativeLevel(MainHwnd,DSSCL_PRIORITY)!=DS_OK) return FALSE;

        memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
        DSB_desc.dwSize = sizeof(DSBUFFERDESC);
        DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;

        if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return FALSE;
        SoundDSB -> SetVolume(DSBVOLUME_MAX); // DSBVOLUME_MIN
        SoundDSB -> SetPan(DSBPAN_RIGHT);
        return TRUE;
    }
    return FALSE;
}

void _Play( int num )
{
    SndObjPlay( Sound[num], NULL );
}

////////////////////////


BOOL Fail( HWND hwnd )
{
    ShowWindow( hwnd, SW_HIDE );
    MessageBox( hwnd, "DIRECT X 초기화에 실패했습니다.", "게임 디자인", MB_OK );
    DestroyWindow( hwnd );
    return FALSE;
}

void _ReleaseAll( void )
{
    if ( DirectOBJ != NULL )
    {
        if ( RealScreen != NULL )
        {
            RealScreen->Release();
            RealScreen = NULL;
        }
        if ( Player != NULL )
        {
            Player->Release();
            Player = NULL;
        }
        if ( BackGround != NULL )
        {
            BackGround->Release();
            BackGround = NULL;
        }
        DirectOBJ->Release();
        DirectOBJ = NULL;
    }
}

long FAR PASCAL WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int Step=10;


    switch ( message )
    {
        case	WM_KEYDOWN:    
            switch (wParam)
            {
				case	'x':
				case	'X':
					GAME_MODE = 1;
					return 0;

                case VK_ESCAPE:
                case VK_F12: 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;            

				case VK_LEFT: 
					Click = 1;
					MouseX-=Step;
					return 0;

                case VK_RIGHT: 
					Click = 1;
					MouseX+=Step;
					return 0;

                case VK_UP:
					MouseY-=Step;	
					return 0;

                case VK_DOWN: 
					MouseY+=Step;
					return 0;

				case VK_SPACE:
					beam = 1;
					Click=1;
					_Play( 3 );
					break;
			}
            break;

        case    WM_DESTROY      :  _ReleaseAll();
                                    PostQuitMessage( 0 );
                                    break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}

BOOL _GameMode( HINSTANCE hInstance, int nCmdShow, int x, int y, int bpp )
{
    HRESULT result;
    WNDCLASS wc;
    DDSURFACEDESC ddsd;
    DDSCAPS ddscaps;
    LPDIRECTDRAW pdd;

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TEST";
    RegisterClass( &wc );


	if(gFullScreen){
		if((MainHwnd=CreateWindowEx (0, "이제환_master", NULL, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), 
				GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL ))==NULL)
			ExitProcess(1);
	}
	else{
		if((MainHwnd=CreateWindow("TEST", "이제환_master", WS_OVERLAPPEDWINDOW, 0, 0, x, 
									y, NULL, NULL, hInstance, NULL))==NULL)
			ExitProcess(1);
		SetWindowPos(MainHwnd, NULL, 100, 100, x, y, SWP_NOZORDER);
	}

    SetFocus( MainHwnd );
    ShowWindow( MainHwnd, nCmdShow );
    UpdateWindow( MainHwnd );
//    ShowCursor( FALSE );

    result = DirectDrawCreate( NULL, &pdd, NULL );
    if ( result != DD_OK ) return Fail( MainHwnd );

    result = pdd->QueryInterface(IID_IDirectDraw, (LPVOID *) &DirectOBJ);
    if ( result != DD_OK ) return Fail( MainHwnd );


	// 윈도우 핸들의 협력 단계를 설정한다.
	if(gFullScreen){
	    result = DirectOBJ->SetCooperativeLevel( MainHwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
		if ( result != DD_OK ) return Fail( MainHwnd );

		result = DirectOBJ->SetDisplayMode( x, y, bpp);
		if ( result != DD_OK ) return Fail( MainHwnd );

		memset( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;

	    result = DirectOBJ -> CreateSurface( &ddsd, &RealScreen, NULL );
	   if ( result != DD_OK ) return Fail( MainHwnd );

		memset( &ddscaps, 0, sizeof(ddscaps) );
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		result = RealScreen -> GetAttachedSurface( &ddscaps, &BackScreen );
		if ( result != DD_OK ) return Fail( MainHwnd );
	}
	else{
	    result = DirectOBJ->SetCooperativeLevel( MainHwnd, DDSCL_NORMAL );
		if ( result != DD_OK ) return Fail( MainHwnd );

		memset( &ddsd, 0, sizeof(ddsd) );
	    ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddsd.dwBackBufferCount = 0;

		result = DirectOBJ -> CreateSurface( &ddsd, &RealScreen, NULL );
	    if(result != DD_OK) return Fail(MainHwnd);

		memset( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof(ddsd);
	    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = x; 
		ddsd.dwHeight = y;
		result = DirectOBJ->CreateSurface( &ddsd, &BackScreen, NULL );
		if ( result != DD_OK ) return Fail( MainHwnd );

		result = DirectOBJ->CreateClipper( 0, &ClipScreen, NULL);
		if ( result != DD_OK ) return Fail( MainHwnd );

		result = ClipScreen->SetHWnd( 0, MainHwnd );
		if ( result != DD_OK ) return Fail( MainHwnd );

		result = RealScreen->SetClipper( ClipScreen );
		if ( result != DD_OK ) return Fail( MainHwnd );

		SetWindowPos(MainHwnd, NULL, 100, 100, x, y, SWP_NOZORDER | SWP_NOACTIVATE); 
	}


    return TRUE;
}


extern void CommInit(int argc, char **argv);
extern void CommSend(char *sending);
extern void CommRecv(char *recvData);
void load_random() {
	srand(10);
	for (int i = 0; i < 500; i++) {
		rand_pos[i] = rand();
		speed[i] = rand() % 2 + 1;
		
	}
	srand(11);

	for (int i = 0; i < 200; i++) {
		sp[i] = rand() % 5 - 2;
	}
	for (int i = 0; i < 20; i++)
		coin[i] = (rand() % (gWidth - 30)) * 10 + 100;

	srand(20);
	for (int i = 0; i < 20; i++)
		heart[i] = (rand() % (gWidth - 30)) * 15;

}

void dynamicSprite() {
	static int Frame = 0;
	//캐릭터
	SpriteRect[0].left = 520;
	SpriteRect[0].top = 1548 - Frame * 256;
	SpriteRect[0].right = SpriteRect[0].left + 128;
	SpriteRect[0].bottom = SpriteRect[0].top + 256;
	//캐릭터
	dstRect[0].left = MouseX;
	dstRect[0].top = MouseY;
	dstRect[0].right = dstRect[0].left + 70 + p_size;
	dstRect[0].bottom = dstRect[0].top + 140 + 2 * p_size;
	//walk animation
	if (Click) {
		if (Frame == 1)
			Frame = 0;
		else
			Frame = 1;
		Click = 0;
	}
	SpriteRect[9].left = 256;
	SpriteRect[9].top = 1792 - Frame * 256;
	SpriteRect[9].right = SpriteRect[9].left + 128;
	SpriteRect[9].bottom = SpriteRect[9].top + 256;
	//beam shoot
	SpriteRect[1].left = 0;
	SpriteRect[1].top = 0;
	SpriteRect[1].right = SpriteRect[1].left + 371;
	SpriteRect[1].bottom = SpriteRect[1].top + 53;
	//beam shoot
	dstRect[1].left = dstRect[0].right - 20;
	dstRect[1].top = (dstRect[0].top + dstRect[0].bottom) / 2;
	dstRect[1].right = dstRect[1].left + 500;
	dstRect[1].bottom = dstRect[1].top + 50 + p_size / 2;
	if (dstRect[1].right > gWidth - 20)
		dstRect[1].right = gWidth - 20;
}

void setSprite() {
	//pop
	SpriteRect[4].left = 360;
	SpriteRect[4].top = 425;
	SpriteRect[4].right = 390;
	SpriteRect[4].bottom = 455;
	//die
	SpriteRect[5].left = 640;
	SpriteRect[5].top = 256;
	SpriteRect[5].right = SpriteRect[5].left + 128;
	SpriteRect[5].bottom = SpriteRect[5].top + 256;
	//heart
	SpriteRect[7].left = 2860;
	SpriteRect[7].top = 1300;
	SpriteRect[7].right = SpriteRect[7].left + 128;
	SpriteRect[7].bottom = SpriteRect[7].top + 128;
	//coin
	SpriteRect[8].left = 2730;
	SpriteRect[8].top = 650;
	SpriteRect[8].right = SpriteRect[8].left + 128;
	SpriteRect[8].bottom = SpriteRect[8].top + 128;
}

void CALLBACK _GameProc(HWND hWnd, UINT message, UINT wParam, DWORD lParam)
{
	RECT BackRect = { 0, 300, 640, 300 + 480 };
	RECT BackRect_S = { 0, 0, 640, 480 };
	RECT DispRect = { 0, 0, gWidth, gHeight };
	RECT WinRect;
	dynamicSprite();
	char sendData[200];
	sprintf(sendData, "%d", GAME_MODE + 10);
	CommSend(sendData);
	if (BIG) {
		SndObjStop(Sound[0]);
		_Play(6);
	}
	else {
		SndObjStop(Sound[6]);
		SndObjPlay(Sound[0], DSBPLAY_LOOPING);
	}

	if (GAME_MODE==0) {
		BackScreen->BltFast(0, 0, BackGround3, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround4, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
	}
	else if (GAME_MODE == 1) {
		BackScreen->BltFast(0, 0, BackGround, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround2, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);

		//커짐
		static int big_timer = 0;
		if (BIG && p_size != 80) {
			p_size += 4;
			MouseY -= 8;
		}
		if (p_size == 80) {
			big_timer++;
		}
		if (big_timer == 300) {
			p_size = 0;
			big_timer = 0;
			BIG = 0;
		}
		//player
		
		if (!BIG) {
			BackScreen->Blt(&dstRect[0], Player, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 0, dstRect[0].left, dstRect[0].top, dstRect[0].right, dstRect[0].bottom, SpriteRect[0].left, SpriteRect[0].top, SpriteRect[0].right, SpriteRect[0].bottom);
			CommSend(sendData);
		}
		else {
			BackScreen->Blt(&dstRect[0], Player, &SpriteRect[9], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d %d %d %d %d %d %d %d %d",9, dstRect[0].left, dstRect[0].top, dstRect[0].right, dstRect[0].bottom, SpriteRect[9].left, SpriteRect[9].top, SpriteRect[9].right, SpriteRect[9].bottom);
			CommSend(sendData);
		}

		if (MouseX <= 35)
			MouseX = 35;
		if (MouseX > gWidth - 35)
			MouseX = gWidth - 35;
		if (MouseY <= 3)
			MouseY = 3;
		if (MouseY > gHeight - 145 + 2 * p_size)
			MouseY = gHeight - 145 + 2 * p_size;

		
		//beam shoot 
		static int beam_timer = 0;
		if (beam == 1) {
			BackScreen->Blt(&dstRect[1], Shoot, &SpriteRect[1], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 1, dstRect[1].left, dstRect[1].top, dstRect[1].right, dstRect[1].bottom, SpriteRect[1].left, SpriteRect[1].top, SpriteRect[1].right, SpriteRect[1].bottom);
			CommSend(sendData);
			beam_timer++;
		}
		if (beam_timer == 15) {
			beam = 0;
			beam_timer = 0;
			Click = 0;
		}
		if (life <= 0)
			GAME_MODE = 2;
		//life
		for (int i = 0; i < life; i++) {
			SpriteRect[2].left = 640;
			SpriteRect[2].top = 0;
			SpriteRect[2].right = SpriteRect[2].left + 128;
			SpriteRect[2].bottom = SpriteRect[2].top + 256;
			dstRect[2].left = 20 + i * 30;
			dstRect[2].top = 20;
			dstRect[2].right = dstRect[2].left + 30;
			dstRect[2].bottom = dstRect[2].top + 60;

			BackScreen->Blt(&dstRect[2], Player, &SpriteRect[2], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 2, dstRect[2].left, dstRect[2].top, dstRect[2].right, dstRect[2].bottom, SpriteRect[2].left, SpriteRect[2].top, SpriteRect[2].right, SpriteRect[2].bottom);
			CommSend(sendData);
		}

		static int SrcX = 0, SrcY = 0, Collision = 0;
		int posx, posy, size, coll = 0;

		int speed_x, speed_y;

		/////////////////////외계인 수/////////////////////////////////////
		for (int i = 0; i < 200; i++) {
			if (rand_pos[i] == 0)
				continue;
			speed_x = speed[i] * SrcX;
			speed_y = sp[i] * SrcY;
			posx = (speed_x + rand_pos[i]) % (gWidth - 30) * 10 + 30;
			posy = (speed_y + rand_pos[i]) % (gHeight - 30) + 15;
			if (posy < 0)
				posy = 480;
			if (posy > 480)
				posy = 5;
			if (posx < 0)
				rand_pos[i] = 0;

			size = (rand_pos[i] % 20) + 30;
			dstRect[3].left = posx;
			dstRect[3].top = posy;
			dstRect[3].right = dstRect[3].left + size;
			dstRect[3].bottom = dstRect[3].top + size * 2;

			//플레이어가 외계인과 부디침
			if (abs(MouseX + 35 + p_size / 2 - (posx + size / 2)) < (35 + p_size / 2 + size / 2) && abs(MouseY + 105 + p_size - (posy + size + size / 2)) < (35 + p_size + size / 2)) {
				coll = 1;
				BackScreen->Blt(&dstRect[3], Pop, &SpriteRect[4], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
				sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 4, dstRect[3].left, dstRect[3].top, dstRect[3].right, dstRect[3].bottom, SpriteRect[4].left, SpriteRect[4].top, SpriteRect[4].right, SpriteRect[4].bottom);
				CommSend(sendData);
				rand_pos[i] = 0;
			}
			else {
				int frame1 = (rand_pos[i] % 7) * 256;
				int frame2 = (rand_pos[i] % 7) * 128;
				SpriteRect[3].left = frame2;
				SpriteRect[3].top = frame1;
				SpriteRect[3].right = SpriteRect[3].left + 128;
				SpriteRect[3].bottom = SpriteRect[3].top + 256;
				//외계인출력
				if (posx<gWidth+10 && rand_pos[i] != 0) {
					BackScreen->Blt(&dstRect[3], Player, &SpriteRect[3], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
					sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 3, dstRect[3].left, dstRect[3].top, dstRect[3].right, dstRect[3].bottom, SpriteRect[3].left, SpriteRect[3].top, SpriteRect[3].right, SpriteRect[3].bottom);
					CommSend(sendData);

				}
			}
			//빔맞고 외계인 죽음
			if (beam == 1 && abs(MouseX + 275 - (posx + size / 2)) < (275 + size / 2) && abs(dstRect[1].top + 25 + p_size / 4 - (posy + size + size / 2)) < (p_size / 4 + 50 + size / 2)) {
				score++;
				BackScreen->Blt(&dstRect[3], Pop, &SpriteRect[4], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
				sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 4, dstRect[3].left, dstRect[3].top, dstRect[3].right, dstRect[3].bottom, SpriteRect[4].left, SpriteRect[4].top, SpriteRect[4].right, SpriteRect[4].bottom);
				CommSend(sendData);

				rand_pos[i] = 0;
			}
		}

		if (coll) {
			if (!Collision) {
				Collision = 1;
				if (BIG) {
					_Play(7);
					score++;
				}
				else {
					_Play(4);
					life--;
				}
			}
		}
		else
			Collision = 0;

		SrcY--;
		SrcX--;

		//heart
		static int Collision2 = 0;
		int coll2 = 0;
		for (int i = 0; i < 10; i++) {
			dstRect[7].left = heart[i] + SrcX * 5;
			dstRect[7].top = heart[i] % (gHeight - 30) + 15;
			dstRect[7].right = dstRect[7].left + 50;
			dstRect[7].bottom = dstRect[7].top + 50;
			BackScreen->Blt(&dstRect[7], Complete, &SpriteRect[7], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 7, dstRect[7].left, dstRect[7].top, dstRect[7].right, dstRect[7].bottom, SpriteRect[7].left, SpriteRect[7].top, SpriteRect[7].right, SpriteRect[7].bottom);
			CommSend(sendData);
			if (abs(MouseX + 35 + p_size / 2 - (dstRect[7].left + 25)) < p_size / 2 + 35 + 25 && abs(MouseY + p_size + 105 - (dstRect[7].top + 25)) < p_size + 35 + 25) {
				heart[i] = 0;
				coll2 = 1;
			}
		}

		if (coll2) {
			if (!Collision2) {
				Collision2 = 1;
				life++;
				_Play(1);

			}
		}
		else
			Collision2 = 0;

		//coin
		static int Collision3 = 0;
		int coll3 = 0;
		for (int i = 0; i < 10; i++) {
			dstRect[8].left = coin[2*i] + SrcX * 5;
			dstRect[8].top = coin[2*i] % (gHeight - 30) + 15;
			dstRect[8].right = dstRect[8].left + 50;
			dstRect[8].bottom = dstRect[8].top + 50;
			BackScreen->Blt(&dstRect[8], Complete, &SpriteRect[8], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 8, dstRect[8].left, dstRect[8].top, dstRect[8].right, dstRect[8].bottom, SpriteRect[8].left, SpriteRect[8].top, SpriteRect[8].right, SpriteRect[8].bottom);
			CommSend(sendData);
			if (abs(MouseX + 35 + p_size / 2 - (dstRect[8].left + 25)) < p_size / 2 + 35 + 25 && abs(MouseY + p_size / 2 + 105 - (dstRect[8].top + 25)) < p_size + 35 + 25) {
				coin[2*i] = 0;
				coll3 = 1;
			}
		}

		if (coll3) {
			if (!Collision3) {
				Collision3 = 1;
				BIG = 1;
				_Play(1);

			}
		}
		else
			Collision3 = 0;


		SpriteRect[6].left = 0;
		SpriteRect[6].top = 1152;
		SpriteRect[6].right = SpriteRect[6].left + 128;
		SpriteRect[6].bottom = SpriteRect[6].top + 128;
		
		for (int i = 0; i < 2; i++) {
			dstRect[6].left = gWidth - 40 - i * 15;
			dstRect[6].top = 20;
			dstRect[6].right = dstRect[6].left + 30;
			dstRect[6].bottom = dstRect[6].top + 60;

			BackScreen->Blt(&dstRect[6], Number, &SpriteRect[6], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 6, dstRect[6].left, dstRect[6].top, dstRect[6].right, dstRect[6].bottom, SpriteRect[6].left, SpriteRect[6].top, SpriteRect[6].right, SpriteRect[6].bottom);
			CommSend(sendData);
		}
		dstRect[6].left = gWidth - 40 - 2 * 15;
		dstRect[6].right = dstRect[6].left + 30;
		SpriteRect[6].left = 0;
		SpriteRect[6].top = 1152 - score * 128;
		SpriteRect[6].right = SpriteRect[6].left + 128;
		SpriteRect[6].bottom = SpriteRect[6].top + 128;
		BackScreen->Blt(&dstRect[6], Number, &SpriteRect[6], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
		sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 6, dstRect[6].left, dstRect[6].top, dstRect[6].right, dstRect[6].bottom, SpriteRect[6].left, SpriteRect[6].top, SpriteRect[6].right, SpriteRect[6].bottom);
		CommSend(sendData);
		if (score >= 10) {
			third++;
			score -= 10;
		}
		dstRect[6].left = gWidth - 40 - 3 * 15;
		dstRect[6].right = dstRect[6].left + 30;
		SpriteRect[6].left = 0;
		SpriteRect[6].top = 1152 - third * 128;
		SpriteRect[6].right = SpriteRect[6].left + 128;
		SpriteRect[6].bottom = SpriteRect[6].top + 128;
		BackScreen->Blt(&dstRect[6], Number, &SpriteRect[6], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
		sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 6, dstRect[6].left, dstRect[6].top, dstRect[6].right, dstRect[6].bottom, SpriteRect[6].left, SpriteRect[6].top, SpriteRect[6].right, SpriteRect[6].bottom);
		CommSend(sendData);


		if (third > 9)
			GAME_MODE = 3;

	}
	else if (GAME_MODE == 2) {
		SndObjStop(Sound[6]);
		SndObjStop(Sound[0]);
		_Play(2);

		BackScreen->BltFast(0, 0, BackGround5, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround6, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);

		BackScreen->Blt(&dstRect[0], Player, &SpriteRect[5], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
		sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 5, dstRect[0].left, dstRect[0].top, dstRect[0].right, dstRect[0].bottom, SpriteRect[5].left, SpriteRect[5].top, SpriteRect[5].right, SpriteRect[5].bottom);
		CommSend(sendData);
		static int die = 0;
		if (life <= 0 && die != 50) {
			MouseY += 5;
			die++;
		}
		if (MouseY > gHeight - 145 + 2 * p_size)
			MouseY = gHeight - 145 + 2 * p_size;

		SpriteRect[6].left = 0;
		SpriteRect[6].top = 1152;
		SpriteRect[6].right = SpriteRect[6].left + 128;
		SpriteRect[6].bottom = SpriteRect[6].top + 128;
		for (int i = 0; i < 2; i++) {
			dstRect[6].left = 680 - i * 60;
			dstRect[6].top = 360;
			dstRect[6].right = dstRect[6].left + 120;
			dstRect[6].bottom = dstRect[6].top + 120;
			BackScreen->Blt(&dstRect[6], Number, &SpriteRect[6], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 6, dstRect[6].left, dstRect[6].top, dstRect[6].right, dstRect[6].bottom, SpriteRect[6].left, SpriteRect[6].top, SpriteRect[6].right, SpriteRect[6].bottom);
			CommSend(sendData);
		}
		dstRect[6].left = 680 - 2 * 60;
		dstRect[6].right = dstRect[6].left + 120;
		SpriteRect[6].left = 0;
		SpriteRect[6].top = 1152 - score * 128;
		SpriteRect[6].right = SpriteRect[6].left + 128;
		SpriteRect[6].bottom = SpriteRect[6].top + 128;
		BackScreen->Blt(&dstRect[6], Number, &SpriteRect[6], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
		sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 6, dstRect[6].left, dstRect[6].top, dstRect[6].right, dstRect[6].bottom, SpriteRect[6].left, SpriteRect[6].top, SpriteRect[6].right, SpriteRect[6].bottom);
		CommSend(sendData);
		if (score >= 10) {
			third++;
			score -= 10;
		}
		dstRect[6].left = 680 - 3 * 60;
		dstRect[6].right = dstRect[6].left + 120;
		SpriteRect[6].left = 0;
		SpriteRect[6].top = 1152 - third * 128;
		SpriteRect[6].right = SpriteRect[6].left + 128;
		SpriteRect[6].bottom = SpriteRect[6].top + 128;
		BackScreen->Blt(&dstRect[6], Number, &SpriteRect[6], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
		sprintf(sendData, "%d %d %d %d %d %d %d %d %d", 6, dstRect[6].left, dstRect[6].top, dstRect[6].right, dstRect[6].bottom, SpriteRect[6].left, SpriteRect[6].top, SpriteRect[6].right, SpriteRect[6].bottom);
		CommSend(sendData);


	}
	else if (GAME_MODE == 3) {
		SndObjStop(Sound[6]);
		SndObjStop(Sound[0]);
		_Play(5);

		BackScreen->BltFast(0, 0, BackGround7, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround8, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
	}
	



	if(gFullScreen)
		RealScreen->Flip(NULL, DDFLIP_WAIT);
	else{
		sprintf(sendData, "%d", 15);
		CommSend(sendData);
		GetWindowRect(MainHwnd, &WinRect);
		RealScreen->Blt(&WinRect, BackScreen, &DispRect, DDBLT_WAIT, NULL ); 
	}
}




int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG msg;
    if ( !_GameMode(hInstance, nCmdShow, gWidth, gHeight, 32) ) return FALSE;

    Player = DDLoadBitmap( DirectOBJ, "spritesheet_players.bmp", 0, 0 );
	BackGround = DDLoadBitmap(DirectOBJ, "back_L.bmp", 0, 0);
	BackGround2 = DDLoadBitmap(DirectOBJ, "back_R.bmp", 0, 0);
	BackGround3 = DDLoadBitmap(DirectOBJ, "start_L.bmp", 0, 0);
	BackGround4 = DDLoadBitmap(DirectOBJ, "start_R.bmp", 0, 0);
	BackGround5 = DDLoadBitmap(DirectOBJ, "gameover1.bmp", 0, 0);
	BackGround6 = DDLoadBitmap(DirectOBJ, "gameover2.bmp", 0, 0);
	BackGround7 = DDLoadBitmap(DirectOBJ, "ending1.bmp", 0, 0);
	BackGround8 = DDLoadBitmap(DirectOBJ, "ending2.bmp", 0, 0);


	Shoot = DDLoadBitmap(DirectOBJ, "shoot.bmp", 0, 0);
	Number = DDLoadBitmap(DirectOBJ, "number.bmp", 0, 0);
	Pop  = DDLoadBitmap( DirectOBJ, "pop.BMP", 0, 0 );
	Complete = DDLoadBitmap(DirectOBJ, "spritesheet_complete.bmp", 0, 0);


	DDSetColorKey( Player, RGB(0,0,0) );
	DDSetColorKey( Shoot, RGB(0, 0, 0));
	DDSetColorKey( Number, RGB(0, 0, 0));
	DDSetColorKey( Pop, RGB(0,0,0) );
	DDSetColorKey( Complete, RGB(0, 0, 0));
	load_random();
	setSprite();



	SetTimer(MainHwnd, 1, 30, _GameProc);

	CommInit(NULL, NULL);

///////////////////

	if (_InitDirectSound())
	{
		//0번: 배경음, 1번: 아이템획득, 2번: game over , 3번: 레이저, 4번: 부디칠때, 5번: ending, 6번: 커질떄, 7번: 외계인맞을때
		Sound[0] = SndObjCreate(SoundOBJ, "POL-chubby-cat-short.WAV", 1);
		Sound[1] = SndObjCreate(SoundOBJ, "Item2A.WAV", 2);
		Sound[2] = SndObjCreate(SoundOBJ, "No Hope.WAV", 1);
		Sound[3] = SndObjCreate(SoundOBJ, "Photon gun shot.WAV", 2);
		Sound[4] = SndObjCreate(SoundOBJ, "DM-CGS-17.WAV", 2);
		Sound[5] = SndObjCreate(SoundOBJ, "Casual-game-track.wav", 1);
		Sound[6] = SndObjCreate(SoundOBJ, "Riverside Ride Long Loop.WAV", 1);
		Sound[7] = SndObjCreate(SoundOBJ, "DM-CGS-07.WAV", 2);
	}
   
//////////////////

    while ( !_GetKeyState(VK_ESCAPE) )
    {

        if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
        {
            if ( !GetMessage(&msg, NULL, 0, 0) ) return msg.wParam;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
//        else _GameProc();
    }
    DestroyWindow( MainHwnd );

    return TRUE;
}
