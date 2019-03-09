#ifndef __ComponentImage_h__
#define __ComponentImage_h__

#include "Component.h"
#include "Math/float3.h"
#include <vector>

struct Texture;

class ComponentImage : public Component
{
public:
	ComponentImage();
	ComponentImage(GameObject* gameobject);
	ComponentImage(const ComponentImage &copy);
	~ComponentImage();
	Component* Clone() const override;

	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(const JSON_value &value) override;

	const char* textureName = "None Selected";
	float3 color = float3::zero;
	Texture* texture = nullptr;

	std::vector<std::string> textureFiles;

};

#endif // __ComponentImage_h__

