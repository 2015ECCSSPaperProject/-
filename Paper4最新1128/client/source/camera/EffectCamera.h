#pragma once

//*****************************************************************************************************************************
//
//		カメラクラス
//
//*****************************************************************************************************************************
#include	"../textLoader\textLoader.h"

class TimeLineCameraPattern
{
public:
	int no;			// 自分のパターン番号
	int data_count;	// データの持ってる数
	struct Data
	{
		int start_frame;		// タイムライン上での開始フレーム
		int end_frame;			// タイムライン上での終了フレーム
		int frame_len;			// 1パターンあたりのフレーム数(バーの長さ)
		Vector3 *pos_array;		// 座標(開始と終わりだけでなく、中間とかも作るので可変長)
		Vector3 *target_array;	// 注視点(同じく可変長)
		int num_elements;		// 上記可変長の要素数
	}*data;
};

class Camera;

class EffectCamera : public textLoader
{
private:
	enum class APPOINT
	{
		NONE,			// 座標そのまま
		DIRECT,			// Pos直接指定型
		SOME_ONE,		// 誰かのPos
		SOME_ONE_COOD	// 誰かのPos＋Vector3
	};

	//------------------------------------------------------
	//	スクリプト関係
	//------------------------------------------------------
	int pattern_no;
	int data_cursor;
	char message[256];	// コマンドの名前？
	unsigned long ptr;	// 
	int current_frame;	// 現在のフレーム数
	TimeLineCameraPattern *time_line_data = new TimeLineCameraPattern;

	bool In_event(char *command);
	void Out_event();
	bool Jump(char *label_name);
	void Change_camera_mode();
	void Setting_camera(Camera::MODE mode);
	void Getting_targeter(Vector3 *out);
	void Getting_targeter_coodinate(Vector3 *out);

	//------------------------------------------------------
	//	制御するカメラさん
	//------------------------------------------------------
	Camera *camera;

	// ベジエ曲線
	void Bezier(Vector3 *out, Vector3 pos_array[], int num_elements_array, int current_frame, int end_frame);

	void Get_time_line_camera_pos(Vector3 *out_pos, Vector3 *out_target);

public:

	//------------------------------------------------------
	//	初期化・解放
	//------------------------------------------------------
	EffectCamera();
	~EffectCamera();
	void Initialize(Camera *me, LPSTR filename);

	//------------------------------------------------------
	//	更新
	//------------------------------------------------------
	void Update();

	//------------------------------------------------------
	//	行動セット
	//------------------------------------------------------
	bool Set_pattern(int pat);
};