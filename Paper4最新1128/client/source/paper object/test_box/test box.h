
#pragma once

// Paper_obj�̃e�X�g�Ŏg�����N���X
class Test_box : public Paper_obj
{
public:
	void Initialize(iex3DObj *model) override;
	void Update() override;
	void Render() override;
};
