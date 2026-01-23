#pragma once

#define _HAS_STD_BYTE 0

#include <string>
#include "Globals.h"
#include "Log.h"

using namespace std;

class Module
{
public:

	Module() : active(false)
	{
	}

	virtual ~Module() = default; 

    virtual bool Start() 
	{ return true; }

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake()
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update() // Aqui luego podemos a�adit el dt
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

public:

	std::string name;
	bool active;
	typedef unsigned int Uint, uint;
};