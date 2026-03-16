#include <ae/mesh.hpp>
#include <ae/camera.hpp>
#include <ae/global.hpp>
#include <ae/gltf.hpp>

#include <glad/glad.h>

ae::mesh::Mesh::Mesh(ae::Camera* camera)
{
	if (camera == nullptr) return;
	this->vbo = camera->createVBO();
	this->ebo = camera->createVBO();
	this->cam = camera;
	this->sk = nullptr;
	this->texture = 0;
}

ae::mesh::Mesh::~Mesh()
{
	this->vbo = 0;
	this->ebo = 0;
	if (this->sk != nullptr) delete this->sk;
}

void ae::mesh::Mesh::load(ae::gltf::GLTF* file, const char* id)
{
	gltf::Node* node = nullptr;
	for (auto x: file->nodes) if (x.name == id) { node = &x; break; }
	if (!node)
	{
		printf("Error: Node \"%s\" not found\n", id);
		return;
	}
	if (node->mesh == U8_MAX)
	{
		printf("Error: Node \"%s\" does not contain mesh\n", id);
		return;
	}

	auto m = &file->meshes[node->mesh];
	printf("Loading node \"%s\" as mesh \"%s\"\n", id, m->name.c_str());

	auto ia = &file->accessors[m->indices];
	auto ibv = &file->bufferViews[ia->bufferView];
	auto ib = &file->buffers[ibv->buffer];

	auto va = &file->accessors[m->vertices];
	auto vbv = &file->bufferViews[va->bufferView];
	auto vertices = (glm::vec3*)&file->buffers[vbv->buffer]
		.data[vbv->byteOffset];

	auto na = &file->accessors[m->normal];
	auto nbv = &file->bufferViews[na->bufferView];
	auto normals = (glm::vec3*)&file->buffers[nbv->buffer]
		.data[nbv->byteOffset];

	auto ta = &file->accessors[m->texCoord];
	auto tbv = &file->bufferViews[ta->bufferView];
	auto uvs = (glm::vec2*)&file->buffers[tbv->buffer]
		.data[tbv->byteOffset];
	
	glm::u8vec4* joints = nullptr;

	if (m->joints != U16_MAX)
	{
		auto ja = &file->accessors[m->joints];
		auto jbv = &file->bufferViews[ja->bufferView];
		joints = (glm::u8vec4*)&file->buffers[jbv->buffer]
			.data[jbv->byteOffset];
	}
	else
	{
		joints = new glm::u8vec4[va->count];
		for (usize i = 0; i < va->count; i++)
		{
			joints[i].x = 255;
		}
	}

	auto buf = new f32[va->count * 9];

	for (usize i = 0 ; i < va->count; i++)
	{
		(*(glm::vec3*)&buf[i*9+0]) = vertices[i];
		(*(glm::vec3*)&buf[i*9+3]) = normals[i];
		buf[i*9+6] = (f32)joints[i].x;
		(*(glm::vec2*)&buf[i*9+7]) = uvs[i];
	}

	if (m->joints == U16_MAX) delete[] joints;
	
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(
		GL_ARRAY_BUFFER, va->count * 9 * sizeof(f32),
		buf, GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, ibv->byteLength,
		&ib->data[ibv->byteOffset], GL_STATIC_DRAW
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	this->indices = ia->count;

	if (node->skin != U8_MAX)
	{
		this->sk = new Skeleton;
		this->sk->load(file, node->skin);
	}

	if (m->material == U8_MAX) return;

	auto mat = &file->materials[m->material];
	printf("Using material \"%s\"\n", mat->name.c_str());
	auto tex = &file->textures[mat->texture];
	auto img = &file->images[tex->source];
	auto sampler = &file->samplers[tex->sampler];

	this->texture = this->cam->getTexture(
		ae::str::format("meshes/%s", img->name.c_str()).c_str()
	).id;
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler->magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler->magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler->wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler->wrapT);
}

void ae::mesh::Mesh::destroy()
{
	this->cam->removeVBO(this->vbo);
	this->cam->removeVBO(this->ebo);
	glDeleteTextures(1, &this->texture);
	if (this->sk != nullptr)
	{
		delete this->sk;
		this->sk = nullptr;
	}
	this->cam = nullptr;
}

void ae::mesh::Mesh::render(f32 dt, glm::mat3 rotation, glm::mat4 ts)
{
	this->cam->shaderUse("mesh");
	if (this->sk != nullptr) this->sk->update(dt, this->cam);
	this->cam->bindMeshVAO();
	this->cam->bindTexture(this->texture);
	this->cam->shaderMat3("rotation", rotation);
	this->cam->shaderSetModel(ts);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 36, 0
	);
	glVertexAttribPointer(
		1, 4, GL_FLOAT, GL_FALSE, 36, (void*)12
	);
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE, 36, (void*)28
	);
	glDrawElements(GL_TRIANGLES, this->indices, GL_UNSIGNED_SHORT, 0);
}

