#pragma once

class Version
{
public:
	static int GetGameVersion()
	{
		return 1;
	}
	static bool IsSupported(int v)
	{
		if (v == 1)
			return true;

		return false;
	}
};