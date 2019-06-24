#include "DxLib.h"

//�f�o�b�O���[�h
#define _DEBUGMODE

//�E�B���h�E���[�h�ύX
//(��{�̓E�B���h�E���[�h)
//0���S��ʃ��[�h�A1���E�B���h�E���[�h
#define _WINDOWMODE	1

//�t���[�����[�g�萔( 60 )
#define	_FRAMERATE_60	60

/*****      �t�H���g�T�C�Y�萔      *****/
#define _FONTSIZE_SS		10	//�t�H���gSS�T�C�Y
#define _FONTSIZE_S			20	//�t�H���gS�T�C�Y
#define _FONTSIZE_M			32	//�t�H���gM�T�C�Y
#define _FONTSIZE_L			48	//�t�H���gL�T�C�Y
#define _FONTSIZE_LL		64	//�t�H���gLL�T�C�Y

/*****		�}�X�s�N�Z����		*****/
#define _MASS_X		32
#define _MASS_Y		32
#define _MASS_HALF	16

/*****		�}�b�v�̍���		*****/
#define _MAP_X		16
#define _MAP_Y		14

//�Q�[����ԕϐ�
static int GAMESTATE;

//���t���b�V�����[�g�m�F�p�ϐ�
static int RefreshRate;
HDC hdc;

/*****      �Q�[�����[�h�񋓑�      *****/
typedef enum GAME_MODE {
	GAME_TITLE,			//�^�C�g��
	GAME_HELP,			//�w���v
	GAME_LEVEL,			//��Փx
	GAME_INIT,			//������
	GAME_MAIN,			//���C��
	GAME_WIN,			//�������
	GAME_LOSE,			//�s�k�҉��
	GAME_END,			//�G���h����
	GAME_INPUTRANKING,	//�����L���O������
	GAME_RANKING,		//�����L���O���
	END = 99			//�G���h
};

/*****      �摜�\����      *****/
typedef struct PICTURE {
	int Player[ 15 ];
	int StageBlock[ 10 ];
};
PICTURE Pic;	//�摜�\���̐錾

/*****      �T�E���h�\����      *****/
typedef struct SOUND {
	int Sound;
};
SOUND Sound;		//���y�\���̐錾

/*****      �t���[�����[�g�\����      *****/
typedef struct FRAMERATE_CONTROL {
	int mStartTime;
	int mCount;
	float mFps;
	int ReFreshRateState;
};
FRAMERATE_CONTROL FR_Control = { 0, 0, 0.0, 0 };	//�t���[�����[�g����\���̐錾

/*****      �L�[����\����      *****/
typedef struct OPERATE {
	//KeyInput
	int OldK;
	int NowK;
	int Kflg;
} OPERATE;

OPERATE opt;

/*****		  �}�b�v�\����		*****/
typedef struct MAP {
	int DrawFlg;
	int BreakFlg;
};
MAP map;

/*****		�v���C���[�\����		*****/
typedef struct PLAYER {
	int PlayerX;
	int PlayerY;
	float PSpeed;
};
PLAYER Player = { ( ( 2 * _MASS_X ) + _MASS_HALF ), ( 11 * _MASS_Y + _MASS_HALF ), 0 };

int Map[ _MAP_Y ][ _MAP_X ] = 
	{	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  6,  2,  6,  2,  6,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0 },
		{  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0 }		};

/****************************************************/
/*****											*****/
/*****			  �֐��v���g�^�C�v�錾			*****/
/*****											*****/
/****************************************************/

//�t���[�����[�g����֐�
static bool FR_Update( );
static void FR_Draw( );
static void FR_Wait( );

void DrawTitle();
void GameInit();
void GameMain();
void DrawEnd();

void DrawStage();	//�X�e�[�W�`��
void DrawPlayer();	//�v���C���[�`��

int LoadImages();

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {

	GAMESTATE = GAME_TITLE;
	SetMainWindowText( "Aiueo" );

	ChangeWindowMode( _WINDOWMODE );
	SetGraphMode( 512, 448, 32 );
	SetDrawScreen( DX_SCREEN_BACK );

	/*****          ���t���b�V�����[�g�m�F            *****/
	hdc = GetDC( GetMainWindowHandle() ) ;	// �f�o�C�X�R���e�L�X�g�̎擾
	RefreshRate = GetDeviceCaps( hdc, VREFRESH ) ;	// ���t���b�V�����[�g�̎擾
	ReleaseDC( GetMainWindowHandle(), hdc ) ;	// �f�o�C�X�R���e�L�X�g�̉��

	if ( DxLib_Init() == -1 )	return -1;
	if ( LoadImages() == -1 )	return -1;

	while ( ProcessMessage() == 0 && ClearDrawScreen() == 0 && GAMESTATE != 99 ) {
	
		opt.OldK = opt.NowK;
		opt.NowK = GetJoypadInputState( DX_INPUT_KEY_PAD1 );
		opt.Kflg = opt.NowK & ~opt.OldK;

		switch( GAMESTATE ) {

			case GAME_TITLE:
				DrawTitle();
				break;
			case GAME_INIT:
				GameInit();
				break;
			case GAME_MAIN:
				GameMain();
				break;
			case GAME_END:
				DrawEnd();
				break;
		}
	
		FR_Update();
		#ifdef _DEBUGMODE
			FR_Draw();
		#endif
		ScreenFlip();
		FR_Wait();

	}
	
	DxLib_End();
	return 0;

}

