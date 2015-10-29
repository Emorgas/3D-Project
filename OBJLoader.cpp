#include "OBJLoader.h"
#include <string>

bool OBJLoader::FindSimilarVertex(const SimpleVertex& vertex, std::map<SimpleVertex, unsigned short>& vertToIndexMap, unsigned short& index)
{
	std::map<SimpleVertex, unsigned short>::iterator it = vertToIndexMap.find(vertex);
	if(it == vertToIndexMap.end())
	{
		return false;
	}
	else
	{
		index = it->second;
		return true;
	}
}

void OBJLoader::CreateIndices(const std::vector<XMFLOAT3>& inVertices, const std::vector<XMFLOAT2>& inTexCoords, const std::vector<XMFLOAT3>& inNormals, std::vector<unsigned short>& outIndices, std::vector<XMFLOAT3>& outVertices, std::vector<XMFLOAT2>& outTexCoords, std::vector<XMFLOAT3>& outNormals)
{
	//Mapping from an already-existing SimpleVertex to its corresponding index
	std::map<SimpleVertex, unsigned short> vertToIndexMap;

	int numVertices = inVertices.size();
	for(int i = 0; i < numVertices; ++i) //For each vertex
	{
		SimpleVertex vertex = { inVertices[i], inTexCoords[i], inNormals[i]};

		unsigned short index;
		bool found = FindSimilarVertex(vertex, vertToIndexMap, index); //See if a vertex already exists in the buffer that has the same attributes as this one
		if(found) //if found, re-use it's index for the index buffer
		{
			outIndices.push_back(index);
		}
		else //if not found, add it to the buffer
		{
			outVertices.push_back(vertex.Pos);
			outTexCoords.push_back(vertex.TexCoord);
			outNormals.push_back(vertex.normal);

			unsigned short newIndex = (unsigned short)outVertices.size() - 1;
			outIndices.push_back(newIndex);

			//Add it to the map
			vertToIndexMap[vertex] = newIndex;
		}
	}
}

