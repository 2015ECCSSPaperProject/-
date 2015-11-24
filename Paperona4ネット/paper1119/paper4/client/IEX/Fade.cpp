
#include	"iExtreme.h"

//スタティック
bool FadeControl::isFade = 0;
int FadeControl::mode = 0;
int FadeControl::count = 0;
int FadeControl::param = 0;
bool FadeControl::isFadeIn = 0;
bool FadeControl::isFadeOut = 0;
bool FadeControl::isFadeIn_W = 0;
bool FadeControl::isFadeOut_W = 0;

void FadeControl::Init(){

	mode = FADE_IN;
	count = 0;
	param = 0;
	isFade = false;

	isFadeIn = false;
	isFadeOut = false;
	isFadeIn_W = false;
	isFadeOut_W = false;

}

void FadeControl::Setting(int mode, int speed){

	FadeControl::mode = mode;
	param = speed;
	isFade = true;

	switch (mode)
	{
	case FADE_IN:
		count = 255;
		break;

	case FADE_OUT:
		count = 0;
		break;

	case FADE_IN_W:
		count = 255;
		break;

	case FADE_OUT_W:
		count = 0;
		break;

	}

}
void FadeControl::Update(){

	/*___最初にフェードスイッチリセット___*/
	isFadeIn = false;
	isFadeOut = false;
	isFadeIn_W = false;
	isFadeOut_W = false;

	if (!isFade) return;//完了したら処理をしない。

	switch (mode)
	{
	case FADE_IN:
		count -= param;

		if (count <= 0){
			count = 0;
			isFade = false;
			isFadeIn = true;
		}
		else isFadeIn = false;
		break;

	case FADE_OUT:
		count += param;

		if (count >= 255){
			count = 255;
			isFade = false;
			isFadeOut = true;
		}
		else isFadeOut = false;
		break;

	case FADE_IN_W:
		count -= param;

		if (count <= 0){
			count = 0;
			isFade = false;
			isFadeIn_W = true;
		}
		else isFadeIn_W = false;
		break;

	case FADE_OUT_W:
		count += param;

		if (count >= 255){
			count = 255;
			isFade = false;
			isFadeOut_W = true;
		}
		else isFadeOut_W = false;
		break;

	}


}
void FadeControl::Render(){

	DWORD color;//１６進数で扱うとき

	switch (mode)
	{
	case FADE_IN:
		color = ARGB(count, 0, 0, 0);//000で黒の板　　255 255255で白の板
		break;
	case FADE_OUT:
		color = ARGB(count, 0, 0, 0);//000で黒の板　　255 255255で白の板
		break;
	case FADE_IN_W:
		color = ARGB(count, 255, 255, 255);//000で黒の板　　255 255255で白の板
		break;
	case FADE_OUT_W:
		color = ARGB(count, 255, 255, 255);//000で黒の板　　255 255255で白の板
		break;
	}

	iexPolygon::Rect(0, 0, 1280, 720, RS_COPY, color);

}