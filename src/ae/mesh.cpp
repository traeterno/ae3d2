#include <ae/mesh.hpp>
#include <ae/camera.hpp>
#include <ae/global.hpp>

#include <glad/glad.h>

ae::Mesh::Mesh(ae::Camera* camera)
{
	if (camera == nullptr) return;
	this->vbo = camera->createVBO();
	this->ebo = camera->createVBO();
	this->cam = camera;
}

ae::Mesh::~Mesh()
{
	this->vbo = 0;
	this->ebo = 0;
}

void ae::Mesh::load(ae::gltf::GLTF* file, const char* id)
{
	gltf::Node* node = nullptr;
	for (auto x: file->nodes) if (x.name == id) node = &x;
	if (!node)
	{
		printf("Error: Node \"%s\" not found\n", id);
		return;
	}
	if (node->mesh == U16_MAX)
	{
		printf("Error: Node \"%s\" does not contain mesh\n", id);
		return;
	}

	auto m = &file->meshes[node->mesh];
	printf("Loading node \"%s\" as mesh \"%s\"\n", id, m->name.c_str());

	auto va = &file->accessors[m->vertices];
	auto vbv = &file->bufferViews[va->bufferView];
	auto vb = &file->buffers[vbv->buffer];

	auto ia = &file->accessors[m->indices];
	auto ibv = &file->bufferViews[ia->bufferView];
	auto ib = &file->buffers[ibv->buffer];

	auto na = &file->accessors[m->normal];
	auto nbv = &file->bufferViews[na->bufferView];
	auto nb = &file->buffers[nbv->buffer];

	auto ta = &file->accessors[m->texCoord];
	auto tbv = &file->bufferViews[ta->bufferView];
	auto tb = &file->buffers[tbv->buffer];

	auto buf = (u8*)malloc(vbv->byteLength + nbv->byteLength + tbv->byteLength);

	for (usize i = 0 ; i < va->count; i++)
	{
		usize offset = i * 8 * sizeof(f32);
		usize vOffset = vbv->byteOffset + i * 3 * sizeof(f32);
		usize nOffset = nbv->byteOffset + i * 3 * sizeof(f32);
		usize tOffset = tbv->byteOffset + i * 2 * sizeof(f32);
		memcpy(&buf[offset], &vb->data[vOffset], 3 * sizeof(f32));
		memcpy(&buf[offset + 3 * sizeof(f32)], &nb->data[nOffset], 3 * sizeof(f32));
		memcpy(&buf[offset + 6 * sizeof(f32)], &tb->data[tOffset], 2 * sizeof(f32));
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(
		GL_ARRAY_BUFFER, vbv->byteLength + nbv->byteLength + tbv->byteLength,
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

void ae::Mesh::destroy()
{
	this->cam->removeVBO(this->vbo);
	this->cam->removeVBO(this->ebo);
	glDeleteTextures(1, &this->texture);
	this->vbo = 0;
	this->ebo = 0;
	this->texture = 0;
}

void ae::Mesh::render()
{
	this->cam->drawMesh(
		this->vbo,
		this->ebo,
		this->texture,
		this->indices
	);
}

ae::gltf::GLTF* ae::gltf::load(const char* id)
{
	auto src = ae::fs::readJSON(ae::str::format(
		"res/meshes/%s.gltf", id
	));
	if (src.empty())
	{
		printf("Error while loading GLTF \"%s\": file is empty\n", id);
		return nullptr;
	}

	auto f = new GLTF;

	for (auto x: src["buffers"])
	{
		auto uri = ae::str::format("res/meshes/%s", x["uri"].asCString());
		auto len = x["byteLength"].asUInt();
		auto [length, data] = ae::fs::readBinary(uri);
		if (len != length)
		{
			printf("WARNING: GLTF \"%s\": Lengths are not the same(%i|%u)\n",
				x["uri"].asCString(), len, length
			);
		}
		f->buffers.push_back({
			.data = data,
			.byteLength = length
		});
	}

	for (auto x: src["bufferViews"])
	{
		f->bufferViews.push_back({
			.buffer = (u8)x["buffer"].asUInt(),
			.byteOffset = x["byteOffset"].asUInt(),
			.byteLength = x["byteLength"].asUInt(),
			.target = (u16)x["target"].asUInt()
		});
	}

	for (auto x: src["accessors"])
	{
		f->accessors.push_back({
			.bufferView = (u16)x["bufferView"].asUInt(),
			.componentType = (u16)x["componentType"].asUInt(),
			.count = x["count"].asUInt(),
			.type = x["type"].asString()
		});
	}

	f->scene = src["scene"].asUInt();

	for (auto x: src["scenes"])
	{
		Scene s;
		for (auto y: x["nodes"])
		{
			s.nodes.push_back(y.asUInt());
		}
		f->scenes.push_back(s);
	}

	for (auto x: src["nodes"])
	{
		Node n {
			.children = {},
			.mesh = U16_MAX,
			.skin = U16_MAX,
			.name = x["name"].asString(),
			.rotation = glm::quat(),
			.translation = glm::vec3(),
			.scale = glm::vec3()
		};
		if (x["mesh"].isUInt()) { n.mesh = x["mesh"].asUInt(); }
		if (x["skin"].isUInt()) { n.skin = x["skin"].asUInt(); }
		if (x["rotation"].isArray())
		{
			auto r = x["rotation"];
			n.rotation = glm::quat(
				r[3].asFloat(), r[0].asFloat(),
				r[1].asFloat(), r[2].asFloat()
			);
		}
		if (x["translation"].isArray())
		{
			auto t = x["translation"];
			n.translation = glm::vec3(
				t[0].asFloat(), t[1].asFloat(), t[2].asFloat()
			);
		}
		if (x["scale"].isArray())
		{
			auto s = x["scale"];
			n.scale = glm::vec3(
				s[0].asFloat(), s[1].asFloat(), s[2].asFloat()
			);
		}
		for (auto y: x["children"])
		{
			n.children.push_back(y.asUInt());
		}
		f->nodes.push_back(n);
	}
	
	for (auto x: src["meshes"])
	{
		auto p = x["primitives"][0];
		auto a = p["attributes"];
		f->meshes.push_back({
			.name = x["name"].asString(),
			.vertices = (u16)a["POSITION"].asUInt(),
			.normal = (u16)a["NORMAL"].asUInt(),
			.texCoord = (u16)a["TEXCOORD_0"].asUInt(),
			.joints = (u16)a["JOINTS_0"].asUInt(),
			.weights = (u16)a["WEIGHTS_0"].asUInt(),
			.indices = (u16)p["indices"].asUInt(),
			.material = (u8)p["material"].asUInt()
		});
	}

	for (auto x: src["materials"])
	{
		f->materials.push_back({
			.name = x["name"].asString(),
			.texture = (u8)x["pbrMetallicRoughness"]
				["baseColorTexture"]["index"].asUInt()
		});
	}

	for (auto x: src["textures"])
	{
		f->textures.push_back({
			.sampler = (u8)x["sampler"].asUInt(),
			.source = (u8)x["source"].asUInt()
		});
	}

	for (auto x: src["samplers"])
	{
		f->samplers.push_back({
			.magFilter = (u16)x["magFilter"].asUInt(),
			.minFilter = (u16)x["minFilter"].asUInt(),
			.wrapS = (u16)x["wrapS"].asUInt(),
			.wrapT = (u16)x["wrapT"].asUInt()
		});
	}

	for (auto x: src["images"])
	{
		f->images.push_back({
			.mimeType = x["mimeType"].asString(),
			.name = x["name"].asString(),
			.uri = x["uri"].asString()
		});
	}

	return f;
}