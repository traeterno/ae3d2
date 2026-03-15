#ifndef aeGLTF
#define aeGLTF

#include <ae/types.hpp>
#include <vector>
#include <string>
#include <glm/gtc/quaternion.hpp>

namespace ae::gltf
{

struct Buffer
{
	u8* data;
	usize byteLength;
};

struct BufferView
{
	u8 buffer;
	usize byteOffset;
	usize byteLength;
	u16 target;
};

struct Accessor
{
	u16 bufferView;
	u16 componentType;
	usize count;
	std::string type;
};

struct Scene
{
	std::string name;
	std::vector<u16> nodes;
};

struct Node
{
	std::vector<u16> children;
	u8 mesh;
	u8 skin;
	std::string name;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 scale;
};

enum class Interpolation { Step, Linear, CubicSpline };

struct Animation
{
	struct Channel
	{
		u16 sampler;
		u16 targetNode;
		enum class TargetPath
		{
			Translation, Rotation, Scale
		} targetPath;
	};
	struct Sampler
	{
		u16 input;
		Interpolation interpolation;
		u16 output;
	};

	std::vector<Channel> channels;
	f32 duration;
	std::string name;
	std::vector<Sampler> samplers;
	bool repeat;
};

struct Mesh
{
	std::string name;
	u16 vertices;
	u16 normal;
	u16 texCoord;
	u16 joints;
	u16 weights;
	u16 indices;
	u8 material;
};

struct Skin
{
	u16 inverseBindMatrices;
	std::vector<u16> joints;
	std::string name;
};

struct Material
{
	std::string name;
	u8 texture;
};

struct Texture
{
	u8 sampler;
	u8 source;
};

struct Sampler
{
	u16 magFilter;
	u16 minFilter;
	u16 wrapS;
	u16 wrapT;
};

struct Image
{
	std::string mimeType;
	std::string name;
	std::string uri;
};

struct GLTF
{
	std::vector<Buffer> buffers;
	std::vector<BufferView> bufferViews;
	std::vector<Accessor> accessors;
	u8 scene;
	std::vector<Scene> scenes;
	std::vector<Node> nodes;
	std::vector<Animation> animations;
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::vector<Image> images;
	std::vector<Skin> skins;
	std::vector<Material> materials;
	std::vector<Sampler> samplers;
};

GLTF* load(const char* id);
f32 applyInterpolation(Interpolation ip, f32 t);

};

#endif