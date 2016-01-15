#include	"iextreme.h"
#include	"../system/System.h"

#include	"../Player/BasePlayer.h"
#include	"camera.h"
#include	"EffectCamera.h"


//*****************************************************************************************************************************
//
//		超かっこいいカメラクラス
//
//*****************************************************************************************************************************

//=============================================================================================
//		初	期	化
EffectCamera::EffectCamera() :pattern_no(0), ptr(0), current_frame(0), time_line_data(nullptr), data_cursor(-1)
{
}

void EffectCamera::Initialize(Camera *me, LPSTR filename)
{
	// スクリプトIN
	textLoader::Open(filename);

	memset(message, 0, sizeof(message));
	time_line_data = nullptr;
	camera = me;
}
//
//=============================================================================================


//=============================================================================================
//		開		放
EffectCamera::~EffectCamera()
{

}
//
//=============================================================================================


//=============================================================================================
//		更		新
Vector3 operator * (const Vector3 &v, const Matrix &m)
{
	Vector3 ret;
	ret.x = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]);
	ret.y = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]);
	ret.z = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]);
	return ret;
}
void Rot2D(float rad, float *x, float *y)
{
	float ret_x = (*x * cosf(rad)) - (*y * sinf(rad));
	float ret_y = (*x * sinf(rad)) + (*y * cosf(rad));
	*x = ret_x;
	*y = ret_y;
}

void EffectCamera::Update()
{
	//Out_event();
	data_cursor = -1;

	if (time_line_data)
	{
		for (int i = 0; i < time_line_data->data_count; i++)
		{
			if (current_frame >= time_line_data->data[i].start_frame &&
				current_frame <= time_line_data->data[i].end_frame)
			{
				data_cursor = i;
				Vector3 pt[2];
				Get_time_line_camera_pos(&pt[0], &pt[1]);

				// プレイヤー座標系にする
				Rot2D(camera->Get_my_player()->Get_angleY(), &pt[0].z, &pt[0].x);
				pt[0] += camera->Get_pos();
				pt[1] += camera->Get_my_player()->Get_pos();
				camera->Set(pt[0], pt[1]);
				break;
			}
		}

		if (++current_frame > time_line_data->data[time_line_data->data_count - 1].end_frame)
		{
			current_frame = 0;
			camera->scriptON = false;
			delete time_line_data, time_line_data = nullptr;
		}
	}
}
//
//=============================================================================================

//=============================================================================================
//		ラベルジャンプ
bool EffectCamera::Jump(char *label_name)
{
	//ポインタを先頭に
	ptr = GetPointer();
	SetPointer(0);

	//ラベル検索
	if (Search(label_name))
	{
		return true;	// 見つかった場所にポインタが移動しているので何もしない
	}

	SetPointer(ptr);	// 見つからない場合戻す
	return false;
}
//
//=============================================================================================


//=============================================================================================
//		行	動	セ	ッ	ト
bool EffectCamera::Set_pattern(int pat)
{
	// スクリプトエンジンON
	camera->scriptON = true;
	current_frame = 0;
	data_cursor = 0;

	char str[16];
	sprintf_s(str, "PATTERN:%d", pat);

	bool success = Jump(str);	// 飛べたかどうか(本来true)

	if (success)
	{
		pattern_no = pat;

		if (time_line_data) delete time_line_data;
		time_line_data = new TimeLineCameraPattern;

		char work[64];
		LoadString(work);
		LPSTR ctx;// strtok_sの第3引数に使用するのだが、内部的に使用するので深く考えないらしい
		LPSTR t = strtok_s(work, ":", &ctx);
		t = strtok_s(0, "\t\n", &ctx);
		// 個数
		time_line_data->data_count = atoi(t);
		time_line_data->data = new TimeLineCameraPattern::Data[time_line_data->data_count];	// データの個数分

		for (int i = 0; i < time_line_data->data_count; i++)
		{
			// 開始フレームと終了フレーム読み込み
			time_line_data->data[i].start_frame = LoadInt();
			time_line_data->data[i].end_frame = LoadInt();
			time_line_data->data[i].frame_len = time_line_data->data[i].end_frame - time_line_data->data[i].start_frame;	// フレームの長さは自分で計算

			// 座標・注視点要素数読み込み
			time_line_data->data[i].num_elements = LoadInt();
			// 要素数の分だけnew
			time_line_data->data[i].pos_array = new Vector3[time_line_data->data[i].num_elements];
			time_line_data->data[i].target_array = new Vector3[time_line_data->data[i].num_elements];

			// 座標・注視点読み込み
			for (int j = 0; j < time_line_data->data[i].num_elements; j++)
			{
				time_line_data->data[i].pos_array[j].x = LoadFloat();
				time_line_data->data[i].pos_array[j].y = LoadFloat();
				time_line_data->data[i].pos_array[j].z = LoadFloat();
				time_line_data->data[i].target_array[j].x = LoadFloat();
				time_line_data->data[i].target_array[j].y = LoadFloat();
				time_line_data->data[i].target_array[j].z = LoadFloat();
			}

			time_line_data->data[i].fix = (time_line_data->data[i].num_elements == 1);
		}
	}
	return success;
}
//
//=============================================================================================



