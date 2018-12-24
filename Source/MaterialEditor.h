#ifndef __MaterialEditor_h__
#define __MaterialEditor_h__

class ComponentMaterial;

class MaterialEditor
{
public:
	MaterialEditor();
	~MaterialEditor();

	void Draw();
	void Edit(ComponentMaterial* material);

	void CleanUp();

public:
	bool open=false;
	ComponentMaterial* material = nullptr;
};

#endif __MaterialEditor_h__
