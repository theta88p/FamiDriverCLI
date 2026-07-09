#pragma once
#include "NoteMap.h"

using NoteMapTgt = int;

struct NoteMapDif
{
	int number = 0;
	std::map<NoteMapTgt, NoteMap> maplist;

	NoteMapDif operator=(const NoteMapDif& nmd)
	{
		this->number = nmd.number;
		this->maplist = nmd.maplist;
		return nmd;
	}
};

bool operator<(const NoteMapDif& t1, const NoteMapDif& t2);