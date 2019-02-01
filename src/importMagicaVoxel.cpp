
#include "importMagicaVoxel.h"
#include <stdexcept>
#include "utils.h"


int MV::id( int a, int b, int c, int d ) {
    return ( a ) | ( b << 8 ) | ( c << 16 ) | ( d << 24 );
}


glm::vec4 MV::intToColorVec4(int u) {
    float b = u & 0xff;
    u >>= 8;
    float g = u & 0xff;
    u >>= 8;
    float r = u & 0xff;
    u >>= 8;
    float a = u & 0xff;
    return glm::vec4(r, g, b, a) * (1.0f / 256.0f);
}

glm::vec4 MV::rgbaToColorVec4(MV::RGBA rgba) {
    return glm::vec4(rgba.r, rgba.g, rgba.b, rgba.a) * (1.0f / 256.0f);
}



void MV::ModelLoader::ReadChunk( std::ifstream &file, chunk_t &chunk ) {
    // read chunk
    file.read((char*) &chunk.id, sizeof(int));
    file.read((char*) &chunk.contentSize, sizeof(int));
    file.read((char*) &chunk.childrenSize, sizeof(int));

    int pos = file.tellg();
    
    // end of chunk : used for skipping the whole chunk
    chunk.end = pos + chunk.contentSize + chunk.childrenSize;
    
    // print chunk info
    // const char *c = ( const char * )( &chunk.id );
    // printf( "[Log] VoxelModelLoader :: Chunk : %c%c%c%c : %d %d\n",
    //        c[0], c[1], c[2], c[3],
    //        chunk.contentSize, chunk.childrenSize
    //        );
}



bool MV::ModelLoader::ReadModelLoaderFile( std::ifstream &file ) {
    const int MV_VERSION = 150;
    
    const int ID_VOX  = id( 'V', 'O', 'X', ' ' );
    const int ID_MAIN = id( 'M', 'A', 'I', 'N' );
    const int ID_SIZE = id( 'S', 'I', 'Z', 'E' );
    const int ID_XYZI = id( 'X', 'Y', 'Z', 'I' );
    const int ID_RGBA = id( 'R', 'G', 'B', 'A' );
    //const int ID_PACK = id( 'P', 'A', 'C', 'K' );
    
    // magic number
    int magic;
    file.read((char*) &magic, sizeof(int)); 
    //file >> magic;
    if ( magic != ID_VOX ) {
        Error( "magic number does not match" );
        return false;
    }
    
    // version
    int version;
    file.read((char*) &version, sizeof(int));
    if ( version != MV_VERSION ) {
        Error( "version does not match" );
        return false;
    }
    
    // main chunk
    chunk_t mainChunk;
    ReadChunk( file, mainChunk );
    if ( mainChunk.id != ID_MAIN ) {
        Error( "main chunk is not found" );
        return false;
    }
    
    // skip content of main chunk
    file.seekg(mainChunk.contentSize, std::ios_base::cur);
    
    Model currentModel;
    // read children chunks
    while (file.tellg() < mainChunk.end) {
        // read chunk header
        chunk_t sub;
        ReadChunk( file, sub );
        
        if ( sub.id == ID_SIZE ) {
            currentModel = Model();
            // size
            file.read((char*) &currentModel.sizex, sizeof(int));
            file.read((char*) &currentModel.sizey, sizeof(int));
            file.read((char*) &currentModel.sizez, sizeof(int));
        }
        else if ( sub.id == ID_XYZI ) {
            // numVoxels
            file.read((char*) &currentModel.numVoxels, sizeof(int));

            if ( currentModel.numVoxels < 0 ) {
                Error( "negative number of voxels" );
                return false;
            }
            
            // voxels
            if ( currentModel.numVoxels > 0 ) {
                currentModel.voxels = new Voxel[ currentModel.numVoxels ];
                file.read((char*) currentModel.voxels, currentModel.numVoxels * sizeof(int));
            }

            models.push_back(currentModel);
        }
        else if ( sub.id == ID_RGBA ) {
            // last color is not used, so we only need to read 255 colors
            isCustomPalette = true;
            file.read((char*) palette + 1, sizeof(RGBA) * 255);

            // NOTICE : skip the last reserved color
            RGBA reserved;
            file.read((char*) &reserved, sizeof(RGBA));
        }

        // skip unread bytes of current chunk or the whole unused chunk
        file.seekg(sub.end, std::ios::beg);
    }
    
    // print ModelLoader info
    // printf( "[Log] VoxelModelLoader :: ModelLoader : %d %d %d : %d\n",
    //        sizex, sizey, sizez, numVoxels
    //        );
    
    return true;
}



TileMap3d* MV::makeTileMapSingle(const MV::Model &model, bool isCustomPalette, const MV::RGBA* palette, bool makeMesh) {
    std::vector<Tile> pal;
    Tile t;
    t.color = glm::vec4(0.0, 0.0, 0.0, 1.0);
    pal.push_back(t);
	if (isCustomPalette) {
		for (int i = 0; i < 255; i++) {
			Tile tile = {MV::rgbaToColorVec4(palette[i])};
			pal.push_back(tile);
		}
	} else {
		for (int i = 0; i < 255; i++) {
			Tile tile = {MV::intToColorVec4(MV::default_palette[i])};
			pal.push_back(tile);
		}
	}

	TileMap3d* tilemap = new TileMap3d(pal, model.sizex, model.sizey, model.sizez);

	for (int i = 0; i < model.numVoxels; i++) {
		MV::Voxel v = model.voxels[i];
		tilemap->set(v.x, v.y, v.z, v.colorIndex);
	}
    if (makeMesh) {
        tilemap->updateMesh();
    }
    return tilemap;
}



std::vector<TileMap3d*> MV::makeTileMapsFromFile(const char* path, bool makeMeshes, bool &success) {
    MV::ModelLoader modelLoader;
    success = modelLoader.loadModel(path);
    if (!success) {
        throw std::runtime_error("File broken!");
    }
    std::vector<MV::Model> models = modelLoader.models;
    std::vector<TileMap3d*> tilemaps;
    for (unsigned int i = 0; i < models.size(); i++) {
        MV::Model model = models[i];
        TileMap3d* tm = MV::makeTileMapSingle(model, modelLoader.isCustomPalette, modelLoader.palette, makeMeshes);
        tilemaps.push_back(tm);
    }

    modelLoader.free();
    return tilemaps;
}
