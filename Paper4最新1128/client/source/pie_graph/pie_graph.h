
#ifndef PIE_GRAPH_H
#define PIE_GRAPH_H

#include "iextreme.h"
#include <vector>

class Pie_graph_content : protected iex2DObj
{
public:
	float size;

	Pie_graph_content(char *filename);
	~Pie_graph_content();

	void Render(float percent, s32 DstX, s32 DstY, s32 DstW, s32 DstH, s32 SrcX, s32 SrcY, s32 width, s32 height, u32 dwFlags = RS_COPY, COLOR color = 0xFFFFFFFF, float z = .0f);
};

class Pie_graph
{
public:

	Pie_graph();
	~Pie_graph();

	void Render(s32 DstX, s32 DstY, s32 DstW, s32 DstH, s32 SrcX, s32 SrcY, s32 width, s32 height, u32 dwFlags = RS_COPY, COLOR color = 0xFFFFFFFF, float z = .0f);

	void Set_percent(int num, // ContentsÇÃî‘çÜ
		float size); // äÑçá(0à»è„)

	void Add_content(char*filename); // âÊëúÇÃ

private:

	std::vector<Pie_graph_content*> contents;

};

#endif // !PIE_GRAPH_H
