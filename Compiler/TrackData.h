#pragma once
#include <vector>

struct TrackData
{
	int device = 0;
	int track = 0;
	std::vector<unsigned char>data;

	bool operator<(const TrackData& right)
	{
		if (device < right.device)
			return true;

		if (device > right.device)
			return false;

		return track < right.track;
	}

	bool operator>(const TrackData& right)
	{
		if (device < right.device)
			return false;

		if (device > right.device)
			return true;

		return track > right.track;
	}
};
