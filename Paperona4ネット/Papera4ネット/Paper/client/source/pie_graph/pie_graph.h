
#ifndef PIE_GRAPH_H
#define PIE_GRAPH_H

#include "iextreme.h"
#include <vector>

class Pie_graph
{
private:
	class Content : protected iex2DObj
	{
	public:
		float size;

		Content(char *filename);
		~Content();

		void Render(float percent, s32 DstX, s32 DstY, s32 DstW, s32 DstH, s32 SrcX, s32 SrcY, s32 width, s32 height, u32 dwFlags = RS_COPY, COLOR color = 0xFFFFFFFF, float z = .0f);
	};

	std::vector<Content*> contents;
public:
	Pie_graph();
	~Pie_graph();

	void Render(s32 DstX, s32 DstY, s32 DstW, s32 DstH, s32 SrcX, s32 SrcY, s32 width, s32 height, u32 dwFlags = RS_COPY, COLOR color = 0xFFFFFFFF, float z = .0f);

	void Set_percent(int num, // ContentsÇÃî‘çÜ
		float size); // äÑçá(0à»è„)

	void Add_content(char*filename); // âÊëúÇÃ
};

#endif // !PIE_GRAPH_H