void EffectCamera::Get_time_line_camera_pos(Vector3 *out_pos, Vector3 *out_target)
{
	if (data_cursor == -1) return;
	if (time_line_data->data[data_cursor].fix)
	{
		*out_pos = time_line_data->data[data_cursor].pos_array[0];
		*out_target = time_line_data->data[data_cursor].target_array[0];
	}

	// ベジエ計算関数に丸投げ
	Bezier(
		out_pos,																	// 受け皿
		time_line_data->data[data_cursor].pos_array,								// 始点、中間、終点が入ってる座標配列
		time_line_data->data[data_cursor].num_elements,								// の要素数
		current_frame - time_line_data->data[data_cursor].start_frame,				// 今のフレーム
		time_line_data->data[data_cursor].frame_len									// 終了フレーム
		);

	Bezier(
		out_target,
		time_line_data->data[data_cursor].target_array,								// 始点、中間、終点が入ってる座標配列
		time_line_data->data[data_cursor].num_elements,								// の要素数
		current_frame - time_line_data->data[data_cursor].start_frame,				// 今のフレーム
		time_line_data->data[data_cursor].frame_len
		);
}

void EffectCamera::Bezier(Vector3 *out, Vector3 pos_array[], int num_elements_array, int current_frame, int end_frame)
{
	assert(num_elements_array > 0);

	float b = current_frame / (float)end_frame;
	float a = 1 - b;

	/*				//		参考資料		//
	//ベジェ曲線↓　まず　　最初と中間　　　　次に　　　　中間と最後
	pos->x = a*a*a* p1.x + 3 * a*a*b*p2.x + 3 * a*b*b*p2.x + b*b*b*p3.x;
	pos->y = a*a*a* p1.y + 3 * a*a*b*p2.y + 3 * a*b*b*p2.y + b*b*b*p3.y;
	pos->z = a*a*a* p1.z + 3 * a*a*b*p2.z + 3 * a*b*b*p2.z + b*b*b*p3.z;
	*/

	// 始点
	*out = pos_array[0] * (float)pow(a, num_elements_array);

	// 中間
	for (int i = 1; i < num_elements_array - 1; i++)	// -1なのは終点を省くから
	{
		float mult = b;
		for (int j = 1; j < num_elements_array - 1; j++)
		{
			mult *= (j >= i) ? a : b;
		}
		*out += pos_array[i] * (num_elements_array * mult);
	}

	// 終点
	*out += pos_array[num_elements_array - 1] * (float)pow(b, num_elements_array);
}
//
//*****************************************************************************************************************************




//=============================================================================================
//		内部コマンド実行
bool EffectCamera::In_event(char *command)
{
	// ※true:まだ処理中だから何も読み込まないでね


	while (EndCheck())
	{
		// コマンド読み込み
		LoadString(command);

		//================================================================
		//	待機コマンド
		if (lstrcmp(command, "WAIT") == 0)
		{
			//wait_timer = LoadInt();	// 待機時間読み込み
			return true;
		}
		break;
	}
	return false;
}
//
//=============================================================================================

//=============================================================================================
//		外部コマンド実行
void EffectCamera::Out_event()
{
	char com[256];
	while (EndCheck())
	{
		//スプリクト実行
		if (In_event(com))return;	// まだ内部処理中だから帰れ((((ﾉﾟ皿ﾟ)ﾉ

		//----------------------------
		// 外部用コマンド処理
		//----------------------------
		if (strcmp(com, "CHANGE") == 0)
		{
			Change_camera_mode();
			break;
		}
		if (strcmp(com, "END") == 0)
		{
			// スクリプトOFFにして、読み込むファイルの一番先頭にポインタを戻す(Set_patternが呼び出されるまでスクリプト停止)
			camera->scriptON = false;
			Jump("START");
			break;
		}
	}

}
//
//=============================================================================================





