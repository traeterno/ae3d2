#ifndef aeGLTF
#define aeGLTF

#include "ae/types.hpp"
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace ae { class Camera; }

namespace ae::gltf { struct GLTF; }

namespace ae::mesh
{

class Mesh
{
public:
	Mesh(ae::Camera* cam);
	~Mesh();
	void load(gltf::GLTF* file, const char* id);
	void destroy();
	void render();
private:
	u32 vbo;
	u32 ebo;
	usize indices;
	u32 texture;
	ae::Camera* cam;
};

class Bone
{
public:
	Bone();
	Bone(gltf::GLTF* file, u16 id);
	~Bone();
	void update(glm::mat4 parent);
	void render(std::vector<glm::vec3>* pts);
private:
	std::string name;
	std::vector<Bone> children;
	glm::mat4 ts;
	glm::quat angle;
	glm::mat4 translation;
	f32 length;
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();
	void load(gltf::GLTF* file, u8 id);
	void update();
	void render(ae::Camera* cam);
private:
	std::vector<glm::mat4> inverseBindMatrices;
	std::vector<glm::mat4> joints;
	std::vector<Bone> bones;
	u32 vbo;
};

};

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
	u16 mesh;
	u16 skin;
	std::string name;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 scale;
};

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
		enum class Interpolation
		{
			CubicSpline, Linear, Step
		} interpolation;
		u16 output;
	};

	std::vector<Channel> channels;
	f32 duration;
	std::string name;
	std::vector<Sampler> samplers;
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

};

#endif