ae::mesh::Skeleton* ae::mesh::Mesh::getSkeleton()
{
	return this->sk;
}

std::pair<std::string, ae::anim::Animation>
	ae::anim::loadAnimation(gltf::GLTF *file, u8 id)
{
	auto anim = &file->animations[id];
	Animation a;
	a.currentTime = 0.0;
	a.duration = anim->duration;
	a.repeat = anim->repeat;
	for (auto c: anim->channels)
	{
		// TODO if you'll add ability to change smth other than rotation,
		// then use timeline not as vector of keyframes,
		// but number of vectors for each type of changes
		using TP = gltf::Animation::Channel::TargetPath;
		if (c.targetPath != TP::Rotation) { continue; }
		Timeline tl;
		auto s = &anim->samplers[c.sampler];
		tl.func = s->interpolation;
		tl.currentFrame = 0;
		tl.bone = c.targetNode;

		auto ia = &file->accessors[s->input];
		auto ibv = &file->bufferViews[ia->bufferView];
		auto ts = (f32*)&file->buffers[ibv->buffer].data[ibv->byteOffset];
		
		auto oa = &file->accessors[s->output];
		auto obv = &file->bufferViews[oa->bufferView];
		auto rot = (glm::vec4*)&file->buffers[obv->buffer].data[obv->byteOffset];

		for (u16 i = 0; i < ia->count; i++)
		{
			auto r = rot[i*3+1];
			tl.frames.push_back({
				.timestamp = ts[i],
				.rotation = glm::quat(r.w, r.x, r.y, r.z)
			});
		}

		a.frames.push_back(tl);
	}
	return {anim->name, a};
}

ae::mesh::Bone::Bone()
{
	this->children = {};
	this->angle = glm::mat3(1.0);
	this->translation = glm::mat4(1.0);
	this->length = 1.0;
	this->id = U16_MAX;
}

ae::mesh::Bone::Bone(gltf::GLTF* file, u16 nodeID, u16* id)
{
	auto n = &file->nodes[nodeID];
	
	this->name = n->name;
	this->nodeID = nodeID;
	this->id = (*id)++;
	this->angle = glm::mat3_cast(n->rotation);
	this->translation = glm::translate(glm::mat4(1.0), n->translation);

	if (!n->children.empty())
	{
		this->length = glm::length(file->nodes[n->children[0]].translation);
	}
	else { this->length = 0.1; }

	for (u16 i: n->children)
	{
		this->children.push_back(Bone(file, i, id));
	}
}

ae::mesh::Bone::~Bone()
{
	this->children.clear();
}

void ae::mesh::Bone::update(glm::mat4* ts, glm::mat3* f, glm::mat4 pts)
{
	if (pts == glm::mat4(0.0)) pts = this->translation;
	if (f[this->nodeID] == glm::mat3(0.0)) f[this->nodeID] = this->angle;
	ts[this->id] = pts * glm::mat4(f[this->nodeID]);

	glm::mat4 t = ts[this->id] * glm::translate(glm::mat4(1.0), {0, this->length, 0});

	for (auto& b: this->children) { b.update(ts, f, t); }
}

void ae::mesh::Bone::render(glm::mat4* ts, glm::vec3* pts, u16* counter)
{
	pts[*counter * 2 + 0] = ts[*counter] * glm::vec4(0, 0, 0, 1);
	pts[*counter * 2 + 1] = ts[*counter] * glm::vec4(0, this->length, 0, 1);
	(*counter)++;
	for (auto& b: this->children) b.render(ts, pts, counter);
}

ae::u16 ae::mesh::Bone::getID() { return this->id; }
ae::f32 ae::mesh::Bone::getLength() { return this->length; }

ae::mesh::Skeleton::Skeleton()
{
	this->inverseBindMatrices = nullptr;
	this->ts = nullptr;
	this->bones = {};
	this->anims = {};
	this->currentAnim = {};
	glGenBuffers(1, &this->vbo);
}

ae::mesh::Skeleton::~Skeleton()
{
	delete[] this->inverseBindMatrices;
	delete[] this->ts;
	glDeleteBuffers(1, &this->vbo);
}

