#ifndef aeGLTF
#define aeGLTF

#include "ae/types.hpp"
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace ae { class Camera; }

namespace ae::gltf { struct GLTF; }

namespace ae::anim
{

enum class Interpolation { Step, Linear, CubicSpline };

struct Keyframe
{
	f32 timestamp;
	glm::quat rotation;
};

struct Timeline
{
	u16 bone;
	Interpolation func;
	std::vector<Keyframe> frames;
	usize currentFrame;
};

struct Animation
{
	f32 duration, currentTime;
	std::vector<Timeline> frames;
	bool repeat;
};

f32 applyInterpolation(Interpolation ip, f32 t);
std::tuple<std::string, Animation> loadAnimation(gltf::GLTF* file, u8 id);

}

namespace ae::mesh
{

class Bone;

typedef std::vector<Bone> BoneList;
using Mat4 = glm::mat4;
using Mat3 = glm::mat3;

class Bone
{
public:
	Bone();
	Bone(gltf::GLTF* file, u16 id);
	~Bone();
	void update(Mat4* bones, Mat3* frame, Mat4* ts, Mat3* joints, u16 index);
	u16 getID();
	f32 getLength();
private:
	u16 id;
	std::string name;
	std::vector<u16> children;
	glm::mat3 angle;
	glm::mat4 translation;
	f32 length;
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();
	void load(gltf::GLTF* file, u8 id);
	void update(f32 dt, ae::Camera* cam);
	void render(ae::Camera* cam, glm::mat4 ts);
	void setAnimation(std::string name);
private:
	glm::mat4* inverseBindMatrices;
	glm::mat4* ts;
	glm::mat3* joints;
	BoneList bones;
	std::unordered_map<std::string, anim::Animation> anims;
	std::string currentAnim;
	u32 vbo;
};

class Mesh
{
public:
	Mesh(ae::Camera* cam);
	~Mesh();
	void load(gltf::GLTF* file, const char* id);
	void destroy();
	void render(f32 dt, glm::mat3 rotation, glm::mat4 ts);
	Skeleton* getSkeleton();
private:
	u32 vbo;
	u32 ebo;
	usize indices;
	u32 texture;
	ae::Camera* cam;
	Skeleton* sk;
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
	u8 mesh;
	u8 skin;
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
		ae::anim::Interpolation interpolation;
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

};

#endif