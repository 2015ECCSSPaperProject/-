#pragma once

#include <process.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

// キューに贈るイベント
class PaperData
{
public:
	int ID;	//何番が
	int from; //誰から
};

class PaperQueue
{
public:

	// 初期化
	void Clear();

	// キューに詰める
	void Push(PaperData pData);

	// 更新
	PaperData Update();

private:

	// PaperDataキュー
	queue<PaperData> EventQueue;

};
