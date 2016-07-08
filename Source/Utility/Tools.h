#pragma once

// additional tools
#include <algorithm>
#include <memory>
#include <string>

namespace tool
{
	template <class T>
	inline T clamp(const T& val, const T& min, const T& max)
	{
		return std::min(max, std::max(min, val));
	}

	template <class T>
	inline void safeDelete(T*& p)
	{
		delete p;
		p = nullptr;
	}

	template <class T>
	inline void safeDeleteArray(T*& p)
	{
		delete [] p;
		p = nullptr;
	}

	template <class T>
	inline void zeroStruct(T* s)
	{
		memset(s, 0, sizeof(T));
	}

	template <class T>
	inline const char* getClassname(const T& t)
	{
		return typeid(t).name();
	}

	/*
	cutting of left part

	src = string to cut
	c = char that will be cut

	eg: cutStringLeft("data/pic/picture.png", '/')
	-> picture.png

	if c could not be found, src will be returned
	*/
	inline std::string cutStringLeft(const std::string& src, char c)
	{
		if (src.length() == 0)
			return src;
		// search from left to find char
		size_t cutpos = src.length() - 1;
		for (; cutpos > 0; cutpos--)
		{
			if (src[cutpos] == c)
				break;
		}

		if (cutpos == 0 && src[0] != c)
			return src; // no match found

		return src.substr(cutpos + 1, src.length() - 1);
	}

	/*
	cutting of right part

	src = string to cut
	c = char that will be cut

	eg: cutStringRight("picture.png", '.')
	-> picture

	cutStringRight("picture.png.zip", '.')
	-> picture.png

	if c could not be found, src will be returned
	*/
	inline std::string cutStringRight(const std::string& src, char c)
	{
		if (src.length() == 0)
			return src;
		// search from left to find char
		size_t cutpos = src.length() - 1;
		for (; cutpos > 0; cutpos--)
		{
			if (src[cutpos] == c)
				break;
		}

		if (cutpos == 0 && src[0] != c)
			return src; // no match found

		return src.substr(0, cutpos);
	}

	/*
		removes extensions and path

		data/pic/pic.png -> pic

		removes / and \
	*/
	inline std::string fileRemovePath(const std::string& src)
	{
		return cutStringRight(cutStringLeft(cutStringLeft(src, '/'), '\\'), '.');
	}

	/*
		adds file extension
		fileAddExtension(pic,png) -> pic.png
		fileAddExtension(pic.png,png) -> pic.png
		fileAddExtension(pic.zip,png) -> pic.zip.png
		fileAddExtension(.png,png) -> .png
	*/
	inline std::string fileAddExtension(const std::string& src, std::string ext)
	{
		ext = "." + ext;
		if (src.length() < ext.length())
			return src + ext;

		bool bSameEnd = true;
		size_t elen = ext.length();
		size_t slen = src.length();
		for (size_t i = 1; i <= ext.length(); i++)
		{
			if (ext[elen - i] != src[slen - i])
			{
				bSameEnd = false;
				break;
			}
		}

		if (bSameEnd)
			return src; // has already extension
		else
			return src + ext;
	}

	/*
		checks if strings end with ext

		fileHasExtension("hallo.txt",".txt") -> true
	*/
	inline bool fileHasExtension(const std::string& src, const std::string& ext)
	{
		if (src.length() < ext.length())
			return false;

		size_t elen = ext.length();
		size_t slen = src.length();
		for (size_t i = 1; i <= ext.length(); i++)
		{
			if (ext[elen - i] != src[slen - i])
			{
				return false;
			}
		}
		return true;
	}
}