#ifndef aeMesh
#define aeMesh

#include "ae/types.hpp"
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace ae { class Camera; }

namespace ae::gltf { struct GLTF; enum class Interpolation; }

namespace ae::anim
{

struct Keyframe
{
	f32 timestamp;
	glm::quat rotation;
};

struct Timeline
{
	u16 bone;
	ae::gltf::Interpolation func;
	std::vector<Keyframe> frames;
	usize currentFrame;
};

struct Animation
{
	f32 duration, currentTime;
	std::vector<Timeline> frames;
	bool repeat;
};

std::pair<std::string, Animation> loadAnimation(gltf::GLTF* file, u8 id);

}

namespace ae::mesh
{

class Skeleton;

class Bone
{
public:
	Bone(Skeleton* sk, gltf::GLTF* file, u16 nodeID, u16* id);
	~Bone();
	void update(glm::mat4* ts, glm::mat3* f, glm::mat4 pts);
	u16 getID();
	f32 getLength();
	void render(glm::mat4* ts, f32* pts, u16* counter);
	Bone* getBone(std::string path);
	glm::mat4 getTS();
private:
	u16 nodeID, id;
	std::string name;
	std::vector<Bone> children;
	glm::mat3 angle;
	glm::mat4 translation;
	f32 length;
	Skeleton* sk;
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();
	void load(gltf::GLTF* file, u8 id);
	void update(f32 dt, ae::Camera* cam);
	void render(ae::Camera* cam, glm::mat4 ts);
	void startAnimation(std::string name);
	void stopAnimation(std::string name);
	glm::mat4 getBoneTransform(u16 id);
	Bone* getBone(std::string path);
private:
	glm::mat4* inverseBindMatrices;
	glm::mat4* ts;
	std::vector<Bone> bones;
	std::unordered_map<std::string, anim::Animation> anims;
	std::vector<std::string> currentAnim;
	u32 vbo;
	u16 bonesCount;
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

#endif