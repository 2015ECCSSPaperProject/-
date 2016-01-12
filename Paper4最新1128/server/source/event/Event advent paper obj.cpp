
#include "Event.h"
#include "Event advent paper obj.h"

#include "iextreme.h"
#include "../paper object/paper object.h"

#include "../../../share_data/Enum_public.h"

Event_advent_paper_obj::Event_advent_paper_obj(Paper_obj *obj) : Event(), obj(obj)
{
	obj->Rend(); // ”j‚ê‚½ó‘Ô‚Å‘Ò‹@
}

void Event_advent_paper_obj::Start()
{
	obj->Set_broken_true();
}