void ae::mesh::Skeleton::load(gltf::GLTF* file, u8 id)
{
	auto skin = &file->skins[id];

	auto ma = &file->accessors[skin->inverseBindMatrices];
	auto mbv = &file->bufferViews[ma->bufferView];
	auto ibm = (glm::mat4*)&file->buffers[mbv->buffer].data[mbv->byteOffset];

	this->bonesCount = skin->joints.size();
	
	this->ts = new glm::mat4[this->bonesCount];
	this->inverseBindMatrices = new glm::mat4[this->bonesCount];

	auto inherit = new bool[this->bonesCount];
	memset(inherit, 0, this->bonesCount);
	
	for (usize i = 0; i < this->bonesCount; i++)
	{
		this->inverseBindMatrices[i] = ibm[i];
		for (u16 id: file->nodes[skin->joints[i]].children)
		{
			inherit[id] = true;
		}
	}

	u16 counter = 0;
	for (usize i = 0; i < this->bonesCount; i++)
	{
		if (!inherit[i]) this->bones.push_back(Bone(file, i, &counter));
	}
	delete[] inherit;

	for (u8 i = 0; i < file->animations.size(); i++)
	{
		this->anims.insert(anim::loadAnimation(file, i));
	}
}

void ae::mesh::Skeleton::update(f32 dt, ae::Camera* cam)
{
	auto frame = new glm::mat3[this->bonesCount];
	for (usize i = 0; i < this->bonesCount; i++)
	{
		frame[i] = glm::mat3(!this->currentAnim.empty());
		this->ts[i] = glm::mat4(0.0);
	}

	for (usize i = 0; i < this->currentAnim.size(); i++)
	{
		auto anim = &this->anims.at(this->currentAnim[i]);
		anim->currentTime += dt;
		while (anim->currentTime > anim->duration)
		{
			if (anim->repeat) anim->currentTime -= anim->duration;
			else anim->currentTime = anim->duration;
		}

		for (auto& tl: anim->frames)
		{
			if (tl.frames.size() == 1)
			{
				frame[tl.bone] = glm::mat3_cast(tl.frames[0].rotation);
				continue;
			}
			if (anim->currentTime == anim->duration)
			{
				frame[tl.bone] = glm::mat3_cast(tl.frames[tl.frames.size() - 1].rotation);
				continue;
			}

			auto f1 = &tl.frames[tl.currentFrame];
			auto f2 = &tl.frames[tl.currentFrame + 1];

			if (f1->timestamp > anim->currentTime)
			{
				tl.currentFrame = 0;
				f1 = &tl.frames[tl.currentFrame];
				f2 = &tl.frames[tl.currentFrame + 1];
			}

			if (f2->timestamp <= anim->currentTime)
			{
				tl.currentFrame++;
				f1 = &tl.frames[tl.currentFrame];
				f2 = &tl.frames[tl.currentFrame + 1];
			}

			auto t = (anim->currentTime - f1->timestamp) / (f2->timestamp - f1->timestamp);
			frame[tl.bone] = glm::mat3_cast(glm::slerp(
				f1->rotation, f2->rotation,
				ae::gltf::applyInterpolation(tl.func, t)
			));
		}
	}

	for (auto& bone: this->bones)
	{
		bone.update(this->ts, frame, glm::mat4(0.0));
	}

	auto ibm = cam->shaderGetPos("ibm");
	auto ts = cam->shaderGetPos("bones");
	glUniformMatrix4fv(
		ibm, this->bonesCount,
		GL_FALSE, (f32*)this->inverseBindMatrices
	);
	glUniformMatrix4fv(
		ts, this->bonesCount,
		GL_FALSE, (f32*)this->ts
	);

	delete[] frame;
}

void ae::mesh::Skeleton::render(ae::Camera* cam, glm::mat4 ts)
{
	u16 counter = 0;
	auto pts = new glm::vec3[this->bonesCount * 2];
	for (auto& b: this->bones) b.render(this->ts, pts, &counter);
	cam->shaderUse("skeleton");
	cam->bindSkeletonVAO();
	cam->shaderSetModel(ts);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(
		GL_ARRAY_BUFFER, this->bonesCount * 2 * sizeof(glm::vec3),
		pts, GL_STATIC_DRAW
	);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, 12, 0
	);
	glDepthFunc(GL_ALWAYS);
	glDrawArrays(GL_LINES, 0, this->bonesCount * 2);
	glDepthFunc(GL_LESS);
	delete[] pts;
}

void ae::mesh::Skeleton::setAnimation(std::string name)
{
	auto t = this->anims.find(name);
	if (t == this->anims.end()) return;
	for (auto& x: this->currentAnim)
	{
		if (x == name) return;
	}
	t->second.currentTime = 0.0;
	this->currentAnim.push_back(name);
}

void ae::mesh::Skeleton::stopAnimation(std::string name)
{
	for (usize i = 0; i < this->currentAnim.size(); i++)
	{
		if (this->currentAnim[i] == name)
		{
			this->currentAnim[i] = this->currentAnim[this->currentAnim.size() - 1];
			this->currentAnim.pop_back();
		}
	}
}