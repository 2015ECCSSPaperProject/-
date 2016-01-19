
#pragma once

//#include <vector>

class iex3DObj;
class iexShader;

class Inhabitants
{
public:
	Inhabitants(const char *filename);
	~Inhabitants();
	void Update();
	void Render( iexShader *shader, char *name );

private:
	std::vector<iex3DObj*> obj_vector;
};
