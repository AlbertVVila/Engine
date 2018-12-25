//#ifndef __ComponentMaterial_h__
//#define __ComponentMaterial_h__
//
//#include "Component.h"
//#include "Math/float3.h"
//#include "Math/float4.h"
//#include <list>
//
//#define MAXTEXTURES 4
//#define DEFAULTMAT "Default"
//
//struct Texture;
//struct Shader;
//
//enum class TextureType
//{
//	DIFFUSE=0,
//	SPECULAR,
//	OCCLUSION,
//	EMISSIVE
//};
//
//class ComponentMaterial :
//	public Component
//{
//public:
//	ComponentMaterial(GameObject* gameobject);
//	ComponentMaterial(const ComponentMaterial& component);
//	~ComponentMaterial();
//
//	Component* Clone() const override;
//	bool CleanUp() override;
//
//	void DeleteTexture();
//	
//	void SetTexture(const char * data, TextureType type);
//	void SetShader(const char * shaderName);
//	Texture * GetTexture(TextureType type) const;
//	Shader* GetShader() const;
//	void DrawProperties() override;
//	void Save(JSON_value *value) const override;
//	void Load(const JSON_value &value) override;
//
//	std::list<Texture*> GetTextures() const;
//	void SetMaterial(const char * file = nullptr);
//	void SetMaterial(const Material & material);
//
//
//public:
//	Material * material = nullptr;
//
//};
//
//#endif //__ComponentMaterial_h__