//WARNING: This code makes a big assumption -- that your models have texture coordinates AND normals which they should have anyway (else you can't do texturing and lighting!)
//If your .obj file has no lines beginning with "vt" or "vn", then you'll need to change the Export settings in your modelling software so that it exports the texture coordinates 
//and normals. If you still have no "vt" lines, you'll need to do some texture unwrapping, also known as UV unwrapping.
MeshData OBJLoader::Load(char* filename, ID3D11Device* _pd3dDevice, bool invertTexCoords)
{
	std::ifstream inFile;
	inFile.open(filename);

	if(!inFile.good())
	{
		return MeshData();
	}
	else
	{
		//Vectors to store the vertex positions, normals and texture coordinates. Need to use vectors since they're resizeable and we have
		//no way of knowing ahead of time how large these meshes will be
		std::vector<XMFLOAT3> verts;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texCoords;

		//DirectX uses 1 index buffer, OBJ is optimized for storage and not rendering and so uses 3 smaller index buffers.....great...
		//We'll have to merge this into 1 index buffer which we'll do after loading in all of the required data.
		std::vector<unsigned short> vertIndices;
		std::vector<unsigned short> normalIndices;
		std::vector<unsigned short> textureIndices;

		std::string input;

		while(!inFile.eof()) //While we have yet to reach the end of the file...
		{
			inFile >> input; //Get the next input from the file

			//Check what type of input it was, we are only interested in vertex positions, texture coordinates, normals and indices, nothing else
			if(input.compare("v") == 0) //Vertex position
			{
				XMFLOAT3 vert;

				inFile >> vert.x;
				inFile >> vert.y;
				inFile >> vert.z;

				verts.push_back(vert);
			}
			else if(input.compare("vt") == 0) //Texture coordinate
			{
				XMFLOAT2 texCoord;

				inFile >> texCoord.x;
				inFile >> texCoord.y;

				if(invertTexCoords) texCoord.y = 1.0f - texCoord.y;

				texCoords.push_back(texCoord);
			}
			else if(input.compare("vn") == 0) //Normal
			{
				XMFLOAT3 normal;

				inFile >> normal.x;
				inFile >> normal.y;
				inFile >> normal.z;

				normals.push_back(normal);
			}
			else if(input.compare("f") == 0) //Face
			{
				unsigned short vInd[3]; //indices for the vertex position
				unsigned short tInd[3]; //indices for the texture coordinate
				unsigned short nInd[3]; //indices for the normal

				for(int i = 0; i < 3; ++i)
				{
					inFile >> input;
					int slash = input.find("/"); //Find first forward slash
					int secondSlash = input.find("/", slash + 1); //Find second forward slash

					//Extract from string
					std::string beforeFirstSlash = input.substr(0, slash); //The vertex position index
					std::string afterFirstSlash = input.substr(slash + 1, secondSlash - slash - 1); //The texture coordinate index
					std::string afterSecondSlash = input.substr(secondSlash + 1); //The normal index

					//Parse into int
					vInd[i] = (unsigned short)atoi(beforeFirstSlash.c_str()); //atoi = "ASCII to int"
					tInd[i] = (unsigned short)atoi(afterFirstSlash.c_str());
					nInd[i] = (unsigned short)atoi(afterSecondSlash.c_str());
				}

				//Place into vectors
				for(int i = 0; i < 3; ++i)
				{
					vertIndices.push_back(vInd[i] - 1);		//Minus 1 from each as these as OBJ indexes start from 1 whereas C++ arrays start from 0
					textureIndices.push_back(tInd[i] - 1);	//which is really annoying. Apart from Lua and SQL, there's not much else that has indexing 
					normalIndices.push_back(nInd[i] - 1);	//starting at 1. So many more languages index from 0, the .OBJ people screwed up there.
				}
			}
		}
		inFile.close(); //Finished with input file now, all the data we need has now been loaded in

		//Get vectors to be of same size, ready for singular indexing
		std::vector<XMFLOAT3> expandedVertices;
		std::vector<XMFLOAT3> expandedNormals;
		std::vector<XMFLOAT2> expandedTexCoords;
		unsigned int numIndices = vertIndices.size();
		for(unsigned int i = 0; i < numIndices; i++)
		{
			expandedVertices.push_back(verts[vertIndices[i]]);
			expandedTexCoords.push_back(texCoords[textureIndices[i]]);
			expandedNormals.push_back(normals[normalIndices[i]]);
		}

		//Now to (finally) form the final vertex, texture coord, normal list and single index buffer using the above expanded vectors
		std::vector<unsigned short> meshIndices(numIndices);
		std::vector<XMFLOAT3> meshVertices(expandedVertices.size());
		std::vector<XMFLOAT3> meshNormals(expandedNormals.size());
		std::vector<XMFLOAT2> meshTexCoords(expandedTexCoords.size());
		CreateIndices(expandedVertices, expandedTexCoords, expandedNormals, meshIndices, meshVertices, meshTexCoords, meshNormals);

		MeshData meshData;

		//Turn data from vector form to arrays
		SimpleVertex* finalVerts = new SimpleVertex[meshVertices.size()];
		unsigned int numMeshVertices = meshVertices.size();
		for(unsigned int i = 0; i < numMeshVertices; ++i)
		{
			finalVerts[i].Pos = meshVertices[i];
			finalVerts[i].normal = meshNormals[i];
			finalVerts[i].TexCoord = meshTexCoords[i];
		}

		//Put data into vertex and index buffers, then pass the relevant data to the MeshData object.
		//The rest of the code will hopefully look familiar to you, as it's similar to whats in your InitVertexBuffer and InitIndexBuffer methods
		ID3D11Buffer* vertexBuffer;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * meshVertices.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = finalVerts;

		_pd3dDevice->CreateBuffer(&bd, &InitData, &vertexBuffer);

		meshData.VertexBuffer = vertexBuffer;
		meshData.VBOffset = 0;
		meshData.VBStride = sizeof(SimpleVertex);

		unsigned short* indicesArray = new unsigned short[meshIndices.size()];
		unsigned int numMeshIndices = meshIndices.size();
		for(unsigned int i = 0; i < numMeshIndices; ++i)
		{
			indicesArray[i] = meshIndices[i];
		}

		ID3D11Buffer* indexBuffer;

		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * meshIndices.size();     
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = indicesArray;
		_pd3dDevice->CreateBuffer(&bd, &InitData, &indexBuffer);

		meshData.IndexCount = meshIndices.size();
		meshData.IndexBuffer = indexBuffer;

		//This data has now been sent over to the GPU so we can delete this CPU-side stuff
		delete [] indicesArray;
		delete [] finalVerts;

		return meshData;
	}	
}