#include "DxLib.h"

//デバッグモード
#define _DEBUGMODE

//フレームレート定数( 60 )
#define	_FRAMERATE_60	60

/*****      フォントサイズ定数      *****/
#define _FONTSIZE_SS		10	//フォントSSサイズ
#define _FONTSIZE_S			20	//フォントSサイズ
#define _FONTSIZE_M			32	//フォントMサイズ
#define _FONTSIZE_L			48	//フォントLサイズ
#define _FONTSIZE_LL		64	//フォントLLサイズ

/*****		マスピクセル数		*****/
#define _MASS_X		32
#define _MASS_Y		32

//ゲーム状態変数
static int GAMESTATE;

//リフレッシュレート確認用変数
static int RefreshRate;
HDC hdc;

/*****      ゲームモード列挙体      *****/
typedef enum GAME_MODE {
	GAME_TITLE,			//タイトル
	GAME_HELP,			//ヘルプ
	GAME_LEVEL,			//難易度
	GAME_INIT,			//初期化
	GAME_MAIN,			//メイン
	GAME_WIN,			//勝利画面
	GAME_LOSE,			//敗北者画面
	GAME_END,			//エンド処理
	GAME_INPUTRANKING,	//ランキング入り画面
	GAME_RANKING,		//ランキング画面
	END = 99			//エンド
};

/*****      画像構造体      *****/
typedef struct PICTURE {
	int Picture;
};
PICTURE Pic;	//画像構造体宣言

/*****      画像構造体      *****/
typedef struct SOUND {
	int Sound;
};
SOUND Sound;		//音楽構造体宣言

/*****      フレームレート構造体      *****/
typedef struct FRAMERATE_CONTROL {
	int mStartTime;
	int mCount;
	float mFps;
	int ReFreshRateState;
};
FRAMERATE_CONTROL FR_Control = { 0, 0, 0.0, 0 };	//フレームレート制御構造体宣言

/*****      キー操作構造体      *****/
typedef struct OPERATE {
	//KeyInput
	int OldK;
	int NowK;
	int Kflg;
} OPERATE;

OPERATE opt;

/****************************************************/
/*****											*****/
/*****			  関数プロトタイプ宣言			*****/
/*****											*****/
/****************************************************/

//フレームレート制御関数
static bool FR_Update( );
static void FR_Draw( );
static void FR_Wait( );

void DrawTitle();
void GameInit();
void GameMain();
void DrawStage();
void DrawEnd();

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {

	GAMESTATE = GAME_TITLE;
	SetMainWindowText( "Aiueo" );

	ChangeWindowMode( TRUE );
	SetGraphMode( 512, 448, 32 );
	SetDrawScreen( DX_SCREEN_BACK );

	/*****          リフレッシュレート確認            *****/
	hdc = GetDC( GetMainWindowHandle() ) ;	// デバイスコンテキストの取得
	RefreshRate = GetDeviceCaps( hdc, VREFRESH ) ;	// リフレッシュレートの取得
	ReleaseDC( GetMainWindowHandle(), hdc ) ;	// デバイスコンテキストの解放

	if ( DxLib_Init() == -1 )	return -1;

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

/******　　　　　　　フレーム数制御関数         ******/
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

/******　　　　　　　フレームレート描画         ******/
static void FR_Draw( ) {
	
	SetFontSize( _FONTSIZE_S );
	DrawFormatString( 0, 0, 0xff0000, "%.1f", FR_Control.mFps );

}

/******　　　　　　　 処理待ち関数          ******/
static void FR_Wait( ) {
	int tookTime = GetNowCount() - FR_Control.mStartTime;
	int waitTime = FR_Control.mCount * 1000 / _FRAMERATE_60 - tookTime;

	if ( waitTime > 0 ) {
		Sleep( waitTime );
	}
}

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

void GameInit() {
	GAMESTATE = GAME_MAIN;
}

void GameMain() {

	DrawFormatString( 0, 0, 0xffffff, "MAIN" );

	DrawStage();

	if ( opt.Kflg & PAD_INPUT_10 ) {
		GAMESTATE = GAME_TITLE;
	}
}

void DrawEnd() {
	GAMESTATE = END;
}

void DrawStage() {
	
	//ライン描画
	for ( int StageX = 0; StageX < 16; StageX++ ) {
		DrawLine( StageX * _MASS_X, 0, StageX * _MASS_X, 480, 0xffffff );
	}
	for ( int StageY = 0; StageY < 14 ; StageY++ ) {
		DrawLine( 0, StageY * _MASS_Y, 640, StageY * _MASS_Y, 0xffffff );
	}

}