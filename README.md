# Voxelman

A voxel engine for godot, focusing more on editor integration, gameplay-related features, and extendability (even from gdscript), without sacrificing too much speed.

This is an engine module! Which means that you will need to compile it into Godot! [See the compiling section here.](#compiling)

You can find a demonstration project (with pre-built binaries) here: https://github.com/Relintai/the_tower

## Godot Version Support

I'm currently mostly using [Terraman](https://github.com/Relintai/terraman) instead of this, so it might get temporarily a bit behind.\
If compile breaks, and I don't notice please report.

3.2 - Will likely work, probably needs changes by now. (TODO check.)\
3.3 - Will more likely work, might need smaller changes by now. (TODO check.)\
3.4 - Should work without any issues. (TODO check.)\
3.x - Works. [last tested commit](6ea58db2d849d9ca0ccee5bc6a6d2b919d404bc1)\
4.0 - Have been fixing support from time to time. Currently it won't build. Mostly done with the fix though.

## Optional Dependencies

`https://github.com/Relintai/thread_pool`: Threaded chunk generation. Without this Voxelman is single threaded! \
`https://github.com/Relintai/texture_packer`: You get access to [VoxelLibraryMerger](#voxellibrarymerger) and [VoxelLibraryMergerPCM](#voxellibrarymergerpcm). \
`https://github.com/Relintai/mesh_data_resource`: You get access to a bunch of properties, and methods that can manipulate meshes.\
`https://github.com/Relintai/props`: You get access to a bunch of properties, and methods that can manipulate, and use props.\
`https://github.com/Relintai/mesh_utils`: Lets you use lod levels higher than 4 by default.

## Usage

First create a scene, and add a VoxelWorldBlocky node into it. Create a VoxelLibrary, and assign it to the Library property.
Also, add a VoxelSurface into your library.

(VoxelWorldBlocky is the only one that works properly for now, this will soon be fixed!)

Tick the editable property, deselect, then select the world again, and click the insert button at the top toolbar, or press B to insert a
voxel at the inspector's camera's location.

Select the add button, and now you can just add voxels with the mouse, by clicking on the newly added voxel.

## VoxelLibrary

This class stores the materials, and the VoxelSurfaces.

Note: If you want lods, assign equal (or more) materials than your maximum lod level. If you only want one material just assign it 
multiple times. If you don't then your meshes won't have materials (They will be white).

### VoxelLibrarySimple

The simplest library, just assign a material with a texture, and using the atlas_rows and atlas_culomns properties to tell the system
how the UVs should be divided.

### VoxelLibraryMerger

You will only have this if your godot also contains https://github.com/Relintai/texture_packer

You can assign any texture to your surfaces with this, and it will merge them together.

### VoxelLibraryMergerPCM

(PCM = Per Chunk Material)

You will only have this if your godot also contains https://github.com/Relintai/texture_packer

You can assign any texture to your surfaces with this, and it will merge them together, but it will do it for every required chunk/voxel combination.

For example if you have a chunk with voxel Grass, and voxel Stone used in it, this library will create a material with a merged texture for Stone and Grass.
If you have an anouther chunk which only has Grass and Stone in it, this material will be reused.
And if you have a third chunk which only has a Grass voxel used in it, it will get a new merged material and texture only containing Grass voxel.

## Worlds

The 2 base classes:

VoxelWorld: Basic world, does not do anything until you implemnent the required virtual methods!\
VoxelWorldDefault: This adds threading, and LoD storage support to VoxelWorld. Will not create meshes for you!

### VoxelWorldBlocky

The most basic world. It is the Minecraft-style world.

### VoxelWorldMarchingCubes

A marching cubes based Voxel World. Actually it uses a modified version of the Transvoxel tables. It is UV mapped.

### VoxelWorldCubic

This is my own meshing algorithm, it's basicly a Minecraft style mesher that can take isolevel into account.

### Level generation

Assign a VoxelManLevelGenerator to the `World`'s `Level Generator` property.

You can write your own algorithm by implementing the ``` void _generate_chunk(chunk: VoxelChunk) virtual ``` method.

`VoxelManLevelGeneratorFlat` is also available, it will generate a floor for you, if you use it.

## VoxelJobs

Producing just a terrain mesh for a chunk is not that hard by itself. However when you start adding layers/features
like lod generation, collision meshes (especially since manipulating the physics server is not threadsafe), 
vertex volumetric lights, props, snapping props, props with vertex lights, etc
chunk mesh generation can quicly become a serious mess.

VoxelJobs are meant to solve the issue with this complexity.

They also provide a way to easily modularize mesh generation.

### VoxelJob

Base class for jobs.

If the (thread pool)[https://github.com/Relintai/thread_pool] module is present, this is inherited from `ThreadPoolJob`,
else it implements the same api as `ThreadPoolJob`, but it's not going to use threading.

A job has a reference to it's owner chunk.

If you implement your own jobs, when your job finishes call `next_job()`.

### VoxelLightJob

This is the job that will generate vertex light based ao, random ao, and will bake your `VoxelLight`s.

### VoxelTerrainJob

This will generate your terrain collider and mesh (with lods) for you, using the meshers that you add into it.

### VoxelPropJob

This will generate your prop meshes (with lods).

### Internal workings

#### VoxelWorld

Whenever you want to spawn a chunk your World will create it using the ``` VoxelChunk _create_chunk(x: int, y: int, z: int, chunk: VoxelChunk) virtual ``` method.

Since properly initializing a chunk usually takes quite a few steps that you probably don't want to repeat everywhere the `chunk`
parameter was added. This means you can just call the super `_create_chunk` methods, and you won't need to worry about your chunk
getting overridden. Like:

Note that `_create_chunk` is also responsible for initializing chunks if you have them stored inside a scene. 
This is done by `setup_chunk(shunk)` in `VoxelWorld`.

``` 
    func _create_chunk(x : int, y : int, z : int, chunk : VoxelChunk) -> VoxelChunk:
        if !chunk:
            chunk = MyChunk.new()

        # We need to check whether or not we need to initialize jobs
        if chunk.job_get_count() == 0:
            # Setup a blocky (minecratf like) mesher job
            var tj : VoxelTerrainJob = VoxelTerrainJob.new()

            tj.add_mesher(VoxelMesherBlocky.new())
            tj.add_liquid_mesher(VoxelMesherLiquidBlocky.new())

            chunk.job_add(tj);

        #setup your chunk here

        return ._create_chunk(x, y, z, chunk)
```

#### VoxelChunk

Stores terrain data, prop data. And mesh data (VoxelChunkDefault), and the mesh generation jobs.

When it starts building meshes it will start submitting jobs to thread_pool (if present) one by one.

#### VoxelMesher

If you want to implement your own meshing algorithm you can do so by overriding ``` void _add_chunk(chunk: VoxelChunk) virtual ```.

VoxelMesher works similarly to SurfaceTool, so first you need to set colors, uvs, etc and then call add_vertex.
They won't get reset, so for exaple if you want all your vertices to have a certain color, you can get away with setting it only once.

## Compiling

First make sure that you can compile godot. See the official docs: https://docs.godotengine.org/en/3.2/development/compiling/index.html

1. Clone the engine if you haven't already:

If you want Godot 3.x:
```git clone -b 3.x https://github.com/godotengine/godot.git godot```

If you want Godot 4.0:
```git clone https://github.com/godotengine/godot.git godot```


2. go into the modules folder inside the engine's directory:

```cd godot``` \
```cd modules```

3. clone this repository

```git clone https://github.com/Relintai/voxelman.git voxelman```

(the folder needs to be named voxelman!)

4. If you want the optional dependencies run these commands aswell:

```git clone https://github.com/Relintai/texture_packer.git texture_packer``` \
```git clone https://github.com/Relintai/mesh_data_resource.git mesh_data_resource```

5. Go up one folder

```cd ..```

6. Compile godot.

For example:

```scons p=x11 t=release_debug tools=yes```
