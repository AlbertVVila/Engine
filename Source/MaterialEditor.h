#ifndef __MaterialEditor_h__
#define __MaterialEditor_h__

struct Material;

class MaterialEditor
{
public:
	MaterialEditor();
	~MaterialEditor();

	void Draw();
	void Edit(Material* material);

	void CleanUp();

public:
	bool open=false;
	Material* material = nullptr;
};

#endif __MaterialEditor_h__