//=============================================================================================
//		カメラ設定
void EffectCamera::Setting_camera(Camera::MODE mode)
{
	Vector3 pos;
	Vector3 target;

	//================================================================
	//	読み込む数値指定
	int appoint[2];
	appoint[0] = LoadInt();
	appoint[1] = LoadInt();


	//================================================================
	//	位置読み込み
	switch ((APPOINT)appoint[0])
	{
	case APPOINT::NONE:
		pos = camera->Get_pos();
		break;
	case APPOINT::DIRECT:
		pos.x = LoadFloat();
		pos.y = LoadFloat();
		pos.z = LoadFloat();
		break;
	case APPOINT::SOME_ONE:
		Getting_targeter(&pos);
		break;
	case APPOINT::SOME_ONE_COOD:
		Getting_targeter_coodinate(&pos);
		break;
	}

	//================================================================
	//	ターゲット読み込み
	switch ((APPOINT)appoint[1])
	{
	case APPOINT::NONE:
		target = camera->Get_target();
		break;
	case APPOINT::DIRECT:
		target.x = LoadFloat();
		target.y = LoadFloat();
		target.z = LoadFloat();
		break;
	case APPOINT::SOME_ONE:
		Getting_targeter(&target);
		break;
	case APPOINT::SOME_ONE_COOD:
		Getting_targeter_coodinate(&target);
		break;
	}

	//================================================================
	//	位置・注視点の情報セット！！！
	camera->Change_mode(mode, pos, target);


	//================================================================
	//	補正モードならさらに読み込む
	if (mode == Camera::MODE::M_SLERP)
	{

		//================================================================
		//	読み込む数値指定
		appoint[0] = LoadInt();
		appoint[1] = LoadInt();


		//================================================================
		//	「目標」位置読み込み
		switch ((APPOINT)appoint[0])
		{
		case APPOINT::NONE:
			pos = camera->Get_pos();
			break;
		case APPOINT::DIRECT:
			pos.x = LoadFloat();
			pos.y = LoadFloat();
			pos.z = LoadFloat();
			break;
		case APPOINT::SOME_ONE:
			Getting_targeter(&pos);
			break;
		case APPOINT::SOME_ONE_COOD:
			Getting_targeter_coodinate(&pos);
			break;
		}

		//================================================================
		//	「目標」ターゲット読み込み
		switch ((APPOINT)appoint[1])
		{
		case APPOINT::NONE:
			target = camera->Get_pos();
			break;
		case APPOINT::DIRECT:
			target.x = LoadFloat();
			target.y = LoadFloat();
			target.z = LoadFloat();
			break;
		case APPOINT::SOME_ONE:
			Getting_targeter(&target);
			break;
		case APPOINT::SOME_ONE_COOD:
			Getting_targeter_coodinate(&target);
			break;
		}

		//================================================================
		//	補完割合読み込み
		const float p = LoadFloat();


		//================================================================
		//	目標座標セット
		camera->Set_slerp(pos, target, p);
	}
}

void EffectCamera::Getting_targeter(Vector3 *out)
{
	char targeter_name[16];
	LoadString(targeter_name);
	if (strcmp(targeter_name, "PLAYER") == 0)
	{
		*out = camera->Get_my_player()->Get_center_pos();
	}
}

void EffectCamera::Getting_targeter_coodinate(Vector3 *out)
{
	char targeter_name[16];
	LoadString(targeter_name);
	if (strcmp(targeter_name, "PLAYER") == 0)
	{
		Vector3 set;
		set = camera->Get_my_player()->Get_pos();
		set.x += LoadFloat();
		set.y += LoadFloat();
		set.z += LoadFloat();
		*out = set;
	}
}



//=============================================================================================
//		カメラモード変更
void EffectCamera::Change_camera_mode()
{
	// モード名読み込み
	char str[16];
	LoadString(str);

	static const char *n_list[4] =
	{
		"FIX", "PAN", "SLERP", "TPS"
	};

	static const Camera::MODE m_list[4] =
	{
		Camera::MODE::M_FIX, Camera::MODE::M_PAN, Camera::MODE::M_SLERP, Camera::MODE::M_TPS
	};

	for (int i = 0; i < 4; i++)
	{
		//	文字判定
		if (strcmp(str, n_list[i]) == 0)
		{
			Setting_camera(m_list[i]);
			return;
		}
	}

	MessageBox(0, "テキストのカメラモード名が間違っているよ", null, MB_OK);
}
//
//=============================================================================================

