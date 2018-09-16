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


RECT SpriteRect[1], dstRect[1];	//0: player, 1:beam, 2:life, 3:moster, 4:mon_die 5:player_die, 6:score, 7:heart, 8:item
int gFullScreen = 0, Click = 0, beam = 0;
int gWidth = 640 * 2, gHeight = 480;
int MouseX = 100, MouseY = gHeight / 2;


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
	if (DirectOBJ != NULL)
	{
		if (RealScreen != NULL)
		{
			RealScreen->Release();
			RealScreen = NULL;
		}
		if (Player != NULL)
		{
			Player->Release();
			Player = NULL;
		}
		if (BackGround != NULL)
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
	int Step=5;


    switch ( message )
    {
/*
        case    WM_MOUSEMOVE    :   MouseX = LOWORD(lParam);
                                    MouseY = HIWORD(lParam);
                                    break;
*/
		case	WM_LBUTTONDOWN	: 	Click=1;
									_Play( 3 );
									break;
        case	WM_KEYDOWN:            
            switch (wParam)
            {
                case VK_ESCAPE:
                case VK_F12: 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;            

				case VK_LEFT: 
					MouseX-=Step;
					return 0;

                case VK_RIGHT: 
					MouseX+=Step;
					return 0;

                case VK_UP:
					MouseY-=Step;	
					return 0;

                case VK_DOWN: 
					MouseY+=Step;
					return 0;

				case VK_SPACE:
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
		if((MainHwnd=CreateWindowEx (0, "이제환_Slave", NULL, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), 
				GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL ))==NULL)
			ExitProcess(1);
	}
	else{
		if((MainHwnd=CreateWindow("TEST", "이제환_Slave", WS_OVERLAPPEDWINDOW, 0, 0, x, 
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



void _GameProcDraw(char *recvData)
{
	RECT BackRect = { 0, 300, 640, 300 + 480 };
	RECT BackRect_S = { 0, 0, 640, 480 };
	RECT DispRect = { 0, 0, gWidth, gHeight };
	RECT WinRect;
	static int type = 10;
	sscanf(recvData, "%d %d %d %d %d %d %d %d %d %d", &type, &dstRect[0].left, &dstRect[0].top, &dstRect[0].right, &dstRect[0].bottom, &SpriteRect[0].left, &SpriteRect[0].top, &SpriteRect[0].right, &SpriteRect[0].bottom);
	sscanf(recvData, "%d", &type);

	if (type == 10) {
		BackScreen->BltFast(0, 0, BackGround3, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround4, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
	}
	else if (type == 11) {
		BackScreen->BltFast(0, 0, BackGround, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround2, &BackRect, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
	}
	else if (type == 12) {
		BackScreen->BltFast(0, 0, BackGround5, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround6, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
	}
	else if (type == 13) {
		BackScreen->BltFast(0, 0, BackGround7, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
		BackScreen->BltFast(640, 0, BackGround8, &BackRect_S, DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY);
	}

	if (type == 0)
		BackScreen->Blt(&dstRect[0], Player, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if(type == 1)
		BackScreen->Blt(&dstRect[0], Shoot, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if (type == 2)
		BackScreen->Blt(&dstRect[0], Player, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if(type ==3)
		BackScreen->Blt(&dstRect[0], Player, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if (type == 4)
		BackScreen->Blt(&dstRect[0], Pop, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if (type == 5)
		BackScreen->Blt(&dstRect[0], Player, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if (type == 6)
		BackScreen->Blt(&dstRect[0], Number, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if(type ==7)
		BackScreen->Blt(&dstRect[0], Complete, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if(type==8)
		BackScreen->Blt(&dstRect[0], Complete, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	else if (type == 9)
		BackScreen->Blt(&dstRect[0], Player, &SpriteRect[0], DDBLT_WAIT | DDBLT_KEYSRC, NULL);

	
	
	
	if (type == 15) {
		GetWindowRect(MainHwnd, &WinRect);
		RealScreen->Blt(&WinRect, BackScreen, &DispRect, DDBLT_WAIT, NULL);
		
	}
}



int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG msg;

    if ( !_GameMode(hInstance, nCmdShow, gWidth, gHeight, 32) ) return FALSE;

	Player = DDLoadBitmap(DirectOBJ, "spritesheet_players.bmp", 0, 0);
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
	Pop = DDLoadBitmap(DirectOBJ, "pop.BMP", 0, 0);
	Complete = DDLoadBitmap(DirectOBJ, "spritesheet_complete.bmp", 0, 0);

	DDSetColorKey(Player, RGB(0, 0, 0));
	DDSetColorKey(Shoot, RGB(0, 0, 0));
	DDSetColorKey(Number, RGB(0, 0, 0));
	DDSetColorKey(Pop, RGB(0, 0, 0));
	DDSetColorKey(Complete, RGB(0, 0, 0));



	CommInit(NULL, NULL);

///////////////////

    if ( _InitDirectSound() )
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
