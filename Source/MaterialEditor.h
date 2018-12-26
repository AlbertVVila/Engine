#ifndef __MaterialEditor_h__
#define __MaterialEditor_h__

class Material;

class MaterialEditor
{
public:
	MaterialEditor();
	~MaterialEditor();

	void Draw();
	void CleanUp();

public:
	bool open=false;
	bool isUsed = false;
	Material* material = nullptr;
};

#endif __MaterialEditor_h__
