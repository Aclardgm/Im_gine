#pragma once


//Rendering Hardware Interface
class RHI
{

public:

	virtual ~RHI() {}

	virtual void init() = 0;

	virtual void postInit() = 0;

	virtual void shutdown() = 0;

};



