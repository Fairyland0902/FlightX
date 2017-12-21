#pragma once
#include "model.h"
#include "camera.h"

class Aircraft :
	public Model,
	public Camera
{
public:
	Aircraft(string);
	~Aircraft();
};

