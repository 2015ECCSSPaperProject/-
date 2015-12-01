
#pragma once

class Score;
class BasePlayer;

class Poster : public Paper_obj
{
public:
	Poster();
	~Poster();

	void Initialize(iex3DObj *model) override;
	void Update() override;
	void Render() override;
};