/******�@�@�@�@�@�@�@�t���[��������֐�         ******/
static bool FR_Update( ) {
	if ( FR_Control.mCount == 0 ) {
		FR_Control.mStartTime = GetNowCount();
	}
	if ( FR_Control.mCount == RefreshRate ) {
		int t = GetNowCount();
		FR_Control.mFps = 1000.f / ( ( t - FR_Control.mStartTime ) / ( float )RefreshRate );
		FR_Control.mCount = 0;
		FR_Control.mStartTime = t;
	}
	FR_Control.mCount++;
	return true;
}

/******�@�@�@�@�@�@�@�t���[�����[�g�`��         ******/
static void FR_Draw( ) {
	
	SetFontSize( _FONTSIZE_S );
	DrawFormatString( 0, 0, 0xff0000, "%.1f", FR_Control.mFps );
	DrawFormatString( 0, 20, 0xff0000, "%d", RefreshRate );

}

/******�@�@�@�@�@�@�@ �����҂��֐�          ******/
static void FR_Wait( ) {
	int tookTime = GetNowCount() - FR_Control.mStartTime;
	int waitTime = FR_Control.mCount * 1000 / _FRAMERATE_60 - tookTime;

	if ( waitTime > 0 ) {
		Sleep( waitTime );
	}
}

//�^�C�g���`��
void DrawTitle() {
	int x;

	x = 320 - GetDrawStringWidth( "MARIO", 5 );
	DrawFormatString( x, 240, 0xff0000, "MARIO" );

	x = 320 - GetDrawStringWidth( "Push Space", 10 );
	DrawFormatString( x, 400, 0xffffff, "Push Space" );

	if ( opt.Kflg & PAD_INPUT_10 ) {
		GAMESTATE = GAME_INIT;
	} else if ( opt.Kflg & PAD_INPUT_START ) {
		GAMESTATE = GAME_END;
	}
}

//�G���h���
void DrawEnd() {
	GAMESTATE = END;
}

void GameInit() {

	GAMESTATE = GAME_MAIN;

}

void GameMain() {

	DrawFormatString( 0, 0, 0xffffff, "MAIN" );

	DrawStage();		//�X�e�[�W�`��
	DrawPlayer();		//�v���C���[�`��

	if ( opt.Kflg & PAD_INPUT_10 ) {
		GAMESTATE = GAME_TITLE;
	}
}

//�X�e�[�W�`��
void DrawStage() {

	//�w�i�`��
	DrawBox( 0, 0, 512, 448, 0x5080f8, TRUE );
	
	//���C���`��
	for ( int StageX = 0; StageX < _MAP_X; StageX++ ) {
		DrawLine( StageX * _MASS_X, 0, StageX * _MASS_X, 480, 0xffffff );
	}
	for ( int StageY = 0; StageY < _MAP_Y ; StageY++ ) {
		DrawLine( 0, StageY * _MASS_Y, 640, StageY * _MASS_Y, 0xffffff );
	}

	for ( int StageY = 0; StageY < _MAP_Y; StageY++ ) {
		for ( int StageX = 0; StageX < _MAP_X; StageX++ ) {
			DrawRotaGraph( ( ( StageX * _MASS_X ) + _MASS_HALF ), ( ( StageY * _MASS_Y ) + _MASS_HALF ), 1.0f, 0, Pic.StageBlock[ Map[ StageY ][ StageX ] ], TRUE );
		}
	}

}

void DrawPlayer() {

	if ( Player.PlayerX <= ( 14 * _MASS_X + _MASS_HALF ) && opt.NowK & PAD_INPUT_RIGHT ) {
		Player.PlayerX += ( 1 + Player.PSpeed );
	} else if ( Player.PlayerX >= ( 2 * _MASS_X + _MASS_HALF ) && opt.NowK & PAD_INPUT_LEFT ) {
		Player.PlayerX -= ( 1 + Player.PSpeed );
	}

	if ( opt.OldK != 0 ) {
		if ( Player.PSpeed <= 2.0f ) {
			Player.PSpeed += 0.1f;
		}
	} 
	if ( opt.OldK == 0 && Player.PSpeed >= 0.0f ) {
		if ( Player.PSpeed > 0.0f ) {
			Player.PSpeed -= 0.03f;
		}
	}

	DrawFormatString( 0, 50, 0xff0000, "%d", opt.OldK );
	DrawFormatString( 0, 80, 0xff0000, "%d", opt.NowK );

	DrawRotaGraph( Player.PlayerX, Player.PlayerY, 1.0f, 0, Pic.Player[ 0 ], TRUE );
}

int LoadImages() {

	if ( LoadDivGraph( "images/Block.png", 9, 9, 1, 32, 32, Pic.StageBlock + 1 ) == -1 )	return -1;
	if ( LoadDivGraph( "images/mario_chara.png", 15, 5, 3, 32, 32, Pic.Player ) == -1 )	return -1;

	return TRUE;
}