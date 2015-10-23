
#ifndef PIE_GRAPH_H
#define PIE_GRAPH_H

#include "iextreme.h"

class Pie_graph : protected iex2DObj
{
protected:
	float percent; // 0~1
public:
	Pie_graph(char *filename);
	~Pie_graph();

	void Render(s32 DstX, s32 DstY, s32 DstW, s32 DstH, s32 SrcX, s32 SrcY, s32 width, s32 height, u32 dwFlags = RS_COPY, COLOR color = 0xFFFFFFFF, float z = .0f);
	// 0~1
	void Set_percent(float in){ percent = in; }
};

#endif // !PIE_GRAPH_H
